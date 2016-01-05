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
	puts("initts");
	if (twi_mr_start(slave) != TWST_OK)
	{
		PINB = 0x20;
		puts("failed");
		twi_stop();
		printf("%x\n", TW_STATUS);
	}

	PORTB = 0x20;
	_delay_ms(20);
	while(1)
	{	
		uint8_t data = twi_read();
		printf("Data: %d\tStatus: %x\n", data, TW_STATUS);

		PINB = 0x20;

		if (TW_STATUS != TW_MT_DATA_ACK)
			break;
	}

	return 0;
}