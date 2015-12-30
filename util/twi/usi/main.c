#include <util/delay.h>
#include <avr/io.h>

#include "twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	if (usi_init_slave(0x08) == USI_SLAVE_RECEIVE)
		PORTB &= ~0x02;


	while (1) ;
	return 0;
}