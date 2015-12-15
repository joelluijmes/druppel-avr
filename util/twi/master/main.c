#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../twi.h"
#include "../../uart/uart.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	uart_init();
 	stdout = &mystdout;
	sei();
	DDRB = 0x20;
	PORTC = 1 << 4 | 1 << 5;
	PORTB = 0x20;

	twi_init();

	uint8_t something = 1;
	uint8_t slave = 0xAB;
	uint8_t status;

	while (1)
	{
		do 
		{
			status = twi_start() & 0xF8;
			printf("start: %x\n", status);
		 	PORTB = 0x20; 	// error
		} while (status != TW_START && status != TW_REP_START);

		twi_mt_start(slave);
		printf("mt_start: %x\n", twi_status());
		twi_write(something);
		printf("write: %x\n", twi_status());

		twi_stop();
		printf("stop: %x\n", twi_status());


		_delay_ms(1000);
	}
}