#pragma once

#define DS1307_CTRL_ID 0x68

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
