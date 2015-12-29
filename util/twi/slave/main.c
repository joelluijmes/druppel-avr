#include <avr/io.h>
#include <avr/interrupt.h>

#include "../twi_module.h"

int main()
{
	sei();
	DDRB = 0x20;	// ledje
	twi_slave_init(0x08);

	PORTB = 0x20;
	while (1)
	{
		uint8_t data = twi_read();

		PINB = 0x20;
	}
}