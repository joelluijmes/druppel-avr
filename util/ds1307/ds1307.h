#pragma once

#include <avr/io.h>
#include "../twi/twi.h"

#define DS1307_CTRL_ID 0x68
#define SECONDS_FROM_1970_TO_2000 946684800

struct DS1307 {
	uint8_t second; 
	uint8_t minute; 
	uint8_t hour; 
	uint8_t day;
	uint8_t month; 
	uint8_t year;
	uint32_t ticks; 
};
typedef struct DS1307 DS1307;

uint32_t read_unix_time();
