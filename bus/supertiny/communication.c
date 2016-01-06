#include "communication.h"

void communication_start(uint8_t slave_address)
{
	uint8_t receive_buffer[SENSORS_RECEIVE_BUFFER_SIZE]; 
	if (twi_mt_start(slave_address) != TWST_OK)
		return CONNECTED;

	twi_write(0x02);												// Sending reading command

	if (twi_mr_start(slave_address) != TWST_OK)
		return CONNECTED;

	uint8_t bytes = twi_read();										// Read how many bytes there must be sent 
	bytes = 3; 

	uint8_t i = 0; 
	while(i < bytes && i < SENSORS_RECEIVE_BUFFER_SIZE)
	{
		receive_buffer[i] = twi_read(); 
		i++; 
	}
	twi_stop(); 													// All bytes received, sending stop condition.


	_delay_us(100);
	// Writing data to eeprom
	total_written_bytes += write_sensor_eeprom(uint8_txtime, slave_address, &receive_buffer, bytes);

	return READING_DONE; 


}