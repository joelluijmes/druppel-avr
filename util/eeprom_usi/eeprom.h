#pragma once

#include <avr/io.h>
#include "../twi/twi.h"

uint8_t eeprom_write(uint16_t address, uint8_t* buf, uint8_t buflen);
uint8_t eeprom_read(uint16_t address, uint8_t* buf, uint8_t buflen);