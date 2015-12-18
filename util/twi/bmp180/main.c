#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../twi.h"
#include "../../uart/uart.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void start_measure(uint8_t slave) 
{
	_delay_ms(2000);
	printf("Starting\n");
	if (twi_mt_start(slave) != TWST_OK)
		return; 

	printf("1\n");

	twi_write(0xF4); // Control register value; 


	twi_write(0x2E); 

	twi_stop(); 
	// return; 

	// printf("2\n");

	// if(twi_mr_start(slave) != TWST_OK)
	// 	continue;
	// printf("3\n");

	// uint8_t value_msb  = twi_read();
	// printf("4\n");

	// uint8_t value_lsb = twi_peek(); 
	// printf("5\n");


	// printf("%d: %d\n", value_msb, value_lsb);


	// twi_stop();
}

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

	uint8_t something = 1;
	uint8_t slave = 0x77;
	uint8_t status;

	start_measure(slave); 

	while (1)
	{
		_delay_ms(2000);
		printf("Starting\n");
		if (twi_mt_start(slave) != TWST_OK)
			continue;

		printf("1\n");

		twi_write(0xF6); // Temperature; 
		printf("2\n");
		
		if(twi_mr_start(slave) != TWST_OK)
			continue;
		printf("3\n");

		uint8_t value_msb  = twi_read();
		printf("4\n");

		uint8_t value_lsb = twi_peek(); 
		printf("5\n");


		uint16_t UT = value_msb << 8 | value_lsb; 

		uint16_t T = (value_msb << 8 ) + value_lsb; 
		printf("%d: %d: %d : %d\n", value_msb, value_lsb, UT, T);


		twi_stop();


		printf("ok\n");

		//twi_write(something);
		//twi_stop();

		_delay_ms(1000);
	}
}