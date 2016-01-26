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
static measure_t _measure[I2C_ADDRLEN];

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

void idp_process(uint8_t* data, uint8_t data_len)
{
	uint8_t len = 0;

	for (uint8_t i = 0; i < I2C_ADDRLEN; ++i)
	{
		uint8_t l = 0;
		handle_device(_addresses[i], _measure[i], data + len, data_len - len, &l);

		len += l;
	}
}

static state handle_device(uint8_t slave_addr, measure_t measure, uint8_t* data, uint8_t datalen, uint8_t* len)
{
	state _state = STATE_IDLE;
	while (1)
	{
		switch (_state)
		{
		case STATE_FAILED:
		case STATE_COMPLETED:
			return _state;
		case STATE_IDLE:
			_state = receive_request(slave_addr);
			break;
		case STATE_MEASURING:
		{
			*len = measure(data, datalen);
			_state = send_ready(slave_addr);
			break;
		}
		case STATE_SENDING:
			_state = send_data(slave_addr, data, *len);
			break;
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
