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
    USISR = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| /* Clear all flags, except Start Cond  */ \
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

static volatile uint8_t _state, _address;

USIRESULT usi_init_slave(uint8_t slave_addr)
{
	SCL_HIGH();
	SDA_HIGH();
	SCL_OUTPUT();
	SDA_INPUT();

	USICR    =  (1<<USISIE)|(0<<USIOIE)|                            // Enable Start Condition Interrupt. Disable Overflow Interrupt.
              (1<<USIWM1)|(0<<USIWM0)|                            // Set USI in Two-wire mode. No USI Counter overflow prior
                                                                  // to first Start Condition (potentail failure)
              (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|                // Shift Register Clock Source = External, positive edge
              (0<<USITC);
  	USISR    = 0xF0;                                                // Clear all flags and reset overflow counter

  restart:
  	while ((USISR & (1 << USISIF)) == 0) ;

  	SDA_INPUT();

  	while (IS_SCL_HIGH() && !(SDA_HIGH())) ;

	USICR = (IS_SDA_HIGH())				
		? 1 << USISIE | 1 << USIWM1 | 1 << USIWM0 | 1 << USICS1		// stop
		: 1 << USISIE | 1 << USIOIE | 1 << USIWM1 | 1 << USIWM0 | 1 << USICS1;	// start

    USISR = SR_RESET;

    WAIT();

    USIRESULT result = USI_OK;
    if (USIDR == 0 || (USIDR >> 1) == slave_addr)
	{
		result = (USIDR & 0x01)
			? USI_SLAVE_TRANSMIT
			: USI_SLAVE_RECEIVE;

		SET_USI_TO_SEND_ACK();
	}
	else
	{
		SET_USI_TO_TWI_START_CONDITION_MODE();
		goto restart;
	}

	return result;
}

uint8_t usi_write(uint8_t data)
{
	WAIT();

	USIDR = data;
	SET_USI_TO_SEND_DATA();

	WAIT();
	SET_USI_TO_READ_ACK();

	WAIT();
	if (USIDR)			// nack: stop reading
	{
		SET_USI_TO_TWI_START_CONDITION_MODE();
		// goto: restart
	}

	return !!USIDR;
}
