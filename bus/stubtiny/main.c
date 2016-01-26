#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "idp_slave.h"

#define SLAVE_ADDR 0x11
#define BUF_LEN 64
#define DATA_NUM 1

static uint8_t measure(uint8_t* data, uint8_t len)
{
	*(uint32_t*)data = 0xDEADBEEF;
	return 4;
}

int main()
{
	uint8_t data[BUF_LEN];
	
	idp_init(SLAVE_ADDR, DATA_NUM, measure);
	while (1)
	{
		idp_process(data, BUF_LEN);

		// TODO: Sleep
	}
}
