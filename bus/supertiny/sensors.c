#include "eeprom.h"

void check_sensors()
{
	//uint8_t sensors to do; //SENSORS_ADDRESS_SIZE

	for(uint8_t i = SENSORS_ADDRESS_FROM; i < SENSORS_ADDRESS_TO; i++)
	{
		if (twi_mt_start(i) != TWST_OK)
			continue; 						// Device is not responding


		twi_write(0x01);					// Sending command code
		twi_stop(); 


	}

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
