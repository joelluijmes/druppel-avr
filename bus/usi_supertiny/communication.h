#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../../util/twi/twi.h"
#include "../../util/eeprom/eeprom.h"

uint8_t communication_send_data(uint8_t* data, uint8_t len);
uint8_t communication_avaible();