#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/ds1307_usi/ds1307.h"
#include "../../util/twi/twi.h"
#include "../../util/eeprom_usi/eeprom.h"

#include "sensors.h"

#define EEPROM_ADDR_LAST_ACCESSED 0
static uint16_t read_addr()
{
	uint8_t buf[2];
	eeprom_read(EEPROM_ADDR_LAST_ACCESSED, buf, 2);

	return buf[0] << 8 | buf[1];
}

int main()
{
	uint8_t data[64];
	uint16_t addr = read_addr();

	while (1)
	{
		uint32_t unixtime = read_unix_time();
		uint8_t len = sensor_fill(unixtime, data, 64);

		eeprom_write(addr, data, len);
		addr += len;
		// if (communcation_active())
		// 	communication_send(data, len);
	}
}