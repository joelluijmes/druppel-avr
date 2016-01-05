#include <util/delay.h>
#include <avr/io.h>

#include "..\twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	//sei();
	usi_init_slave(0x08);
	while (1)
	{
		_delay_ms(100);
		usi_read_slave();
		PORTB ^= 0x02;
	}

	return 0;
}