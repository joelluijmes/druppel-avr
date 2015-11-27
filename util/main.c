#include <avr/io.h>

#include "pin.h"

void something(const pin* pin)
{
	volatile uint8_t value = pin_read(pin);
	value *= 2;
}

int main()
{
	pin pin = 
	{ 
		//.ddr = &DDRB,
		.port = &PORTB,
		.mask = 0x02
	};

	something(&pin);

	return 0;
}