#include "twi.h"

#define TRANSMIT 0x01
#define RECEIVE 0x00
#define NACK 0x01
#define ACK 0x00

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#include "avr/twi_mega.h"

	#define MASTER_INIT() (twi_master_init())
	#define MASTER_START(slaveaddr) (twi_mt_start(slaveaddr))
	#define MR_INIT(slaveaddr) (twi_mr_start(slaveaddr))
	#define MASTER_WRITE(data) (twi_write(data))

#elif defined (__AVR_ATtiny85__)
	#include "usi/twi_usi.h"

	#define MASTER_INIT() ()	
	#define MT_START(slaveaddr) (usi_start_master(slaveaddr, TRANSMIT))
	#define MR_START(slaveaddr) (usi_start_master(slaveaddr, RECEIVE))
	#define MASTER_WRITE(data) (usi_write_master(data))
	#define MASTER_READ(nack) (usi_read_master(nack))
	#define MASTER_STOP() (usi_stop())
#else
	#error "Device not supported"
#endif

static uint8_t _state = 0;
#define CLOSED (1 << 0)

TWRESULT twi_master_send(uint8_t slaveaddr, uint8_t* buffer, uint8_t len, uint8_t keepAlive)
{
	if (_state & CLOSED)
	{
		if(MT_START(slaveaddr) != TWST_OK)
		{
			twi_close();
			return TWST_START_FAILED; 
		}
		
		_state &= ~CLOSED;
	}

	for (uint8_t i = 0; i < len; ++i)
		MASTER_WRITE(buffer[i]);

	if (!keepAlive)
		twi_close();

	return TWST_OK;
}

TWRESULT twi_master_receive(uint8_t slaveaddr, uint8_t* buffer, uint8_t len, uint8_t keepAlive)
{
	if (_state & CLOSED)
	{
		if(MR_START(slaveaddr) != TWST_OK)
		{
			twi_close();
			return TWST_START_FAILED;
		}

		_state &= ~CLOSED;
	}

	for (uint8_t i = 0; i < len - 1; ++i)
		buffer[i] = MASTER_READ(ACK);

	buffer[len - 1] = MASTER_READ(NACK); 

	if (!keepAlive)
		twi_close();

	return TWST_OK;
}

void twi_close()
{
	MASTER_STOP();
	_state |= CLOSED;
}
