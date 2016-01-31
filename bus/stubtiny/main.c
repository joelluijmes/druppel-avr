#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "idp_slave.h"
#include "..\..\util\wdt.h"
#include "..\..\util\led.h"

static uint8_t measure_1(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0x11111111;
	return 4;
}

static uint8_t measure_2(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0x22222222;
	return 4;
}

int main()
{
	_wdt_disable();
	uint8_t addresses[] = { 1, 2 };
	measure_t measures[] = { &measure_1, &measure_2 };
	
	idp_init(addresses, measures);

	_wdt_enable(WDTO_4S);
	while (1)
	{
		idp_process();

		// TODO: Sleep
	}
}
