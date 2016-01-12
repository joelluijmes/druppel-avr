#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi_module.h"
#include "../../util/uart/uart.h"


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void start_reading(uint8_t slave)
{
	if (twi_mr_start(slave) != TWST_OK)
	//if (twi_mt_start(0x55) != TWST_OK)
		return;

	//twi_write(0x25); 
	char c = twi_read(); 
	//char x = twi_read(); 
	twi_stop(); 

	printf("Received byte: 0x%c %c\n", c);

	PINB = 0x20;
}

int main()
{
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	twi_master_init();


	while(1) 
	{
		_delay_ms(1000);

		// start_reading(0x08); 
		// _delay_ms(1);
		start_reading(0x55); 



	}


}