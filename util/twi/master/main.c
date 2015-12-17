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

	twi_master_init();

	uint8_t something = 1;
	uint8_t slave = 0xAB;
	uint8_t status;

	while (1)
	{
		_delay_ms(1000);
		if (twi_mt_start(slave) != TWST_OK)
			continue;

		twi_write(something);
		twi_stop();

		_delay_ms(1000);
	}
}