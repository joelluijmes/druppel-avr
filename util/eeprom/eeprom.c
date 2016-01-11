#include "eeprom.h"

#define ADDR_EEPROM (EEPROM_24LC256_CTRL_ID << 3 | EEPROM_24LC256_A2 << 2 | EEPROM_24LC256_A1 << 1 | EEPROM_24LC256_A0)
static uint16_t eeprom_data_address;

uint16_t eeprom_get_address() 
{
	if(eeprom_data_address == 0) 					// not initialized
	{
		uint8_t buffer[2]; 								// First 2 bytes are used to keep the last written address
		eeprom_read_page_address(0x00, (uint8_t*) &buffer, 2);
		eeprom_data_address = buffer[0] << 8 | buffer[1];
		if(eeprom_data_address == 65535)
			eeprom_set_address(0x10);
	}
	return eeprom_data_address; 
}

void eeprom_set_address(uint16_t address) 
{
	uint8_t buffer[2];
	buffer[0] = (uint8_t) (address >> 8);
	buffer[1] = (uint8_t) address;
	eeprom_write_page_address(0x00, (uint8_t*) &buffer, 2);
	eeprom_data_address = address; 
}

uint8_t eeprom_read_address(uint16_t address)
{
	if (twi_mt_start(ADDR_EEPROM) != TWST_OK)
		return 0;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	if (twi_mr_start(ADDR_EEPROM) != TWST_OK)
		return 0;

	uint8_t receive = twi_peek(); 
	twi_stop(); 
	return receive; 
}

void eeprom_read_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (twi_mt_start(ADDR_EEPROM) != TWST_OK)
		return;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	if (twi_mr_start(ADDR_EEPROM) != TWST_OK)
		return;

	for(uint8_t i = 0; i < (buflen - 1); i++)
		buf[i] = twi_read(); 
	buf[buflen - 1] = twi_peek(); 

	twi_stop();
}

void eeprom_write_address(uint16_t address, uint8_t byte)
{
	if (twi_mt_start(ADDR_EEPROM) != TWST_OK)
		return;

	twi_write((uint8_t) address >> 8);
	twi_write((uint8_t) address);
	twi_write(byte);
	twi_stop();
	_delay_ms(10);
}

void eeprom_write_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (twi_mt_start(ADDR_EEPROM) != TWST_OK)
		return;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	for(uint8_t i = 0; i < buflen; i++)
		twi_write(buf[i]); 

	twi_stop();
	_delay_ms(10);
}