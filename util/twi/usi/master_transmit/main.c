#include <util/delay.h>
#include <avr/io.h>

#include "..\twi_usi.h"

int main()
{
	DDRB = 0x02;
	PORTB |= 0x02;

	while (usi_init_master(0x08, 0xFF) != TWST_OK)
	{	// Didn't get ack -> repeat
		usi_stop();
	}
	
	PORTB &= ~0x02;

	while (1)
	{
		for (uint8_t i = 255; i > 0; --i)
		{
			usi_write_master(i);
			PINB = 0x02;
		}
	}

	return 0;
}