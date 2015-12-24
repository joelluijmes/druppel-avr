#include <avr/io.h>
#include "bmp180.h"
#include "twi.h"

const uint8_t days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

static uint8_t bcd2dec(uint8_t num);
static uint16_t date2days(uint16_t years, uint8_t months, uint8_t days);
static uint32_t time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);
static uint16_t read_register(uint8_t address);
static int16_t read_signed_register(uint8_t address);
static CALIBRATION read_calibration_coefficients();

void read_unix_time()
{
	// DS1307 time = read_ds1307(); 
	// uint32_t unixtime = time2long(date2days(time.year, time.month, time.day), time.hour, time.minute, time.second);
	// return unixtime + SECONDS_FROM_1970_TO_2000;

	uint16_t value = read_register(0xD0); 


	printf("%d: %d\n", value >> 8, value);
}

void read_temperature() 
{
	CALIBRATION c = read_calibration_coefficients();

	printf("%d\n", c.AC1);
	printf("%d\n", c.AC2);
	printf("%d\n", c.AC3);
	printf("%u\n", c.AC4);
	printf("%u\n", c.AC5);
	printf("%u\n", c.AC6);

	printf("%d\n", c.MB);
	printf("%d\n", c.MC);
	printf("%d\n", c.MD);

	int16_t test = -32007; 

	test = c.AC5; 
	printf("(%d << 8) + %d\n", (uint8_t) (test >> 8), (uint8_t) test);

	test = c.AC6; 
	printf("(%d << 8) + %d\n", (uint8_t) (test >> 8), (uint8_t) test);

	uint16_t UT 	= read_register(0xF6); 

	printf("%u\n", UT );

	long X1 = (UT - c.AC6) * c.AC5 / (1 << 15);
	long X2 = c.MC * (1 << 11) / (X1 + c.MD);
	long B5 = X1 + X2; 
	long temp = (B5 + 8) / (1 << 4); 
	printf("%d\n", (uint8_t) temp);

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

static uint16_t read_register(uint8_t address) 
{
	if (twi_mt_start(BMP180_CTRL_ID) != TWST_OK)
		return 0;

	twi_write(address);								// Write register address

	if(twi_mr_start(BMP180_CTRL_ID) != TWST_OK)
		return 0;

	uint8_t msb  = twi_read();
	uint8_t lsb = twi_peek();
	twi_stop();

	return msb << 8 | lsb;
}

static int16_t read_signed_register(uint8_t address) 
{
	if (twi_mt_start(BMP180_CTRL_ID) != TWST_OK)
		return 0;

	twi_write(address);								// Write register address

	if(twi_mr_start(BMP180_CTRL_ID) != TWST_OK)
		return 0;

	uint8_t msb  = twi_read();
	uint8_t lsb = twi_peek();
	twi_stop();

	return (int16_t)(msb << 8 | lsb);
}

static CALIBRATION read_calibration_coefficients() 
{
	CALIBRATION data; 
	//Reading values
	data.AC1 	= read_signed_register(0xAA);
	data.AC2 	= read_signed_register(0xAC);
	data.AC3 	= read_signed_register(0xAE);
	data.AC4 	= read_register(0xB0);
	data.AC5 	= read_register(0xB2);
	data.AC6 	= read_register(0xB4);
	data.B1 	= read_signed_register(0xB6);
	data.B2 	= read_signed_register(0xB8);
	data.MB 	= read_signed_register(0xBA);
	data.MC 	= read_signed_register(0xBC);
	data.MD 	= read_signed_register(0xBE);

    return data;
}

