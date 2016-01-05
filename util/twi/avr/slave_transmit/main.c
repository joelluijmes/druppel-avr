#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>

#include "../twi_mega.h"
#include "../../../uart/uart.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

int main()
{
	DDRB = 0x20;
	PORTC = 1 << 4 | 1 << 5;
	stdout = &mystdout;
	stdin = &mystdin;

	uart_init();
	puts("Welcome!");
	
	twi_slave_init(0x08);

	twi_write(0xAB);

	printf("Enter char to send: ");
	while (1)
	{
		char c = getchar();
		twi_write(c);
		printf("Status: %x\n", TW_STATUS);
		PINB = 0x20;
	}

	return 0;
}