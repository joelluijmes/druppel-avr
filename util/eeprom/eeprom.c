#include "eeprom.h"



uint8_t read_address(uint16_t address)
{
	if (twi_mt_start(get_ctrl_id()) != TWST_OK)
		return 0;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	if (twi_mr_start(get_ctrl_id()) != TWST_OK)
		return 0;

	uint8_t receive = twi_peek(); 
	twi_stop(); 
	return receive; 
}

void read_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (twi_mt_start(get_ctrl_id()) != TWST_OK)
		return 0;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	if (twi_mr_start(get_ctrl_id()) != TWST_OK)
		return 0;

	for(uint8_t i = 0; i < buflen; i++)
		buf[i] = twi_read(); 

	twi_stop();
}

void write_address(uint16_t address, uint8_t byte)
{
	if (twi_mt_start(get_ctrl_id()) != TWST_OK)
		return 0;

	twi_write((uint8_t) address >> 8);
	twi_write((uint8_t) address);
	twi_write(byte);
	twi_stop();
	_delay_ms(10);
}

void write_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (twi_mt_start(get_ctrl_id()) != TWST_OK)
		return 0;

	twi_write(address >> 8);
	twi_write((uint8_t) address);

	for(uint8_t i = 0; i < buflen; i++)
		twi_write(buf[i]); 

	twi_stop();
	_delay_ms(10);
}

uint8_t get_ctrl_id()
{
	return EEPROM_24LC256_CTRL_ID << 3 | EEPROM_24LC256_A2 << 2 | EEPROM_24LC256_A1 << 1 | EEPROM_24LC256_A0;
}