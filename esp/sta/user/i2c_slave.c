/******************************************************************************
 *
 * FileName: i2c_slave.c
 *
 * Description: I2C slave implementation.
 *
 * Modification history:
 *     2015/1/4, v1.0 create this file.
*******************************************************************************/

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "user_interface.h"

#include "i2c_slave.h"
#include "user_global_definitions.h"
#include "user_state.h"
#include "user_tcpclient.h"

#define READ_PIN(pin) (!!(PIN_IN & ( 1  << pin )))    // outputs 0 or 1
#define I2C_READ_PIN(pin) (PIN_IN & ( 1  << pin ))
#define I2C_SDA_SET(value) ((value > 0) ? (PIN_OUT_SET = 1 << SDA_PIN) : (PIN_OUT_CLEAR = 1 << SDA_PIN))

uint8_t    i2c_byte_buffer[65]; 
volatile uint8_t    i2c_buffer; 
volatile int8_t     i2c_bit_number;  
int8_t    i2c_byte_number; 
volatile int8_t     clockpulses;        // Debug
static volatile os_timer_t timer1;
static uint8_t i2c_status;

static void i2c_slave_reading_start();
static void i2c_slave_reading_address();
static void i2c_slave_writing_address();
static void i2c_return_interrupt(); 
static void user_i2c_debug(void);

void ICACHE_FLASH_ATTR 
i2c_slave_init(void)
{
    DEBUG_0(os_printf("I2C: init\n")); 
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);            // SET GPIO function, not uart...
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);            // SET GPIO function, not uart...
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);            // GPIO function, instead of U0RXD

    gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));                 // Configure SDA to input
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(SCL_PIN));                 // Configure SCL to input

    GPIO_OUTPUT_SET(3, 1);                                          // Set gpio 3 to output high, so i2c bus can connect

    // dissable todo reset if not responding...
    //DEBUG_1(user_i2c_debug());

    i2c_update_status(I2C_READING_START);                           // Starting reading
    ETS_GPIO_INTR_ENABLE();                                         // Enable gpio interrupts
}

void ICACHE_FLASH_ATTR 
i2c_slave_stop(void)
{
    DEBUG_0(os_printf("I2C: stop\n")); 
    ETS_GPIO_INTR_DISABLE();                                // Disable Interrupts
    i2c_update_status(I2C_IDLE);                            // Disable interrupt on specific pin.  
}

/**
*  trigger
*  0: disable
*  1: positive edge
*  2: negative edge
*  3: any egde
*/
void
i2c_update_status(uint8_t status)
{
    // We're assuming that interrupts are disabled
    if(status == I2C_READING_BYTES && i2c_status == I2C_READING_ADDRESS) {
        i2c_status = status; 
        return;     // do nothing
    }

    // Disable interrupts
    gpio_pin_intr_state_set(SDA_PIN, 0); 
    gpio_pin_intr_state_set(SCL_PIN, 0); 


    i2c_status = status;

    switch(status) 
    {
        case I2C_IDLE: 
            break; 
        case I2C_READING_START:
            ETS_GPIO_INTR_ATTACH(i2c_slave_reading_start, SDA_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SDA_PIN, 2);                            // Interrupt on negative edge

            i2c_buffer = 0; 
            i2c_bit_number = 7; 
            break;
        case I2C_READING_ADDRESS:
            ETS_GPIO_INTR_ATTACH(i2c_slave_reading_address, SCL_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
            i2c_bit_number = 7; 
            i2c_byte_number = -1; 
            break;
        case I2C_READING_BYTES:
            ETS_GPIO_INTR_ATTACH(i2c_slave_reading_address, SCL_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
            i2c_bit_number = 7; 
            break;
        case I2C_WRITING_BYTES:
            ETS_GPIO_INTR_ATTACH(i2c_slave_writing_address, SCL_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SCL_PIN, 2);                            // Interrupt on negative edge
            i2c_bit_number = 8; // One for disable ack 
            break;
    }
}

static void
i2c_slave_reading_address() {
    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
    clockpulses++; 

    //while(!I2C_READ_PIN(SCL_PIN));               // Wait till SCL is low

    if(i2c_bit_number > 0) {
        i2c_buffer |= READ_PIN(SDA_PIN) << i2c_bit_number;
        i2c_bit_number--; 

    } else if(i2c_bit_number == 0) {
        i2c_buffer |= READ_PIN(SDA_PIN) << i2c_bit_number;
        i2c_bit_number--; 


        if(i2c_status == I2C_READING_ADDRESS) {
            if((i2c_buffer >> 1) != I2C_SLAVE_ADDRESS) {
                DEBUG_2(os_printf("I2C: Reading restart, received address: %d, 0x%x \n", i2c_buffer, i2c_buffer)); 
                i2c_update_status(I2C_READING_START);
                return i2c_return_interrupt(); 
            }
            if((i2c_buffer & 1) == 0) {
                i2c_update_status(I2C_READING_BYTES);
            } else {
                i2c_update_status(I2C_WRITING_BYTES);
            }
        }

        while(I2C_READ_PIN(SCL_PIN));               // Wait till SCL is low
        PIN_OUT_SET = 1 << SDA_PIN;                 // Sent an ACK
        GPIO_OUTPUT_SET(SDA_PIN, 0);
    
    } else {
        while(I2C_READ_PIN(SCL_PIN));               // Wait till SCL is low
        PIN_DIR_INPUT = 1 << SDA_PIN;               // ACK is sent so set pin direction to input
        gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));

        if(i2c_byte_number >= 0)
            i2c_byte_buffer[i2c_byte_number] = i2c_buffer;      // Save received data


        if(i2c_byte_number == 1 && i2c_byte_buffer[0] == 255)               // Received command... 
        {
            tcpclient_update_state(i2c_byte_buffer[1]);

            os_delay_us(5000);

            i2c_update_status(I2C_READING_START);
            return i2c_return_interrupt(); 
        }

        if(i2c_byte_number > 0 && i2c_byte_number >= i2c_byte_buffer[0]) 
        {
            // Sending bytes ?
            os_printf("Received %d bytes\n", i2c_byte_number); 

            if(tcpclient_get_state() == STATE_CONNECTED)
                tcpclient_send_data(&i2c_byte_buffer[1], i2c_byte_number, KEEP_ALIVE); 

            i2c_update_status(I2C_READING_START);
            return i2c_return_interrupt(); 
        }

        i2c_byte_number++;  
        i2c_buffer = 0;                             // Clear tempory values
        i2c_bit_number = 7; 
    }

    i2c_return_interrupt(); 
}

static void
i2c_slave_writing_address()
{
    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
    clockpulses++;

    if(i2c_bit_number == 8) while(I2C_READ_PIN(SCL_PIN));   // Wait till SCL is low

    if(i2c_bit_number > 0) {
        I2C_SDA_SET((tcpclient_get_state() & (1 << (i2c_bit_number - 1)))); 
    } else if(i2c_bit_number == 0) {
        PIN_DIR_INPUT = 1 << SDA_PIN; 
        gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));
        
        while(!I2C_READ_PIN(SCL_PIN));                      // Wait until SCL become high

        if(I2C_READ_PIN(SDA_PIN) > 0) {
            tcpclient_update_state(STATE_CONNECT);

            i2c_update_status(I2C_READING_START);           // Received NACK
            i2c_return_interrupt(); 
            DEBUG_2(os_printf("Writing status done, received nack from master\n"));
            return; 
        } else {
            i2c_bit_number == 7;                            // Received ACK
            DEBUG_2(os_printf("Received ack? writing status is 1 byte\n"));

            i2c_update_status(I2C_READING_START);           // Received NACK
            i2c_return_interrupt(); 
        }
    }

    i2c_bit_number--;

    i2c_return_interrupt(); 
}

static void
i2c_return_interrupt() 
{
    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);                  // Clear interrupt status

    ETS_GPIO_INTR_ENABLE();                                                 // Enable gpio interrupts
}

void ICACHE_FLASH_ATTR
print_debug_info(void *arg)
{
    if(clockpulses != 0)
    {
        os_printf("DEBUG, count: %d, clockpulses: %d\n", i2c_bit_number, clockpulses);

        os_printf("BUFFER: %d, %d, %d \n", i2c_byte_buffer[0], i2c_byte_buffer[1], i2c_byte_buffer[2]);
    }

    clockpulses = 0; 

    //temp fix
    PIN_DIR_INPUT = 1 << SDA_PIN; 
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));


    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts

    i2c_update_status(I2C_READING_START);

    os_memset(&i2c_byte_buffer[0], 0, sizeof(i2c_byte_buffer));

    // stime = system_get_time(); 
    // reading_address = system_get_time() - stime; 

    ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR
i2c_soft_wd(void *arg)
{
    PIN_DIR_INPUT = 1 << SDA_PIN; 
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));

    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
    i2c_update_status(I2C_READING_START);
    ETS_GPIO_INTR_ENABLE();
}

// Important to keep this function at this position...
// Otherwise switching interrupts is to late... 
static void ICACHE_FLASH_ATTR
i2c_slave_reading_start() {
    ETS_GPIO_INTR_DISABLE();                                // Disable gpio interrupts

    if(I2C_READ_PIN(SCL_PIN))                               // If valid I2C start condition is sent
    {
        i2c_update_status(I2C_READING_ADDRESS);             // Start reading an address
    }

    i2c_return_interrupt();
}

static void ICACHE_FLASH_ATTR 
user_i2c_debug(void)
{
    os_printf("I2C: Debug on\n"); 
    //Disarm timer
    os_timer_disarm(&timer1);

    //Setup timer
    os_timer_setfn(&timer1, (os_timer_func_t *)i2c_soft_wd, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&timer1, 100, 1);
}