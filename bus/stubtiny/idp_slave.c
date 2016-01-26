#include "idp_slave.h"

static uint8_t _slave_addr;
static uint8_t _num_data;
static measure_t _measure;

static state receive_request();
static state send_ready();
static state send_data(uint8_t* data, uint8_t datalen);

void idp_init(uint8_t slave_addr, uint8_t num_data, measure_t measure)
{
	_slave_addr = slave_addr;
	_num_data = num_data;
	_measure = measure;
}

state idp_process(uint8_t* data, uint8_t data_len)
{
	state state = STATE_IDLE;
	uint8_t len = 0;
	
	while (1)
	{ 
		switch (state)
		{
		case STATE_FAILED:	
		case STATE_COMPLETED:
			return state;					// exit on completion or fail
		case STATE_IDLE:
			state = receive_request();
			break;
		case STATE_MEASURING:
			len = _measure(data, data_len);
			state = send_ready();
			break;
		case STATE_SENDING:
			state = send_data(data, len);
			break;
		}
	}	
}

static state receive_request()
{
	uint8_t cmd;
	return (twi_slave_receive_byte(_slave_addr, &cmd) == TWST_OK && cmd == COMMAND_REQUEST_MEASUREMENT)
		? STATE_MEASURING
		: STATE_FAILED;
}

static state send_ready()
{
	// TODO: Add command that it failed?
	if (twi_slave_send_byte(_slave_addr, COMMAND_OK) != TWST_OK)
		return STATE_FAILED;

	uint8_t cmd;
	return (twi_slave_receive_byte(_slave_addr, &cmd) == TWST_OK && cmd == COMMAND_OK)
		? STATE_SENDING
		: STATE_FAILED;
}

static state send_data(uint8_t* data, uint8_t datalen)
{
	if (twi_slave_send_byte(_slave_addr, datalen) != TWST_OK)
		return STATE_FAILED;

	return (twi_slave_send(_slave_addr, data, &datalen) == TWST_OK)
		? STATE_COMPLETED
		: STATE_FAILED;
}
