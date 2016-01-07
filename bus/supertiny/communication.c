#include "communication.h"

uint8_t communication_start()
{

	uint16_t last_address = eeprom_get_address();

	if(last_address <= 0x10)		// No data to sent...
		return 0; 

	uint16_t current_address = 0x00; 

	uint8_t buffer[64] = {}; 


	while(last_address > current_address )
	{
		eeprom_read_page_address(current_address, &buffer, 64);
		uint8_t bytes = 64; 
		if(last_address < (current_address + 64))
			bytes = last_address - current_address; 		// Bytes to sent...

		//TODO sent bytes...
		

		
	

		current_address += 64; 
	}
	
	_delay_ms(10);
	uint8_t buff[64]; 
	for(uint8_t i = 0; i < 64; i++)
		buff[i] = 255; 
	eeprom_write_page_address(0x00, &buff, 64);
	_delay_ms(10);
	eeprom_read_page_address(0x00, &buff, 64);



		twi_write(0x02);												// Sending reading command

	if (twi_mr_start(slave_address) != TWST_OK)
		return 0;

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

uint8_t communication_status(uint8_t slave_address)
{
	if (twi_mr_start(slave_address) != TWST_OK)
		return 0;

	uint8_t status = twi_read(); 
	
	twi_stop(); 

	return status; 
}