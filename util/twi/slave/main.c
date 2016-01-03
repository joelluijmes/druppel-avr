#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "../twi_module.h"
#include "../../uart/uart.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	stdout = &mystdout;
	uart_init();
	printf("init\n");

	sei();
	DDRB = 0x20;	// ledje
	PORTB = 0x00;
	twi_slave_init(0x08);
	printf("%x\n", TW_STATUS);

	PORTB = 0x20;
	while (1) ;
	{
		uint8_t data = twi_read();

		PINB = 0x20;
	}
}