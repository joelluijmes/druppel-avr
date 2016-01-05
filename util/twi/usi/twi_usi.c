#include "twi_usi.h"

#define SCL_HIGH() PORT_USI |= (1<<PIN_USI_SCL)
#define SCL_LOW() PORT_USI &= ~(1<<PIN_USI_SCL)
#define SDA_LOW() PORT_USI &= ~(1<<PIN_USI_SDA)
#define SDA_HIGH() PORT_USI |= (1<<PIN_USI_SDA)

#define SDA_OUTPUT() DDR_USI |= (1<<PIN_USI_SDA)
#define SDA_INPUT() DDR_USI &= ~(1<<PIN_USI_SDA)
#define SCL_OUTPUT() DDR_USI |= (1<<PIN_USI_SCL)
#define SCL_INPUT() DDR_USI &= ~(1<<PIN_USI_SCL)

#define IS_SCL_HIGH() (PIN_USI & (1<<PIN_USI_SCL))
#define IS_SDA_HIGH() (PIN_USI & (1<<PIN_USI_SDA))

#define SR_RESET (1 << USISIF | 1 << USIOIF | 1 << USIPF | 1 << USIDC)
#define SR_SHIFT8 (SR_RESET)
#define SR_SHIFT1 (SR_RESET | 0x0E << USICNT0)
#define SR_TICK (SR_RESET | 1 << USITC)

#define CR_RESET (1 << USIWM1 | 1 << USICS1 | 1 << USICLK)
#define CR_TICK (1 << USIWM1 | 1 << USICS1 | 1 << USICLK | 1 << USITC)

#define SYS_CLK   1000.0  // [kHz]

// Atmel stuff (AVR310)
#define T2_TWI    ((SYS_CLK *4700) /1000000) +1 // >4,7us
#define T4_TWI    ((SYS_CLK *4000) /1000000) +1 // >4,0us

#define SET_USI_TO_SEND_ACK()                                                                                 \
{                                                                                                             \
    USIDR    =  0;                                              /* Prepare ACK                         */ \
    SDA_OUTPUT();                                               /* Set SDA as output                   */ \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|  /* Clear all flags, except Start Cond  */ \
    (0x0E<<USICNT0);                                /* set USI counter to shift 1 bit. */ \
}

#define SET_USI_TO_READ_ACK()                                                                                 \
{                                                                                                             \
    SDA_INPUT();                                                /* Set SDA as intput */                   \
    USIDR    =  0;                                              /* Prepare ACK        */                      \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|  /* Clear all flags, except Start Cond  */ \
    (0x0E<<USICNT0);                                /* set USI counter to shift 1 bit. */ \
}

#define SET_USI_TO_TWI_START_CONDITION_MODE()                                                                                     \
{                                                                                                                                 \
    USICR    =  (1<<USISIE)|(0<<USIOIE)|                          /* Enable Start Condition Interrupt. Disable Overflow Interrupt.*/  \
    (1<<USIWM1)|(0<<USIWM0)|                          /* Set USI in Two-wire mode. No USI Counter overflow hold.      */  \
    (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|              /* Shift Register Clock Source = External, positive edge        */  \
    (0<<USITC);                                                                                                         \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|    /* Clear all flags, except Start Cond               */  \
    (0x0<<USICNT0);                                                                                                     \
}

#define SET_USI_TO_SEND_DATA()                                                                               \
{                                                                                                            \
    SDA_OUTPUT();                                               /* Set SDA as output                  */ \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|  /* Clear all flags, except Start Cond */ \
    (0x0<<USICNT0);                                 /* set USI to shift out 8 bits        */ \
}

#define SET_USI_TO_READ_DATA()                                                                               \
{                                                                                                            \
    SDA_INPUT();                                                /* Set SDA as input                   */ \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|  /* Clear all flags, except Start Cond */ \
    (0x0<<USICNT0);                                 /* set USI to shift out 8 bits        */ \
}

#define WAIT_TRANSFER()                                                                                \
{                                                                                                      \
    do                                                                                                 \
    {                                                                                                  \
        _delay_us(T2_TWI/4);                                                                           \
        USICR = CR_TICK;                        /* Rising SCL Edge */                                  \
        while (!IS_SCL_HIGH());                 /* wait scl high */                                    \
        _delay_us(T4_TWI/4);                                                                           \
        USICR = CR_TICK;                        /* Falling SCL Edge */                                 \
    } while ((USISR & (1 << USIOIF)) == 0);     /* Operation completes */                              \
    _delay_us(T2_TWI/4);                                                                               \
}

#define WAIT_TRANSFER_1BIT()                                                                           \
{                                                                                                      \
    USISR = SR_SHIFT1;                          /* Count one bit (2 edges) */                          \
    WAIT_TRANSFER();                                                                                   \
}                                                                                                      
                                                                                                       
#define WAIT_TRANSFER_8BIT()                                                                           \
{                                                                                                      \
    USISR = SR_SHIFT8;                          /* Count one bit (2 edges) */                          \
    WAIT_TRANSFER();                                                                                   \
}

#define SLAVE_WAIT() do { } while ((USISR & (1 << USIOIF)) == 0)

static uint8_t _address;
static void start_condition();

USIRESULT usi_wait()
{
    do
    {
        SET_USI_TO_TWI_START_CONDITION_MODE();              // Reset to receive start condition
        while ((USISR & (1 << USISIF)) == 0) ;              // Wait for USI to detect the start condition
        SDA_INPUT();                                        // Data as input -> to receive slave address

        while (IS_SCL_HIGH() && !(SDA_HIGH())) ;            // Be sure start condition was completed

        USICR = (IS_SDA_HIGH())                             // Check if we received start of stop condition
            ? 1 << USISIE | 1 << USIWM1 | 1 << USIWM0 | 1 << USICS1                 // stop condition
            : 1 << USISIE | 1 << USIOIE | 1 << USIWM1 | 1 << USIWM0 | 1 << USICS1;  // start condition

        USISR = SR_RESET;                                   // Reset flags

        SLAVE_WAIT();                                       // Wait for completion
    } while (USIDR != 0 && (USIDR >> 1) != _address);       // Repeat as long we are not addressed by the master

    USIRESULT result = (USIDR & 0x01)                       // Check if we are transmitting or receiving slave
        ? USI_SLAVE_TRANSMIT
        : USI_SLAVE_RECEIVE;

    SET_USI_TO_SEND_ACK();                                  // Send the acknowledge 
    return result;
}

USIRESULT usi_init_slave(uint8_t slave_addr)
{
    _address = slave_addr;
    // Not sure what initial values should be used but these seem to work best :D
    SCL_LOW();
    SDA_LOW();
    SCL_INPUT();
    SDA_INPUT();

    return usi_wait();
}

uint8_t usi_init_mt(uint8_t slave_addr)
{
    SCL_HIGH();
    SDA_HIGH();
    
    SCL_OUTPUT();
    SDA_OUTPUT();

    USIDR = 0xFF;
    USICR = CR_RESET;                                   // TWI Mode | Software Strobe Clock
    USISR = SR_RESET;                                   // Clear flags

    start_condition();
    if ((USISR & (1 << USISIF)) == 0)                   // Checks start condition :)
        return 0;

    return usi_write_master(slave_addr << 1);           // Sends slave address and returns ack
}

uint8_t usi_write_master(uint8_t data)
{
    // WRITE DATA
    SCL_LOW();                                          
    USIDR = data;                                       // Set data to send
    WAIT_TRANSFER_8BIT();                               // Wait transfer of 8 bits (data)
    
    // READ ACK
    SDA_INPUT();                                        // Receive input from slave
    WAIT_TRANSFER_1BIT();                               // Wait transfer of a bit (ACK bit)
    SDA_OUTPUT();                                       // Claim SDA 

    return USIDR;
}

uint8_t usi_write_slave(uint8_t data)
{
    SLAVE_WAIT();                                       // Wait for bus to be ready

    USIDR = data;                                       // Sets data
    SET_USI_TO_SEND_DATA();                             // Issues USI to send data

    SLAVE_WAIT();                                       // Wait again..
    SET_USI_TO_READ_ACK();                              // Issues USI to read the ack 

    SLAVE_WAIT();                                       // Waits once more..
    if (USIDR)                                          // Checks ACK 
    {
        SET_USI_TO_TWI_START_CONDITION_MODE();          // ACK Received -> restart?
        // goto: restart??
    }

    return !USIDR;                                      // Returns ACK received                                         
}

uint8_t usi_read_slave()
{
    SLAVE_WAIT();                                       // Wait to be come ready
    SET_USI_TO_READ_DATA();                             // Sets usi to start reading

    SLAVE_WAIT();                                       // Wait for read to complete
    uint8_t data = USIDR;                               // Temporary hold data 
    SET_USI_TO_SEND_ACK();                              // Send ACK

    return data;                                        // Returns the data
}

static void start_condition()
{
    // We set the clock manually high and wait for it. This due the fact if there was already some I2C traffic
    // we wouldn't be able to claim it. And so not corrup the bus

    SCL_HIGH();                                         // Set clock high
    while (!IS_SCL_HIGH()) ;                            // Wait for it
    _delay_us(T2_TWI/4);                                // Wait rising

    SDA_LOW();                                          // Data low
    _delay_us(T4_TWI/4);                                // Wait falling
    SCL_LOW();                                          // Clock low
    SDA_HIGH();                                         // Data high
}