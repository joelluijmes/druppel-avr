#pragma once

#include <avr/io.h>
#include "twi.h"

uint8_t eeprom_write(uint16_t address, uint8_t* buf, uint8_t buflen);
uint8_t eeprom_read(uint16_t address, uint8_t* buf, uint8_t buflen);

static inline uint8_t eeprom_write_byte(uint16_t address, uint8_t data)
{
	uint8_t buf[] = { data };
	return eeprom_write(address, buf, 1);
}

static inline uint8_t eeprom_read_byte(uint16_t address)
{
	uint8_t buf[1];
	eeprom_read(address, buf, 1);

	return buf[0];
}
