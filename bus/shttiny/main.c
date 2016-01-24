#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi.h"
#include "../../util/sht15/sht.h"

#define SLAVE_ADDR 0x16

typedef enum state state;
enum state
{
	STATE_IDLE,
	STATE_MEASURING,
	STATE_SENDING,
	STATE_COMPLETED, 
	STATE_FAILED
};

typedef enum command command;
enum command
{
	COMMAND_REQUEST_MEASUREMENT,
	COMMAND_OK
};


static state receive_request();
static uint8_t measure(uint8_t* data, uint8_t datalen);
static state send_ready();
static state send_data(uint8_t* data, uint8_t datalen);

static sht* _sht;

int main()
{
	pin sck =
	{
		.ddr = &DDRB,
		.out = &PORTB,
		.in = &PINB,
		.mask = 1 << 4
	};
	pin pinData =
	{
		.ddr = &DDRB,
		.out = &PORTB,
		.in = &PINB,
		.mask = 1 << 3
	};

	sht sht =
	{
		.pinSCK = sck,
		.pinDATA = pinData
	};
	_sht = &sht;

	pin_output(&sck);
	pin_output(&pinData);

	volatile state state = STATE_IDLE;
	uint8_t data[16];
	uint8_t len = 0;

	while (1)
	{ 
		switch (state)
		{
		case STATE_IDLE:
			state = receive_request();
			break;
		case STATE_MEASURING:
			len = measure(data, 64);
			state = send_ready();
			break;
		case STATE_SENDING:
			state = send_data(data, len);
			break;
		case STATE_COMPLETED:
			state = STATE_IDLE;
			break;
		case STATE_FAILED:
			break;
		}
	}
}

static state receive_request()
{
	uint8_t cmd;
	return (twi_slave_receive_byte(SLAVE_ADDR, &cmd) == TWST_OK && cmd == COMMAND_REQUEST_MEASUREMENT)
		? STATE_MEASURING
		: STATE_FAILED;
}

static uint8_t measure(uint8_t* data, uint8_t datalen)
{
	double tmp = sht_readTemperature(_sht);
	*((double*)data) = tmp;
	return sizeof(double);
}

static state send_ready()
{
	// TODO: Add command that it failed?
	if (twi_slave_send_byte(SLAVE_ADDR, COMMAND_OK) != TWST_OK)
		return STATE_FAILED;

	uint8_t cmd;
	return (twi_slave_receive_byte(SLAVE_ADDR, &cmd) == TWST_OK && cmd == COMMAND_OK)
		? STATE_SENDING
		: STATE_FAILED;
}

static state send_data(uint8_t* data, uint8_t datalen)
{
	if (twi_slave_send_byte(SLAVE_ADDR, datalen) != TWST_OK)
		return STATE_FAILED;

	return (twi_slave_send(SLAVE_ADDR, data, &datalen) == TWST_OK)
		? STATE_COMPLETED
		: STATE_FAILED;
}