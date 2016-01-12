#include "communication.h"

#define COMMUNICATION_ADDRESS_WIFI 0x10

#define STATUS_BUSY 0x01
#define STATUS_READY 0x02

static uint8_t communication_ready(uint8_t slave_address);
static void communication_sent_bytes(uint8_t slave_address, uint8_t* buf, uint8_t buflen);

uint8_t communication_start(uint8_t slave_address)
{
	uint16_t last_address = eeprom_get_address();

	if(last_address <= 0x10)		// No data to sent...
		return 0; 

	uint16_t current_address = 0x00; 
	uint8_t buffer[64] = {}; 

	while(last_address > current_address )
	{
		while(!communication_ready(COMMUNICATION_ADDRESS_WIFI))			// Wait until communication respond with ready
			_delay_ms(1500); 

		eeprom_read_page_address(current_address, (uint8_t*) &buffer, 64);
		uint8_t bytes_count = 64; 
		if(last_address < (current_address + 64))
			bytes_count = last_address - current_address; 				// Number of bytes to sent...

		if(current_address == 0x00)
			communication_sent_bytes(COMMUNICATION_ADDRESS_WIFI, (uint8_t*) &buffer[0x10], bytes_count);
		else
			communication_sent_bytes(COMMUNICATION_ADDRESS_WIFI, (uint8_t*) &buffer, bytes_count);

		current_address += 64; 
	}

	//eeprom_set_address(0x10);			//Writing all bytes to communication line is done so set our new pointer in eeprom
	return 1; 
}

static uint8_t communication_ready(uint8_t slave_address)
{
	if (twi_mr_start(slave_address) != TWST_OK)
		return 0; 

	uint8_t status = twi_peek(); 
	
	twi_stop(); 

	return status == STATUS_READY;
}

static void communication_sent_bytes(uint8_t slave_address, uint8_t* buf, uint8_t buflen)
{
	_delay_ms(1); 
	if (twi_mt_start(slave_address) != TWST_OK)
		return;

	twi_write(buflen); 								// Sending count bytes

	for(uint8_t i = 0; i < buflen; i++)
		twi_write(buf[i]); 

	twi_stop();
	_delay_ms(1);
}