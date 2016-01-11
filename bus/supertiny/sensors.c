#include "sensors.h"

#define SENSORS_RECEIVE_BUFFER_SIZE 0x10

#define SENSORS_ADDRESS_START 0x00
#define SENSORS_ADDRESS_END 0x10
#define SENSORS_ADDRESS_LEN 9

typedef enum state state;
enum state
{
	STATE_NOT_ATTEMPTED = 0,
    STATE_NOT_CONNECTED = 1,
    STATE_CONNECTED = 2,
    STATE_READING_DONE = 3,
};

typedef enum command command;
enum command
{
	COMMAND_REQUEST_MEASUREMENT = 0,
	COMMAND_OK = 1
};

uint8_t total_written_bytes;

static state sensor_start_measure(uint8_t slave_address);
static state sensor_start_reading(uint8_t slave_address, uint32_t unixtime);
static uint8_t write_sensor_eeprom(uint32_t unixtime, uint8_t sensor_id, uint8_t* buf, uint8_t buflen);

static state _states[SENSORS_ADDRESS_LEN] = {};
uint8_t sensor_fill(uint8_t* data, uint8_t len)
{
	uint8_t filled = 0;
	uint32_t rawtime = read_unix_time();

	while (filled < len)
	{
		for (uint8_t address = SENSORS_ADDRESS_START; address < SENSORS_ADDRESS_END; ++address)
		{
			state* p_state = &_states[address - SENSORS_ADDRESS_END];
			switch (*p_state)
			{
			case STATE_FAILED:
			case STATE_NOT_ATTEMPTED:
				*p_state = request_measurement(address); 
				break;
			case STATE_MEASURING:
				*p_state = sensor_ready(); 
				break;
			case STATE_AVAILABLE:
				break;
			case STATE_CONNECTED:
				*p_state = request_measurement(address); 
				break;
			case STATE_READING_DONE:
				continue; 
			default:
				break;
			}
		}
	}
}

static state request_measurement(uint8_t slave_address)
{
	return (twi_master_send_byte(slave_address, COMMAND_REQUEST_MEASUREMENT, CLOSE) != TWST_OK)
		? STATE_FAILED
		: STATE_MEASURING;
}

static state sensor_ready(uint8_t slave_address)
{
	uint8_t data;
	return (twi_master_receive_byte(slave_address, &data, CLOSE) != TWST_OK)
		? STATE_FAILED
		: (data == COMMAND_OK)
			? STATE_AVAILABLE
			: STATE_MEASURING;
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