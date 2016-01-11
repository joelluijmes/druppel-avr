#include "eeprom.h"

static uint8_t eeprom_get_ctrl_id();
static uint16_t eeprom_data_address;

uint16_t eeprom_get_address() 
{
	if(eeprom_data_address == 0) 					// not initialized
	{
		uint8_t buffer[2]; 								// First 2 bytes are used to keep the last written address
		eeprom_read_page_address(0x00, &buffer[0], 2);
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
	eeprom_write_page_address(0x00, &buffer[0], 2);
	eeprom_data_address = address; 
}

// uint8_t eeprom_read_address(uint16_t address)
// {
// 	if (twi_mt_start(eeprom_get_ctrl_id()) != TWST_OK)
// 		return 0;

// 	twi_write(address >> 8);
// 	twi_write((uint8_t) address);

// 	if (twi_mr_start(eeprom_get_ctrl_id()) != TWST_OK)
// 		return 0;

// 	uint8_t receive = twi_peek(); 
// 	twi_stop(); 
// 	return receive; 
// }

void eeprom_read_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	uint8_t address_buffer[2]; 
	address_buffer[0] = (address >> 8);
	address_buffer[1] = (uint8_t) address;

	volatile TWRESULT status = twi_master_send(eeprom_get_ctrl_id(), &address_buffer[0], sizeof(address_buffer));
	if (status != TWST_OK)
	{
		return; 
	}

	status = twi_master_read(eeprom_get_ctrl_id(), buf, buflen);

	return; 

	// if (twi_mt_start(eeprom_get_ctrl_id()) != TWST_OK)
	// 	return;

	// twi_write(address >> 8);
	// twi_write((uint8_t) address);

	// if (twi_mr_start(eeprom_get_ctrl_id()) != TWST_OK)
	// 	return;

	// for(uint8_t i = 0; i < (buflen - 1); i++)
	// 	buf[i] = twi_read(); 
	// buf[buflen - 1] = twi_peek(); 

	// twi_stop();
}

// void eeprom_write_address(uint16_t address, uint8_t byte)
// {
// 	if (twi_mt_start(eeprom_get_ctrl_id()) != TWST_OK)
// 		return;

// 	twi_write((uint8_t) address >> 8);
// 	twi_write((uint8_t) address);
// 	twi_write(byte);
// 	twi_stop();
// 	_delay_ms(10);
// }

void eeprom_write_page_address(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	uint8_t buffer[buflen + 2]; 
	buffer[0] = (address >> 8);
	buffer[1] = (uint8_t) address;

	volatile TWRESULT status = twi_master_send(eeprom_get_ctrl_id(), &buffer[0], sizeof(buffer));

	return; 
}

static uint8_t eeprom_get_ctrl_id()
{
	return EEPROM_24LC256_CTRL_ID << 3 | EEPROM_24LC256_A2 << 2 | EEPROM_24LC256_A1 << 1 | EEPROM_24LC256_A0;
}