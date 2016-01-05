#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <util/twi.h>

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
	twi_master_init();

	puts("Welcome!");
	while (twi_mr_start(0x08) != TWST_OK)
	{
		printf("Failed: %x\n", TW_STATUS);
		twi_stop();
		puts("No ACK (Enter to continue)");
		getchar();
	}

	while (1)
	{
		char c = twi_read();
		printf("Status: %x\n", TW_STATUS);
		if (TW_STATUS != TW_MR_DATA_ACK)
			break;
		printf("%c\n", c);
	}

	puts("Disconnected");
	// printf("Enter char to send: ");
	// while (1)
	// {
	// 	char c = getchar();
	// 	twi_write(c);
	// 	PINB = 0x20;
	// }

	return 0;
}