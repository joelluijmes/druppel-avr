#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi_module.h"
#include "../../util/uart/uart.h"


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	PORTC = 1 << 4 | 1 << 5;

	uart_init();
	puts("Slave Receive!");
	
	twi_slave_init(0x01);
	//puts("Connected");

	while (1)
	{
		char c = twi_read();
		char x = twi_read();
		printf("Char: %c Status: %x\n", c, TW_STATUS);

		if (TW_STATUS != TW_SR_SLA_ACK)
			break;
		
		PINB = 0x20;
	}

	return 0;
}