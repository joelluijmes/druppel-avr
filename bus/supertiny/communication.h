#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../../util/twi/twi_module.h"
#include "../../util/eeprom/eeprom.h"

#define COMMUNICATION_ADDRESS_FROM 0x10
#define COMMUNICATION_ADDRESS_TO 0x10
#define COMMUNICATION_ADDRESS_SIZE 0x10


// enum state
// {
// 	NOT_ATTEMPTED = 0,
//     NOT_CONNECTED = 1,
//     CONNECTED = 2,
//     READING_DONE = 3,
// };

// typedef enum state state;

uint8_t communication_start(uint8_t slave_address);