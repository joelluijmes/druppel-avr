#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../twi/twi.h"

#define EEPROM_24LC256_CTRL_ID 10
#define EEPROM_24LC256_A0 0
#define EEPROM_24LC256_A1 0
#define EEPROM_24LC256_A2 0

// uint16_t eeprom_get_address();
// void eeprom_set_address(uint16_t address);
// uint8_t eeprom_read_address(uint16_t address);
void eeprom_read_page_address(uint16_t address, uint8_t* buf, uint8_t buflen);
//void eeprom_write_address(uint16_t address, uint8_t byte);
void eeprom_write_page_address(uint16_t address, uint8_t* buf, uint8_t buflen);