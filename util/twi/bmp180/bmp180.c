#include <avr/io.h>
#include "bmp180.h"
#include "twi.h"

static uint16_t read_register(uint8_t address);
static int16_t read_signed_register(uint8_t address);
static const CALIBRATION read_calibration_coefficients();
static void print_calibration();

void read_temperature() 
{
	/*
		X1 = (UT - AC6) * AC5 / (1 << 15)
		X2 = MC * (1 << 11) / (X1 + MD)
		T = (X1+X2+8)/(1 << 4)
	*/ 
	static CALIBRATION c;

	// Check if device id is set
	if(c.chipid != 0x55) { 
		c = read_calibration_coefficients();
	}

	// Get temperature
	uint16_t UT 	= read_register(0xF6); 

	// Compute floating-point polynominals:
	double c5 = (pow(2,-15) / 160) * c.AC5;
	double c6 = c.AC6;
	double mc = (pow(2,11) / pow(160,2)) * c.MC;
	double md = c.MD / 160.0;

	// Calculate temperature
	double a = c5 * (UT - c6);
	double T = a + (mc / (a + md));
	T = T * 100; 

	printf("%d\n", (int16_t) T);
}

static const CALIBRATION read_calibration_coefficients() 
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
	data.chipid	= read_register(0xD0) >> 8; 

	printf("oke\n");
    return data;
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

static void print_calibration() 
{
	CALIBRATION c = read_calibration_coefficients();

	printf("AC1: %d\n", c.AC1);
	printf("AC2: %d\n", c.AC2);
	printf("AC3: %d\n", c.AC3);
	printf("AC4: %u\n", c.AC4);
	printf("AC5: %u\n", c.AC5);
	printf("AC6: %u\n", c.AC6);
	printf("MB:  %d\n", c.MB);
	printf("MC:  %d\n", c.MC);
	printf("MD:  %d\n", c.MD);
}