#include <avr/io.h>
#include <avr/interrupt.h>

#include "idp_slave.h"

#define BUF_LEN 64
#define DATA_NUM 1

static uint8_t measure_1(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0x01;
	return 4;
}

static uint8_t measure_2(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0x02;
	return 4;
}

int main()
{
	uint8_t data[BUF_LEN];
	uint8_t addresses[] = { 1, 2 };
	volatile measure_t measures[] = { &measure_1, &measure_2 };
	
	idp_init(addresses, measures);
	while (1)
	{
		idp_process(data, BUF_LEN);
		asm volatile("nop");
		// TODO: Sleep
	}
}
