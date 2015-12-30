#include "twi_usi.h"

#define SCL_HIGH() PORT_USI |= (1<<PIN_USI_SCL)
#define SCL_LOW() PORT_USI &= ~(1<<PIN_USI_SCL)
#define SDA_LOW() PORT_USI &= ~(1<<PIN_USI_SDA)
#define SDA_HIGH() PORT_USI |= (1<<PIN_USI_SDA)

#define SDA_OUTPUT() DDR_USI |= (1<<PIN_USI_SDA)
#define SDA_INPUT() DDR_USI &= ~(1<<PIN_USI_SDA)
#define SCL_OUTPUT() DDR_USI |= (1<<PIN_USI_SCL)
#define SCL_INPUT() DDR_USI &= ~(1<<PIN_USI_SCL)

#define SR_RESET (1 << USISIF | 1 << USIOIF | 1 << USIPF | 1 << USIDC)
#define SR_SHIFT1 (SR_RESET | 0x0E << USICNT0)
#define SR_TICK (SR_RESET | 1 << USITC)
#define CR_TICK (1 << USIWM1 | 1 << USICS1 | 1 << USICLK | 1 << USITC)

#define IS_SCL_HIGH() (PIN_USI & (1<<PIN_USI_SCL))
#define IS_SDA_HIGH() (PIN_USI & (1<<PIN_USI_SDA))

#define STATE_CHECK_ADDRESS                (0x00)
#define STATE_SEND_DATA                    (0x01)
#define STATE_REQUEST_REPLY_FROM_SEND_DATA (0x02)
#define STATE_CHECK_REPLY_FROM_SEND_DATA   (0x03)
#define STATE_REQUEST_DATA                 (0x04)
#define STATE_GET_DATA_AND_SEND_ACK        (0x05)

#define WAIT() do { } while ((USISR & (1 << USIOIF)) == 0)

#define SET_USI_TO_SEND_ACK() {  \
    USIDR = 0; /* Prepare ACK */ \
	SDA_OUTPUT(); /* Set SDA as output*/ \
    USISR = (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond  */ \
      (0x0E<<USICNT0); /* set USI counter to shift 1 bit. */ \
    }

#define SET_USI_TO_READ_ACK() { \
    SDA_INPUT(); /* Set SDA as input */  \
    USIDR = 0; /* Prepare ACK */ \
    USISR = (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond */ \
      (0x0E<<USICNT0); /* set USI counter to shift 1 bit. */ \
    }

#define SET_USI_TO_TWI_START_CONDITION_MODE() { \
    USICR = (1<<USISIE)|(0<<USIOIE)|  /* Enable Start Condition Interrupt. Disable Overflow Interrupt.*/  \
      (1<<USIWM1)|(0<<USIWM0)| /* Set USI in Two-wire mode. No USI Counter overflow hold. */  \
      (1<<USICS1)|(0<<USICS0)|(0<<USICLK)| /* Shift Register Clock Source = External, positive edge */   \
      (0<<USITC); \
    USISR = (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond */  \
      (0x0<<USICNT0); \
}

#define SET_USI_TO_SEND_DATA() { \
    SDA_OUTPUT(); /* Set SDA as output */ \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond */ \
      (0x0<<USICNT0); /* set USI to shift out 8 bits */ \
    }

#define SET_USI_TO_READ_DATA() { \
    SDA_INPUT(); /* Set SDA as input*/ \
    USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond */ \
      (0x0<<USICNT0); /* set USI to shift out 8 bits */ \
    }

void usi_init_slave(uint8_t slave_addr)
{
	SCL_LOW();
	SDA_LOW();
	SCL_INPUT();
	SDA_INPUT();

	do 
	{
		SET_USI_TO_TWI_START_CONDITION_MODE();

		while ((USISR & (1 << USISIF)) == 0);			// wait start condition

		SDA_INPUT();
		while (IS_SCL_HIGH() && !(SDA_HIGH())) ;

		USICR = (IS_SDA_HIGH())				// stop condition
			? 1 << USISIE | 1 << USIWM1 | 1 << USICS1
			: 1 << USISIE | 1 << USIOIE | 1 << USIWM1 | 1 << USIWM0 | 1 << USICS1;

		USISR = SR_RESET;
		
		// After the start condition is detected the slave address is sent
		// we check if it matches ours (or global.) If not we reset the state
		WAIT();										
	} 	// not our slave address
	while (USIDR != 0 && (USIDR >> 1) != slave_addr);

	//	if (USIDR & 0x01) transmit mode

	// Send the acknowledgment
	SET_USI_TO_SEND_ACK();
}

uint8_t usi_write(uint8_t data)
{
	USIDR = data;

	SDA_OUTPUT();
	SDA_HIGH();
	USISR = SR_RESET;

	WAIT();
	SDA_INPUT();
	USISR = SR_SHIFT1;

	WAIT();
	return !!USIDR;			// return NACK?
}