#include <avr/io.h>
#include <avr/interrupt.h>

#include "../twi.h"

int main()
{
	sei();
	DDRB = 0x20;	// ledje


	twi_init();
	twi_start_slave(0xAB);

	while (1)
	{
		uint8_t data = twi_read();

		PINB = 0x20;
	}
}