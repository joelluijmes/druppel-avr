#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi_module.h"
#include "../../util/uart/uart.h"
#include "../../util/ds1307/ds1307.h"

#include "sensors.h"
#include "communication.h"


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	twi_master_init();



	uint32_t unixtime = read_unix_time();
	//sensors_check(); 



	_delay_ms(10);
	uint8_t buff[64]; 
	// for(uint8_t i = 0; i < 64; i++)
	// 	buff[i] = 255; 
	// eeprom_write_page_address(0x00, &buff, 64);
	// _delay_ms(10);
	//eeprom_read_page_address(0x00, &buff, 64);



	//communication_start(0x20); 



}