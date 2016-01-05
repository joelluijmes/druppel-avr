#include "twi.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#include "avr/twi_mega.h"

	#define MASTER_INIT() (twi_master_init())
	#define MT_INIT(slaveaddr) (twi_mt_start(slaveaddr))
	#define MT_WRITE(data) (twi_write(data))
#elif defined (__AVR_ATtiny85__)
	#include "usi/twi_usi.h"

	#define MASTER_INIT() ()	
	#define MT_INIT(slaveaddr) (usi_init_master(slaveaddr, 0x01))
	#define MT_WRITE(data) (usi_write_master(data))
#else
	#error "Device not supported"
#endif

#define STATE_NONE 0
#define STATE_MASTER 1
#define STATE_SLAVE 2
static uint8_t state = STATE_NONE;

TWRESULT twi_send(uint8_t slaveaddr, uint8_t* buffer, uint8_t len)
{
	MASTER_INIT();
	MT_INIT(slaveaddr);

	for (uint8_t i = 0; i < len; ++i)
		MT_WRITE(buffer[i]);

	return TWST_OK;
}