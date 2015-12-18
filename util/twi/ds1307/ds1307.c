#include <avr/io.h>
#include "ds1307.h"
#include "twi.h"

static uint8_t bcd2dec(uint8_t num);
static DS1307 read_ds1307();

uint32_t read_unix_time()
{

	return 0; 
}

static uint8_t bcd2dec(uint8_t num)
{
	// Convert Binary Coded Decimal (BCD) to Decimal
	return ((num/16 * 10) + (num % 16));
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