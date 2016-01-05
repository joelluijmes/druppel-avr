#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../twi/twi_module.h"
#include "../uart/uart.h"

#include "eeprom.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	twi_master_init();

	_delay_ms(20);


	uint8_t buffer[64]; 
	for(uint8_t i; i < 64; i++)
		buffer[i] = i;
	write_page_address(0x00, &buffer, 64); 

	_delay_ms(250); 

	for(uint8_t i; i < 64; i++)
		buffer[i] = i;
	write_page_address(0x10, &buffer, 20); 

	for(uint8_t i = 0; i < 64; i++)
		buffer[i] = 0;

	_delay_ms(250); 

	read_page_address(0x00, &buffer, 64);

	for(uint8_t i = 0 ; i < 64; i++)
		printf("%d ", buffer[i]);

	printf("\n\n");

	_delay_ms(250); 

	printf("%d %d \n", read_address(0x00), read_address(0x01));

}