#include "twi.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#include "avr/twi_mega.h"

	#define MASTER_INIT() (twi_master_init())
	#define MASTER_START(slaveaddr) (twi_mt_start(slaveaddr))
	#define MR_INIT(slaveaddr) (twi_mr_start(slaveaddr))
	#define MASTER_WRITE(data) (twi_write(data))

#elif defined (__AVR_ATtiny85__)
	#include "usi/twi_usi.h"

	#define MASTER_INIT() ()	
	#define MT_START(slaveaddr) (usi_start_master(slaveaddr, 0x01))
	#define MR_START(slaveaddr) (usi_start_master(slaveaddr, 0x00))
	#define MASTER_WRITE(data) (usi_write_master(data))
	#define MASTER_READ(nack) (usi_read_master(nack))
	#define MASTER_STOP() (usi_stop())
#else
	#error "Device not supported"
#endif

#define STATE_NONE 0
#define STATE_MASTER 1
#define STATE_SLAVE 2
static uint8_t state = STATE_NONE;

TWRESULT twi_master_send(uint8_t slaveaddr, uint8_t* buffer, uint8_t len)
{
	if(MT_START(slaveaddr) == TWST_OK)
		return TWST_START_FAILED; 

	for (uint8_t i = 0; i < len; ++i)
		MASTER_WRITE(buffer[i]);

	MASTER_STOP();

	return TWST_OK;
}

TWRESULT twi_master_read(uint8_t slaveaddr, uint8_t* buffer, uint8_t len)
{
	if(MR_START(slaveaddr) == TWST_OK)
		return TWST_START_FAILED;

	for (uint8_t i = 0; i < (len -1); ++i)
		buffer[i] = MASTER_READ(0);

	buffer[len -1] = MASTER_READ(1); 

	MASTER_STOP();

	return TWST_OK;
}