#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../twi.h"


int main()
{
	sei();
	DDRB = 0x20;
	PORTC = 1 << 4 | 1 << 5;
	PORTB = 0x20;

	twi_master_init();

	uint8_t slave = 0x08;

	_delay_ms(1000);
	if (twi_mr_start(slave) != TWST_OK)
		PORTB = 0x20;
	else 
		PORTB &= ~0x20;
	while(1);
	return 0;
}