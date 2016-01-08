#include "sensors.h"

uint8_t total_written_bytes;

static state sensor_start_measure(uint8_t slave_address);
static state sensor_start_reading(uint8_t slave_address, uint32_t unixtime);
static uint8_t write_sensor_eeprom(uint32_t unixtime, uint8_t sensor_id, uint8_t* buf, uint8_t buflen);

uint8_t sensors_check()
{
	total_written_bytes = 0; 
	state states[SENSORS_ADDRESS_SIZE] = {};

	uint32_t unixtime = read_unix_time();					// Get the current unixtime to store in eeprom 

	uint8_t done = false; 
	while(!done)
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
				states[state_address] = sensor_start_reading(address, unixtime); 
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

	if(total_written_bytes) {
		_delay_us(100);
		
		eeprom_set_address(eeprom_get_address() + total_written_bytes); // Set new eeprom address
	}

	return 0;
}

static state sensor_start_measure(uint8_t slave_address)
{
	if (twi_mt_start(slave_address) != TWST_OK)
		return NOT_CONNECTED;										// Device is not responding

	twi_write(0x01);												// Sending measure command code
	twi_stop(); 
	return CONNECTED;
}

static state sensor_start_reading(uint8_t slave_address, uint32_t unixtime)
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
	total_written_bytes += write_sensor_eeprom(unixtime, slave_address, (uint8_t*) &receive_buffer, bytes);

	return READING_DONE; 
}


static uint8_t write_sensor_eeprom(uint32_t unixtime, uint8_t sensor_id, uint8_t* buf, uint8_t buflen)
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
	eeprom_write_page_address(eeprom_get_address(), (uint8_t*) &buffer, 6 + buflen);

	return 6 + buflen; 
}