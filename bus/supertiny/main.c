#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi_module.h"
#include "../../util/uart/uart.h"
#include "../../util/ds1307/ds1307.h"

#include "sensors.h"
#include "communication.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static uint16_t read_addr()
{
	uint8_t buf[2];
	eeprom_read(buf, 2);

	return buf[0] << 8 | buf[1];
}

int main()
{
	uint8_t data[64];
	
	addr = read_addr();

	while (1)
	{
		uint8_t len = sensor_fill(data, 64);

		eeprom_write(addr, data, len);
		addr += len;

		if (communcation_active())
			communication_send(data, 64);
	}




	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	PORTC = 1 << 4 | 1 << 5;

	twi_master_init();



	// uint32_t unixtime = read_unix_time();
	//sensors_check(); 


	// _delay_ms(10);
	uint8_t buff[64]; 
	for(uint8_t i = 0; i < 64; i++)
		buff[i] = 0xAB; //255; 
	// eeprom_write_page_address(0x00, &buff, 64);
	// _delay_ms(10);
	//eeprom_read_page_address(0x00, &buff, 64);




	uint8_t status = communication_start(0x10); 
	printf("%d\n", status);



	// while(1)
	// {
	// 	_delay_ms(500);
	// 	if (twi_mt_start(0x10) != TWST_OK)
	// 		continue;

	// 	twi_write(2); 
	// 	for(uint8_t i = 0; i < 2; i++)
	// 		twi_write(buff[i]); 

	// 	twi_stop();
	// }

}