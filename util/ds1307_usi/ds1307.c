#include "ds1307.h"

#define I2C_ADDR_DS1307 0x68
#define SECONDS_FROM_1970_TO_2000 946684800

const uint8_t days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

struct DS1307 {
	uint8_t second; 
	uint8_t minute; 
	uint8_t hour; 
	uint8_t day;
	uint8_t month; 
	uint8_t year;
};
typedef struct DS1307 DS1307;

static void read_ds1307(DS1307 *time);
static uint8_t bcd2dec(uint8_t num);
static uint16_t date2days(uint16_t years, uint8_t months, uint8_t days);
static uint32_t time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

uint32_t read_unix_time()
{
	DS1307 time = {};											// Initialize new time struct
	read_ds1307(&time); 										// Getting time data
	uint32_t unixtime = time2long(								// Getting seconds from days, hours, minutes and seconds
		date2days(time.year, time.month, time.day),  			// Getting count days from year, month and days
		time.hour, time.minute, time.second
	);
	return unixtime + SECONDS_FROM_1970_TO_2000;				// Return the seconds past after 2000 and the constant
}

static void read_ds1307(DS1307 *time) 
{
	uint8_t buffer[7] = {};
	if(twi_master_send_byte(I2C_ADDR_DS1307, 0x00, CLOSE) != TWST_OK)
		return; 

	if(twi_master_receive(I2C_ADDR_DS1307, buffer, sizeof(buffer), CLOSE) != TWST_OK)
		return; 

	//Reading values
	time->second = bcd2dec(buffer[0] & 0x7f); 					// Convert bcd to decimal and mask value to get seconds
	time->minute = bcd2dec(buffer[1]);
	time->hour 	 = bcd2dec(buffer[2] & 0x3f);					// Mask value
																// Buffer[3] is not needed, this is the week number 
	time->day 	 = bcd2dec(buffer[4]);
	time->month  = bcd2dec(buffer[5]);
	time->year 	 = bcd2dec(buffer[6]);
}

static uint8_t bcd2dec(uint8_t num)
{
	return ((num/16 * 10) + (num % 16));						// Convert Binary Coded Decimal (BCD) to Decimal
}

static uint16_t date2days(uint16_t years, uint8_t months, uint8_t days) {
	if (years >= 2000)											// Getting years from 2000 to now
		years -= 2000;
	uint16_t total_days = days;
	for (uint8_t i = 1; i < months; ++i)						// Getting the days past in the months
	    total_days += days_in_month[i -1];
	return total_days + 365 * years + (years + 3) / 4 - 1;		// Return total days, (years + 3) / 4 - 1 is for the leap years
}

static uint32_t time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;	// Return the total seconds from days, hours, minutes and seconds
}