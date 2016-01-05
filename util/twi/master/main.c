#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <util/twi.h>
#include "../twi.h"
#include "../../uart/uart.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	stdout = &mystdout;
	uart_init();
	printf("init");

	sei();
	DDRB = 0x20;
	//PORTC = 1 << 4 | 1 << 5;
	PORTB = 0;
	twi_master_init();
	uint8_t slave = 0x08;

	_delay_ms(10);
	while (twi_mt_start(slave) != TWST_OK)
	{
		PINB = 0x20;
		twi_stop();
		_delay_ms(500);
	}

	printf("%x\n", TW_STATUS);

	_delay_ms(2);
	while(1)
	{
		twi_write(0x00);
		
		if (TW_STATUS != TW_MT_DATA_ACK)
			break;
	}

	return 0;
}