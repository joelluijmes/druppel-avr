#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi.h"

typedef enum state state;
enum state
{
	STATE_IDLE,
	STATE_MEASURING,
	STATE_COMPLETED
};

typedef enum command command;
enum command
{
	COMMAND_REQUEST_MEASUREMENT,
	COMMAND_OK
};


static state receive_request();

int main()
{
	static state state = STATE_IDLE;

	while (1)
	{ 
		switch (state)
		{
		case STATE_IDLE:
			state = receive_request();
			break;
		case STATE_MEASURING:
			break;
		case STATE_COMPLETED:
			break;
		}
	}
}

static state receive_request()
{
	
	return STATE_MEASURING;
}