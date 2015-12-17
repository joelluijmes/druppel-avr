#include <util/delay.h>
#include <avr/io.h>

#define DDR_USI DDRB
	#define PORT_USI PORTB
	#define PIN_USI PINB
	#define MASK_SDA 1 << PB0
	#define MASK_SCK 1 << PB2

int main()
{
	USISR = 1 << USISIF | 1 << USIOIF | 1 << USIOIF;
	USICR = 1 << USIWM1 | 1 << USICLK;

	DDRB = 0xFF;

	while (1)
	{
		USICR |= 1 << USITC;

		_delay_ms(2);
	}

	return 0;
}