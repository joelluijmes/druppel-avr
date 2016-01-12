#include <avr/io.h>
#include <util/delay.h>

#include "../twi/twi.h"
#include "bmp180.h"

/*

provided altitude: 1655 meters, 5430 feet
temperature: 22.16 deg C, 71.89 deg F
absolute pressure: 1026.23 mb, 30.31 inHg
relative (sea-level) pressure: 1253.37 mb, 37.02 inHg
computed altitude: 1655 meters, 5430 feet

provided altitude: 1655 meters, 5430 feet
temperature: 22.15 deg C, 71.87 deg F
absolute pressure: 1026.19 mb, 30.31 inHg
relative (sea-level) pressure: 1253.32 mb, 37.01 inHg
computed altitude: 1655 meters, 5430 feet
*/ 

int main()
{
	while(1) 
	{
		double temperature = read_temperature(); 
		_delay_ms(500);
		double pressure = read_presure(); 
		_delay_ms(500);
	}
}