#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "twi.h"
#include "uart.h"

#include "bmp180.h"


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

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
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;
	PORTC = 1 << 4 | 1 << 5;
	PORTB = 0x20;

	printf("test\n");

	twi_master_init();

	while(1) 
	{
		//read_temperature(); 
		read_presure(); 
		printf("\n");
		_delay_ms(2000);
	}
}