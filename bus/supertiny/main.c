#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/ds1307/ds1307.h"
#include "../../util/twi/twi.h"

#include "sensors.h"
#include "communication.h"

#define ADDR_LAST_ACCESSED 0

static uint16_t read_addr()
{
	uint8_t buf[2];
	eeprom_read(ADDR_LAST_ACCESSED, buf, 2);

	return buf[0] << 8 | buf[1];
}

int main()
{
	uint8_t data[64];
	addr = read_addr();

	while (1)
	{
		uint8_t len = sensor_fill(data, 64);

		eeprom_write(addr, data, len);
		addr += len;

		if (communcation_active())
			communication_send(data, 64);
	}
}