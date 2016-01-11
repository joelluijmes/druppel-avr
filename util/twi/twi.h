#pragma once
#include <inttypes.h>

#define CLOSE 0
#define KEEP_ALIVE 1

// Status codes of what this module will return
typedef uint8_t TWRESULT;
#define TWST_OK	1
#define TWST_START_FAILED 2

#define TWST_MASTER_NACK 3
#define TWST_MASTER_RECEIVE_FAILED 7

#define TWST_SL_TRANSMITTING 4
#define TWST_SL_RECEIVING 5

#define TWST_STOP_FAILED 6

TWRESULT twi_master_send(uint8_t slaveaddr, uint8_t* buffer, uint8_t len, uint8_t keepAlive); 
TWRESULT twi_master_receive(uint8_t slaveaddr, uint8_t* buffer, uint8_t len, uint8_t keepAlive);
void twi_close();

static inline TWRESULT twi_master_send_byte(uint8_t slaveaddr, uint8_t data, uint8_t keepAlive)
{
	uint8_t buf[] = { data };
	return twi_master_send(slaveaddr, buf, 1, keepAlive);
}

static inline TWRESULT twi_master_receive_byte(uint8_t slaveaddr, uint8_t* data, uint8_t keepAlive)
{
	uint8_t buf[1];
	if (twi_master_receive(slaveaddr, buf, 1, keepAlive) != TWST_OK)
		return TWST_MASTER_RECEIVE_FAILED;

	*data = buf[0];
	return TWST_OK;
}