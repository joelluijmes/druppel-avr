#include <util/delay.h>
#include <avr/io.h>

#include "../../util/twi/twi.h"
// #include "../../util/twi/usi/twi_usi.h"
// #include "../../util/eeprom_usi/eeprom.h"
#include "../../util/ds1307_usi/ds1307.h"
#include "../../util/bmp180_usi/bmp180.h"

#define PORT_LED PORTB
#define DDR_LED DDRB
#define PIN_LED PINB
#define MASK_LED (1 << 4)

#define LED_HIGH() (PORT_LED |= MASK_LED)
#define LED_LOW() (PORT_LED &= ~MASK_LED)
#define LED_TOGGLE() (PIN_LED = MASK_LED)
#define LED_OUTPUT() (DDR_LED |= MASK_LED)

// ../../util/twi/twi.c 
int main()
{
	LED_OUTPUT();

	//TWRESULT twi_master_read(uint8_t slaveaddr, uint8_t* buffer, uint8_t len)

	//usi_init_master();

	//volatile TWRESULT status = twi_master_read(uint8_t slaveaddr, uint8_t* buffer, uint8_t len)


	// _delay_ms(10);
	uint8_t buff[64]; 
	for(uint8_t i = 0; i < 64; i++)
		buff[i] = 0xAB; //255; 
	// eeprom_write_page_address(0x00, &buff, 64);
	// _delay_ms(10);
	//eeprom_read_page_address(0x10, &buffb[0], 64);

	//uint32_t unixtime = read_unix_time(); 


	double test = read_temperature(); 

	// volatile TWRESULT status = usi_start_master(0x08, 1);
	// if (status != TWST_OK)
	// {
	// 	while(1)
	// 	{
	// 		LED_TOGGLE();
	// 		_delay_ms(100);
	// 	}
	// }

	// LED_HIGH();
	
	// for (uint8_t i = 0; i < 25; ++i)
	// {
	// 	volatile uint8_t ack = usi_write_master(++i);
	// 	//TODO : ack check?
	// }
	
	// usi_stop();
	// LED_LOW();

	// while(1);
	// return 0;
}
