#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../twi/twi.h"

#define EEPROM_24LC256_CTRL_ID 10
#define EEPROM_24LC256_A0 0
#define EEPROM_24LC256_A1 0
#define EEPROM_24LC256_A2 0

uint8_t read_address(uint16_t address);
void read_page_address(uint16_t address, uint8_t* buf, uint8_t buflen);
void write_address(uint16_t address, uint8_t byte);
void write_page_address(uint16_t address, uint8_t* buf, uint8_t buflen);
uint8_t get_ctrl_id();
