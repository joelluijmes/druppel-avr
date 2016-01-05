#include "sensors.h"

uint16_t data_address; 

static state sensor_start_measure(uint8_t slave_address);
static state sensor_start_reading(uint8_t slave_address);

uint8_t sensors_check()
{
	state states[SENSORS_ADDRESS_SIZE] = {};
	uint8_t receive_buffer[SENSORS_RECEIVE_BUFFER_SIZE]; 

	uint8_t done = false; 
	while(!done) // todo 
	{
		done = true;

		for(uint8_t address = SENSORS_ADDRESS_FROM; address < SENSORS_ADDRESS_TO; address++)
		{
			uint8_t state_address = address - SENSORS_ADDRESS_FROM;

			switch(states[state_address])
			{
			case NOT_ATTEMPTED:
				states[state_address] = sensor_start_measure(address); 
				break;
			case NOT_CONNECTED:
				continue; 
			case CONNECTED:
				// TODO... 
				printf("connected %d\n", address);
				states[state_address] = sensor_start_reading(address); 
				break;
			case READING_DONE:
				continue; 
			default:
				printf("default\n");
				break;
			}

			done = false; 
			_delay_us(100);
		}

	}
	
	// There are devices so get the current time. 
	uint32_t unixtime = read_unix_time();

	uint8_t total_written_bytes = 0;
}

static state sensor_start_measure(uint8_t slave_address)
{
	if (twi_mt_start(slave_address) != TWST_OK)
		return NOT_CONNECTED;							// Device is not responding

	twi_write(0x01);							// Sending measure command code
	twi_stop(); 
	return CONNECTED;
}

static state sensor_start_reading(uint8_t slave_address)
{
	if (twi_mt_start(slave_address) != TWST_OK)
		return CONNECTED;

	twi_write(0x02);							// Sending reading command

	if (twi_mr_start(slave_address) != TWST_OK)
		return CONNECTED;

	uint8_t bytes = twi_read();					// Read how many bytes there must be sent 

	uint8_t i = 0; 
	while(i < bytes && i < SENSORS_RECEIVE_BUFFER_SIZE)
	{
		receive_buffer[i] = twi_read(); 
		i++; 
	}
	twi_stop(); 								// All bytes received, sending stop condition.

	// Writing data to eeprom
	//total_written_bytes += write_sensor_eeprom(unixtime, current_sensors[device], &receive_buffer, bytes);
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
	eeprom_write_page_address(sensors_get_eeprom_address(), &buffer, datasize);
	return datasize; 
}

uint16_t sensors_get_eeprom_address() 
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