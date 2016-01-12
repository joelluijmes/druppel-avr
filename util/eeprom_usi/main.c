#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "eeprom.h"

#define LEN 20

int main()
{
	uint8_t buf[LEN];
	for (uint8_t i = 0; i < LEN; ++i)
		buf[i] = i*i;

	volatile uint8_t s = eeprom_write(0x00, buf, LEN);

	_delay_ms(250);

	uint8_t buf2[LEN];
	volatile uint8_t b = eeprom_read(0x00, buf2, LEN);

	volatile uint8_t same = 1;
	for (uint8_t i = 0; i < LEN; ++i)
	{
		if (buf[i] != buf2[i])
		{
			same = 0;
			break;
		}
	}

	PINB = 0x00;
}
