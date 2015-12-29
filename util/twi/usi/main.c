#include <util/delay.h>
#include <avr/io.h>

#include "twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	usi_init_master();
	
	usi_init_mt(0x08);
	
	
	while (1)
	{
		usi_write(0xFF);
	}

	return 0;
}