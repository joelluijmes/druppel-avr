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

	_delay_ms(1000);
	twi_mt_start(slave);
	printf("%x\n", TW_STATUS);

	while(1)
	{
		//_delay_ms(500);
		twi_write(0xFF);
		printf("%x\n", TW_STATUS);
	}

	return 0;
}