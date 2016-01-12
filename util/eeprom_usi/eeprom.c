#include "eeprom.h"

#define EEPROM_24LC256_CTRL_ID 10
#define EEPROM_24LC256_A0 0
#define EEPROM_24LC256_A1 0
#define EEPROM_24LC256_A2 0

#define I2C_ADDR_DEVICE (EEPROM_24LC256_CTRL_ID << 3 | EEPROM_24LC256_A2 << 2 | EEPROM_24LC256_A1 << 1 | EEPROM_24LC256_A0)

uint8_t access_address(uint16_t address)
{
	uint8_t buffer[] = { address >> 8, (uint8_t)address };
	return twi_master_send(I2C_ADDR_DEVICE, buffer, 2, KEEP_ALIVE) == TWST_OK;
}

uint8_t eeprom_write(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (!access_address(address))
	{
		twi_close();
		return 0;
	}

	TWRESULT result = twi_master_send(I2C_ADDR_DEVICE, buf, buflen, CLOSE);
	return result == TWST_OK;
}

uint8_t eeprom_read(uint16_t address, uint8_t* buf, uint8_t buflen)
{
	if (!access_address(address))
	{
		twi_close();
		return 0;
	}

	twi_close();																// reopen as master receive
	TWRESULT result = twi_master_receive(I2C_ADDR_DEVICE, buf, buflen, CLOSE);
	return result == TWST_OK;
}
