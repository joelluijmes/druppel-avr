#include "idp_slave.h"

#define BUF_LEN 16

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
static measure_t _measure[I2C_ADDRLEN];
static state _states[I2C_ADDRLEN];

static state handle_device(uint8_t slave_addr, measure_t measure, uint8_t* data, uint8_t datalen, uint8_t* len);
static state receive_request(uint8_t slave_addr);
static state send_ready(uint8_t slave_addr);
static state send_data(uint8_t slave_addr, uint8_t* data, uint8_t datalen);

#if (I2C_ADDRLEN > 1)
	void idp_init(uint8_t* addresses, measure_t* measure)
	{
		for (uint8_t i = 0; i < I2C_ADDRLEN; ++i)
		{
			_addresses[i] = addresses[i];
			_measure[i] = measure[i];
		}
	}
#else
	void idp_init(uint8_t slave_addr, measure_t measure)
	{
		_addresses[0] = slave_addr;
		_measure[0] = measure;
	}
#endif

void idp_process()
{
	static uint8_t lens[I2C_ADDRLEN];
	static uint8_t buffers[I2C_ADDRLEN][BUF_LEN];								// not efficient :X
	for (uint8_t i = 0; i < I2C_ADDRLEN; ++i)
		_states[i] = handle_device(_addresses[i], _states[i], _measure[i], buffers[i], BUF_LEN, &lens[i]);
}

static state handle_device(uint8_t slave_addr, state state, measure_t measure, uint8_t* data, uint8_t datalen, uint8_t* len)
{
	switch (p_state)
	{
		case STATE_FAILED:
		case STATE_COMPLETED:
			return STATE_IDLE;													// ??
		case STATE_IDLE:
			return receive_request(slave_addr);
		case STATE_MEASURING:
		{
			*len = measure(data, datalen);
			return send_ready(slave_addr);
		}
		case STATE_SENDING:
			return send_data(slave_addr, data, *len);
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
