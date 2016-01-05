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

static volatile os_timer_t timer1;

extern volatile uint32_t PIN_OUT;
extern volatile uint32_t PIN_OUT_SET;
extern volatile uint32_t PIN_OUT_CLEAR;

extern volatile uint32_t PIN_DIR;
extern volatile uint32_t PIN_DIR_OUTPUT;
extern volatile uint32_t PIN_DIR_INPUT;

extern volatile uint32_t PIN_IN;

volatile int8_t clockpulses;  
volatile uint8_t i2c_temp; 
volatile int8_t i2c_bit_number; 
volatile uint8_t i2c_buffer[10];  
volatile uint8_t i2c_byte_num; 

volatile int8_t reading_bytes; 

volatile uint32 stime;

#define I2C_XXX 0x00
#define I2C_READING_START 0x01
#define I2C_READING_ADDRESS 0x02
#define I2C_READING_BYTES 0x03

#define I2C_BYTES 2

#define SDA_PIN 0x02
#define SCL_PIN 0x00

static uint8_t i2c_status;

uint8_t i2c_read_pin(uint8_t pin)
{
    if((PIN_IN & ( 1  << pin )) == 0)
        return 0; 
    else 
        return 1;
}

void i2c_update_status(uint8_t status) 
{ 
    // We're assuming that interrupts are disabled
    //ETS_GPIO_INTR_DISABLE();                                        // Disable gpio interrupts
    if(status == I2C_READING_BYTES && i2c_status == I2C_READING_ADDRESS) {
        i2c_status = status; 
        return;     // do nothing
    }
    i2c_status = status;

    // Disable interrupts
    gpio_pin_intr_state_set(SDA_PIN, 0); 
    gpio_pin_intr_state_set(SCL_PIN, 0); 

    switch(status) 
    {
        case I2C_READING_START:
            i2c_status = I2C_READING_START;
            ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_start, SDA_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SDA_PIN, 2);                            // Interrupt on negative edge

            i2c_temp = 0; 
            i2c_bit_number = 7; 
            i2c_byte_num = 0; 
            break;
        case I2C_READING_ADDRESS:
            i2c_status = I2C_READING_START;
            ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_address, SCL_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
            i2c_bit_number = 7; 
            break;
        case I2C_READING_BYTES:
            i2c_status = I2C_READING_BYTES;
            ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_address, SCL_PIN);      // GPIO2 interrupt handler
            gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
            i2c_bit_number = 7; 
            break;
    } 

    //ETS_GPIO_INTR_ENABLE();
}

void i2c_interupt_reading_address() {
    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
    clockpulses++; 

    if(i2c_bit_number > 0) {
        i2c_temp |= GPIO_INPUT_GET(SDA_PIN) << i2c_bit_number;
        i2c_bit_number--; 

    } else if(i2c_bit_number == 0) {
        i2c_temp |= GPIO_INPUT_GET(SDA_PIN) << i2c_bit_number;
        i2c_bit_number--; 



        
        if(i2c_status == I2C_READING_ADDRESS) {
            if(i2c_temp != 0xAA) {
                os_printf("Reading restart %d \n", i2c_temp);
                i2c_update_status(I2C_READING_START);
            } else {
                i2c_update_status(I2C_READING_BYTES);
            }
        }

        if(i2c_byte_num < (I2C_BYTES)) {
            // Sending ack
            // Wait till SCL is low
            while(i2c_read_pin(SCL_PIN));

            PIN_OUT_SET = 1 << SDA_PIN;
            //PIN_OUT_CLEAR = 1 << SDA_PIN; 
            GPIO_OUTPUT_SET(SDA_PIN, 0);
        }


        i2c_buffer[i2c_byte_num] = i2c_temp; 
        i2c_temp = 0; 
        i2c_byte_num++; 
    } else {
        while(i2c_read_pin(SCL_PIN));
        //ets_delay_us(5); 
        //if(i2c_byte_num < I2C_BYTES) {
        PIN_DIR_INPUT = 1 << SDA_PIN; 
        gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));
        //}

        i2c_bit_number = 7; 
    }

    uint32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    ETS_GPIO_INTR_ENABLE(); // Enable gpio interrupts
}

void i2c_interupt_reading_start() {
    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts

    if(i2c_read_pin(SCL_PIN)) 
    {
        // // Okay start reading...
        // ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_address, SCL_PIN);             // GPIO2 interrupt handler
        // //gpio_output_set(0, 0, 0, GPIO_ID_PIN(SCL_PIN));                       // Set GPIO2 as input
        // gpio_pin_intr_state_set(SCL_PIN, 1);                       // Interrupt on positive edge

        // gpio_pin_intr_state_set(SDA_PIN, 0);

        i2c_update_status(I2C_READING_ADDRESS); 

    } else {
        //os_printf("Not a start condition\n");
    }

    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);


    ETS_GPIO_INTR_ENABLE(); // Enable gpio interrupts
}

void print_debug_info(void *arg) // in Arduino this is loop the main loop
{

    if(clockpulses != 0) {
        os_printf("DEBUG, count: %d, clockpulses: %d\n", i2c_bit_number, clockpulses);

        os_printf("BUFFER: %d, %d, %d \n", i2c_buffer[0], i2c_buffer[1], i2c_buffer[2]);
    }


    clockpulses = 0; 

    //temp fix
    PIN_DIR_INPUT = 1 << SDA_PIN; 
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));


    ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts

    i2c_update_status(I2C_READING_START);

    os_memset(&i2c_buffer[0], 0, sizeof(i2c_buffer));

    //i2c_slave_intr_init(); 

    // stime = system_get_time(); 
    // reading_address = system_get_time() - stime; 

    ETS_GPIO_INTR_ENABLE();
}

/**
*  trigger
*  0: disable
*  1: positive edge
*  2: negative edge
*  3: any egde
*/
void i2c_slave_intr_init(void) {
    ETS_GPIO_INTR_DISABLE();                                        // Disable gpio interrupts

    ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_start, SDA_PIN);      // GPIO2 interrupt handler
    //gpio_output_set(0, 0, 0, GPIO_ID_PIN(SDA_PIN));                 // Set GPIO2 as input
    gpio_pin_intr_state_set(GPIO_ID_PIN(SDA_PIN), 2);              // Interrupt on negative edge

    os_printf("GPIO intr setup\n");

    ETS_GPIO_INTR_ENABLE();                                         // Enable gpio interrupts
}

void ICACHE_FLASH_ATTR 
i2c_slave_init(void)
{

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);            // SET GPIO function, not uart...
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);            // SET GPIO function, not uart...

    // Setting I2C pins to input
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(0));
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(2));


    i2c_byte_num = 0; 

    user_i2c_debug(); 

    //i2c_slave_intr_init(); 
    i2c_update_status(I2C_READING_START); 

    // ETS_GPIO_INTR_ATTACH(i2c_interupt_reading_address, SCL_PIN);             // GPIO2 interrupt handler
    // gpio_pin_intr_state_set(SCL_PIN, 1);                       // Interrupt on positive edge
    // ETS_GPIO_INTR_ENABLE();    
}


void ICACHE_FLASH_ATTR 
user_i2c_debug(void)
{
    //Disarm timer
    os_timer_disarm(&timer1);

    //Setup timer
    os_timer_setfn(&timer1, (os_timer_func_t *)print_debug_info, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&timer1, 1000, 1);

}