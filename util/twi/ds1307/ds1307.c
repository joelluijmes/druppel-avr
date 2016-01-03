#include <avr/io.h>
#include "ds1307.h"
#include "twi.h"

const uint8_t days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

static DS1307 read_ds1307();
static uint8_t bcd2dec(uint8_t num);
static uint16_t date2days(uint16_t years, uint8_t months, uint8_t days);
static uint32_t time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

uint32_t read_unix_time()
{
	DS1307 time = read_ds1307(); 
	uint32_t unixtime = time2long(date2days(time.year, time.month, time.day), time.hour, time.minute, time.second);
	return unixtime + SECONDS_FROM_1970_TO_2000;
}

static DS1307 read_ds1307() 
{
	if (twi_mt_start(DS1307_CTRL_ID) != TWST_OK)
		return;

	twi_write(0x00); 						// Set pointer address to seconds

	if (twi_mr_start(DS1307_CTRL_ID) != TWST_OK)
		return;

	DS1307 time; 
	//Reading values
	time.second = bcd2dec(twi_read() & 0x7f); 
	time.minute = bcd2dec(twi_read());
	time.hour 	= bcd2dec(twi_read() & 0x3f);

	twi_read(); 							// week number not needed

	time.day 	= bcd2dec(twi_read());
	time.month 	= bcd2dec(twi_read());
	time.year 	= bcd2dec(twi_read());

    return time;
}

static uint8_t bcd2dec(uint8_t num)
{
	// Convert Binary Coded Decimal (BCD) to Decimal
	return ((num/16 * 10) + (num % 16));
}

static uint16_t date2days(uint16_t years, uint8_t months, uint8_t days) {
	if (years >= 2000)
		years -= 2000;
	uint16_t total_days = days;
	uint8_t i; 
	for (i = 1; i < months; ++i)
	    total_days += days_in_month[i -1];
	return total_days + 365 * years + (years + 3) / 4 - 1;
}

static uint32_t time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}