#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "bmp180.h"
#include "../twi/twi.h"

#define I2C_ADDR_BMP180 0x77
#define	BMP180_ADDRESS_TEMPERATURE 0x2E				// Calculate temperature (single sample)
#define	BMP180_ADDRESS_PRESSURE0 0x34				// Calculate pressure (single sample)
#define	BMP180_ADDRESS_PRESSURE1 0x74				// Calculate pressure (2 samples)
#define	BMP180_ADDRESS_PRESSURE2 0xB4				// Calculate pressure (4 samples)
#define	BMP180_ADDRESS_PRESSURE3 0xF4				// Calculate pressure (8 samples)

struct CALIBRATION {
	uint8_t chipid; 								// Static chipid of bmp180: 0x55
	int16_t AC1;									// Device specific data. Example: 7448
	int16_t AC2;									// Device specific data. Example: -1063
	int16_t AC3;									// Device specific data. Example: -14398
	uint16_t AC4;									// Device specific data. Example: 34345
	uint16_t AC5;									// Device specific data. Example: 24634
	uint16_t AC6;									// Device specific data. Example: 21015
	int16_t B1;										// Device specific data. Example: 6515
	int16_t B2;										// Device specific data. Example: 37
	int16_t MB;										// Device specific data. Example: -32768
	int16_t MC;										// Device specific data. Example: -11786
	int16_t MD;										// Device specific data. Example: 3043
};
typedef struct CALIBRATION CALIBRATION;

static void start_measure(uint8_t address);
static int16_t read_register(uint8_t address);
static CALIBRATION read_calibration_coefficients();

static CALIBRATION c;												// Struct to hold the calibration data

double read_temperature() 
{
	start_measure(BMP180_ADDRESS_TEMPERATURE);						// Starting measurement

	if(c.chipid != 0x55) 											// Check if calibration data is read, if read chipid = 0x55
		c = read_calibration_coefficients();

	uint16_t UT = (uint16_t) read_register(0xF6); 					// Reading temperture data from adc result register

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
	start_measure(BMP180_ADDRESS_PRESSURE2); 						// Starting measurement

	if(c.chipid != 0x55) 											// Check if calibration data is read, if read chipid = 0x55
		c = read_calibration_coefficients();

	uint16_t UP = (uint16_t) read_register(0xF6); 					// Reading pressure data from adc result register

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

	// Calculate pressure
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
	uint8_t buffer[2] = {0xF4, address}; 									// 0xF4 is the control measurement register address, second is control register value
	if(twi_master_send(I2C_ADDR_BMP180, buffer, 2, CLOSE) != TWST_OK)
		return; 

	uint8_t max_conversion_time;
	switch(address)
	{
		case BMP180_ADDRESS_TEMPERATURE: 
			max_conversion_time = 5; 				// Max conversion time of temperature is 4.5
			break; 
		case BMP180_ADDRESS_PRESSURE0:
			max_conversion_time = 5;				// Max conversion time of pressure0 is 4.5
			break; 
		case BMP180_ADDRESS_PRESSURE1:
			max_conversion_time = 8;				// Max conversion time of pressure0 is 7.5
			break;
		case BMP180_ADDRESS_PRESSURE2:
			max_conversion_time = 14; 				// Max conversion time of pressure0 is 13.5
			break; 
		case BMP180_ADDRESS_PRESSURE3:
			max_conversion_time = 26; 				// Max conversion time of pressure0 is 25.5
			break;
		default:
			break;
	}
	_delay_ms(max_conversion_time); 				// Waiting the maximum conversion time														// Max conversion time of pressure3
}

static CALIBRATION read_calibration_coefficients() 
{
	CALIBRATION data;  
	//Reading the specific calibration data, this is per bmp180 different
	data.AC1 	= read_register(0xAA);
	data.AC2 	= read_register(0xAC);
	data.AC3 	= read_register(0xAE);
	data.AC4 	= (uint16_t) read_register(0xB0);				// Unsigned register value
	data.AC5 	= (uint16_t) read_register(0xB2);				// Unsigned register value
	data.AC6 	= (uint16_t) read_register(0xB4);				// Unsigned register value
	data.B1 	= read_register(0xB6);
	data.B2 	= read_register(0xB8);
	data.MB 	= read_register(0xBA);
	data.MC 	= read_register(0xBC);
	data.MD 	= read_register(0xBE);
	data.chipid	= (uint16_t) read_register(0xD0) >> 8; 			// Getting the chipid, this must be 0x55. Good check if communication is working

    return data;
}

static int16_t read_register(uint8_t address)
{
	if(twi_master_send_byte(I2C_ADDR_BMP180, address, CLOSE) != TWST_OK) 		// Setting register pointer to specific address
		return 0; 

	uint8_t buffer[2]; 
	if(twi_master_receive(I2C_ADDR_BMP180, buffer, 2, CLOSE) != TWST_OK)		// Getting register value (2 * 8 bits)
		return 0; 

	return (int16_t)(buffer[0] << 8 | buffer[1]);								// Return the 16 bit values
}