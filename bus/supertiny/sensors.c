#include "eeprom.h"

uint16_t data_address; 

uint8_t sensors_check()
{
	//uint8_t sensors to do; //SENSORS_ADDRESS_SIZE
	uint8_t current_sensors[SENSORS_ADDRESS_SIZE];
	uint8_t devices_count = 0; 
	uint8_t receive_buffer[SENSORS_RECEIVE_BUFFER_SIZE]; 

	for(uint8_t address = SENSORS_ADDRESS_FROM; address < SENSORS_ADDRESS_TO; address++)
	{
		if (twi_mt_start(address) != TWST_OK)
			continue; 								// Device is not responding

		current_sensors[devices_count] = address; 
		devices_count++; 

		twi_write(0x01);							// Sending command code
		twi_stop(); 
	}

	if(devices_count == 0)
		return -1; 

	// There are devices so get the current time. 
	uint32_t unixtime = read_unix_time();

	uint8_t total_written_bytes = 0;

	for(uint8_t device = 0; device < devices_count; device++)
	{
		if (twi_mt_start(current_sensors[device]) != TWST_OK)
			continue; 

		twi_write(0x02);							// Sending ready command

		if (twi_mr_start(current_sensors[device]) != TWST_OK)
			continue; 

		uint8_t bytes = twi_read();					// Read how many bytes there must be sent 

		uint8_t i = 0; 
		while(i < bytes && i < SENSORS_RECEIVE_BUFFER_SIZE)
		{
			receive_buffer[i] = twi_read(); 
			i++; 
		}
		twi_stop(); 								// All bytes received, sending stop condition.

		// Writing data to eeprom
		total_written_bytes += write_sensor_eeprom(unixtime, current_sensors[device], &receive_buffer, bytes);
		device++; 
	}
}

uint8_t write_sensor_eeprom(uint32_t unixtime, uint8_t sensor_id, uint8_t* buf, uint8_t buflen)
{
	// DATA: sensor_id(1), timestamp(4), length(1), data(buflen)
	uint8_t datasize = SENSORS_RECEIVE_BUFFER_SIZE + 6;
	uint8_t buffer[datasize]; 
	buffer[0] = sensor_id;
	buffer[1] = (uint8_t) (unixtime >> 24);
	buffer[2] = (uint8_t) (unixtime >> 16);
	buffer[3] = (uint8_t) (unixtime >> 8);
	buffer[4] = (uint8_t) unixtime;
	buffer[5] = buflen; 
	for(uint8_t i = 0; i < buflen; i++)
		buffer[6 + i] = buf[i];
	write_page_address(sensors_get_eeprom_address(), &buffer, datasize);
	return datasize; 
}

void sensors_get_eeprom_address() 
{
	if(data_address == 0) 				// not initialized
	{
		// First 2 bytes are used to keep the last written address
		uint8_t buffer[2]; 
		eeprom_read_page_address(0x00, &buffer, 2);
		data_address = buffer[0] << 8 | buffer[1];
		if(data_address == 65535) 
		{
			buffer[0] = 0;
			buffer[1] = 0x10;
			eeprom_write_page_address(0x00, &buffer, 2);
			data_address = 0x10; 
		}
	}
	return data_address; 
}