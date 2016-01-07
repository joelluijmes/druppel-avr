#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../../util/twi/twi_module.h"
#include "../../util/ds1307/ds1307.h"
#include "../../util/eeprom/eeprom.h"

#define true 1
#define false 0
#define SENSORS_ADDRESS_FROM 0x00
#define SENSORS_ADDRESS_TO 0x10
#define SENSORS_ADDRESS_SIZE 0x10

#define SENSORS_RECEIVE_BUFFER_SIZE 0x10

enum state
{
	NOT_ATTEMPTED = 0,
    NOT_CONNECTED = 1,
    CONNECTED = 2,
    READING_DONE = 3,
};

typedef enum state state;

uint8_t sensors_check();
uint8_t write_sensor_eeprom(uint32_t unixtime, uint8_t sensor_id, uint8_t* buf, uint8_t buflen);
uint16_t sensors_get_eeprom_address();