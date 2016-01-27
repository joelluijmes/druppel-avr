#include <avr/io.h>
#include <avr/interrupt.h>

#include "idp_slave.h"
#include "../../util/sht15/sht.h"
#include "../../util/pin.h"

#define BUF_LEN 64
#define DATA_NUM 1

static sht* _sht;

static uint8_t measure_1(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0x11111111;
	return 4;
	if (len < sizeof(double))
	return 0;

	double temp = sht_readTemperature(_sht);
	*(double*)data = temp;

	return sizeof(double);
}

static uint8_t measure_2(uint8_t* data, uint8_t len)
{
*(uint32_t*)data = 0x22222222;
return 4;
	if (len < sizeof(double))
		return 0;

	double temp = sht_readTemperature(_sht);
	double humi = sht_compensateHumidity(_sht, temp);
	*(double*)data = humi;

	return sizeof(double);
}

static void init()
{
	pin sck =
	{
		.ddr = &DDRB,
		.out = &PORTB,
		.in = &PINB,
		.mask = 1 << 4
	};
	pin pinData =
	{
		.ddr = &DDRB,
		.out = &PORTB,
		.in = &PINB,
		.mask = 1 << 3
	};

	sht s =
	{
		.pinSCK = sck,
		.pinDATA = pinData
	};
	_sht = &s;
}

int main()
{
	init();

	uint8_t data[BUF_LEN];
	uint8_t addresses[] = { 1, 2 };
	measure_t measures[] = { &measure_1, &measure_2 };
	
	idp_init(addresses, measures);
	while (1)
	{
		idp_process(data, BUF_LEN);

		// TODO: Sleep
	}
}
