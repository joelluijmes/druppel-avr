#include "idp_slave.h"

typedef enum state state;
enum state
{
	STATE_IDLE,
	STATE_MEASURING,
	STATE_SENDING,
	STATE_COMPLETED,
	STATE_FAILED
};

static uint8_t _addresses[I2C_ADDRLEN];
static measure_t _measure;

static state receive_request(uint8_t slave_addr);
static state send_ready(uint8_t slave_addr);
static state send_data(uint8_t slave_addr, uint8_t* data, uint8_t datalen);

#if I2C_ADDRLEN > 1
void idp_init(uint8_t* addresses, measure_t measure)
{
	memcpy(_addresses, addresses, I2C_ADDRLEN);
#else
void idp_init(uint8_t slave_addr, measure_t measure)
{
	_addresses[0] = slave_addr;
#endif
	
	_measure = measure;
}

void idp_process(uint8_t* data, uint8_t data_len)
{
	volatile state states[I2C_ADDRLEN] = {0};
	uint8_t len = 0;
	
	next_address:
	for (volatile uint8_t i = 0; i < I2C_ADDRLEN; ++i)
	{
		while (1)
		{
			volatile uint8_t slave_addr = _addresses[i];
			volatile state* p_state = states + i;

			switch (*p_state)
			{
			case STATE_FAILED:	
			case STATE_COMPLETED:
				goto next_address;		
			case STATE_IDLE:
				*p_state = receive_request(slave_addr);
				break;
			case STATE_MEASURING:
				len = _measure(data, data_len);
				*p_state = send_ready(slave_addr);
				break;
			case STATE_SENDING:
				*p_state = send_data(slave_addr, data, len);
				break;
			}
		}
	}
}

static state receive_request(uint8_t slave_addr)
{
	uint8_t cmd;
	return (twi_slave_receive_byte(slave_addr, &cmd) == TWST_OK && cmd == COMMAND_REQUEST_MEASUREMENT)
		? STATE_MEASURING
		: STATE_FAILED;
}

static state send_ready(uint8_t slave_addr)
{
	// TODO: Add command that it failed?
	if (twi_slave_send_byte(slave_addr, COMMAND_OK) != TWST_OK)
		return STATE_FAILED;

	uint8_t cmd;
	return (twi_slave_receive_byte(slave_addr, &cmd) == TWST_OK && cmd == COMMAND_OK)
		? STATE_SENDING
		: STATE_FAILED;
}

static state send_data(uint8_t slave_addr, uint8_t* data, uint8_t datalen)
{
	if (twi_slave_send_byte(slave_addr, datalen) != TWST_OK)
		return STATE_FAILED;

	return (twi_slave_send(slave_addr, data, &datalen) == TWST_OK)
		? STATE_COMPLETED
		: STATE_FAILED;
}
