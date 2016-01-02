#include <util/delay.h>
#include <avr/io.h>

#include "twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	//sei();
	usi_init_slave(0x08);
	usi_write(0xFF);
	PORTB &= ~0x02;

	while (1) ;
	return 0;
}