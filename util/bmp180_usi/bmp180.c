#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "bmp180.h"
#include "../twi/twi.h"

#define I2C_ADDR_BMP180 0x77
#define	BMP180_ADDRESS_TEMPERATURE 0x2E
#define	BMP180_ADDRESS_PRESSURE0 0x34
#define	BMP180_ADDRESS_PRESSURE1 0x74
#define	BMP180_ADDRESS_PRESSURE2 0xB4
#define	BMP180_ADDRESS_PRESSURE3 0xF4

struct CALIBRATION {
	uint8_t chipid; 	// 0x55
	int16_t AC1;		// 7448
	int16_t AC2;		// -1063
	int16_t AC3;		// -14398
	uint16_t AC4;		// 34345
	uint16_t AC5;		// 24634
	uint16_t AC6;		// 21015
	int16_t B1;			// 6515
	int16_t B2;			// 37
	int16_t MB;			// -32768
	int16_t MC;			// -11786
	int16_t MD;			// 3043
};
typedef struct CALIBRATION CALIBRATION;

static void start_measure(uint8_t address);
static int16_t read_register(uint8_t address);
static CALIBRATION read_calibration_coefficients();

double read_temperature() 
{
	/*
		X1 = (UT - AC6) * AC5 / (1 << 15)
		X2 = MC * (1 << 11) / (X1 + MD)
		T = (X1+X2+8)/(1 << 4)
	*/ 
	static CALIBRATION c;

	// Starting measurement
	start_measure(BMP180_ADDRESS_TEMPERATURE);

	if(c.chipid != 0x55) 								// Check if calibration data is read, if read chipid = 0x55
		c = read_calibration_coefficients();

	// Get temperature
	uint16_t UT = (uint16_t) read_register(0xF6); 					// Reading data from adc result register

	// Compute floating-point polynominals:
	double c5 = (pow(2,-15) / 160) * c.AC5;
	double c6 = c.AC6;
	double mc = (pow(2,11) / pow(160,2)) * c.MC;
	double md = c.MD / 160.0;

	// Calculate temperature
	double a = c5 * (UT - c6);
	double T = a + (mc / (a + md));

	//printf("Temperature: %d\n", (int16_t) T * 100);
	return T;
}

double read_presure() 
{
	/*
		X1 = (UT - AC6) * AC5 / (1 << 15)
		X2 = MC * (1 << 11) / (X1 + MD)
		T = (X1+X2+8)/(1 << 4)
	*/ 
	static CALIBRATION c;

	// Starting measurement
	start_measure(BMP180_ADDRESS_PRESSURE2); 

	if(c.chipid != 0x55) 								// Check if calibration data is read, if read chipid = 0x55
		c = read_calibration_coefficients();

	// Get pressure
	uint16_t UP = (uint16_t) read_register(0xF6); 					// Reading data from adc result register

	// Compute floating-point polynominals:
	double c3 = 160.0 * pow(2,-15) * c.AC3;
	double c4 = pow(10,-3) * pow(2,-15) * c.AC4;
	double b1 = pow(160,2) * pow(2,-30) * c.B1;
	double x0 = c.AC1;
	double x1 = 160.0 * pow(2,-13) * c.AC2;
	double x2 = pow(160,2) * pow(2,-25) * c.B2;
	double y0 = c4 * pow(2,15);
	double y1 = c4 * c3;
	double y2 = c4 * b1;
	double p0 = (3791.0 - 8.0) / 1600.0;
	double p1 = 1.0 - 7357.0 * pow(2,-20);
	double p2 = 3038.0 * 100.0 * pow(2,-36);

	// Calculate temperature
	double s = read_temperature() - 25.0;
	double x = (x2 * pow(s,2)) + (x1 * s) + x0;
	double y = (y2 * pow(s,2)) + (y1 * s) + y0;
	double z = (UP - x) / y;
	double P = (p2 * pow(z,2)) + (p1 * z) + p0;

	//printf("Pressure: %d mb\n", (int16_t) P);
	return P; 
}

static void start_measure(uint8_t address) 
{
	uint8_t buffer[2] = {0xF4, address}; 									// 0xF4 is the register address, second is control register value
	if(twi_master_send(I2C_ADDR_BMP180, buffer, 2, CLOSE) != TWST_OK)
		return; 

	_delay_ms(26); 															// Max conversion time of pressure3
}

static CALIBRATION read_calibration_coefficients() 
{
	CALIBRATION data; 
	//Reading values
	data.AC1 	= read_register(0xAA);
	data.AC2 	= read_register(0xAC);
	data.AC3 	= read_register(0xAE);
	data.AC4 	= (uint16_t) read_register(0xB0);
	data.AC5 	= (uint16_t) read_register(0xB2);
	data.AC6 	= (uint16_t) read_register(0xB4);
	data.B1 	= read_register(0xB6);
	data.B2 	= read_register(0xB8);
	data.MB 	= read_register(0xBA);
	data.MC 	= read_register(0xBC);
	data.MD 	= read_register(0xBE);
	data.chipid	= (uint16_t) read_register(0xD0) >> 8; 

    return data;
}

static int16_t read_register(uint8_t address) 
{
	if(twi_master_send_byte(I2C_ADDR_BMP180, address, CLOSE) != TWST_OK)
		return 0; 

	uint8_t buffer[2]; 
	if(twi_master_receive(I2C_ADDR_BMP180, buffer, 2, CLOSE) != TWST_OK)
		return 0; 

	return (int16_t)(buffer[0] << 8 | buffer[1]);
}