#include <util/delay.h>
#include <avr/io.h>

#include "..\twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	TWRESULT result = usi_init_slave(0x08);
	while (result != TWST_SL_RECEIVING)
		;	// something gone wrong
	PORTB &= ~0x02;

	while (1)
	{
		volatile uint8_t data = usi_read_slave();
		PINB = 0x02;
	}

	return 0;
}