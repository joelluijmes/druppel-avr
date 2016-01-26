#include "sensors.h"

#define SENSORS_RECEIVE_BUFFER_SIZE 0x10

#define SENSORS_ADDRESS_START 0x11
#define SENSORS_ADDRESS_END 0x11
#define SENSORS_ADDRESS_LEN (SENSORS_ADDRESS_END - SENSORS_ADDRESS_START + 1)

#define DATA_HEADER_LEN  6

typedef enum state state;
enum state
{
	STATE_NOT_ATTEMPTED = 0,
    STATE_NOT_CONNECTED,
    STATE_CONNECTED,
    STATE_BUFFER_FULL,
    STATE_COMPLETED,
    STATE_FAILED,
    STATE_MEASURING,
    STATE_AVAILABLE
};

typedef enum command command;
enum command
{
	COMMAND_REQUEST_MEASUREMENT,
	COMMAND_OK
};

static state request_measurement(uint8_t slave_address);
static state sensor_ready(uint8_t slave_address);
static int8_t read_sensor(uint8_t slave_address, uint8_t* buf, uint8_t len);

uint8_t sensor_fill(uint8_t* data, uint8_t datalen)
{
	state _states[SENSORS_ADDRESS_LEN] = {0};
	uint8_t offset = 0;

	uint8_t completed = 0;												// 'dirty' flag, will be cleared by every sensor if their state has changed
	while (!completed)													// if not it indicates we checked all sensors
	{
		completed = 1;
		for (uint8_t address = SENSORS_ADDRESS_START; address <= SENSORS_ADDRESS_END; ++address)
		{
			state* p_state = _states + address - SENSORS_ADDRESS_START;	// The array starts at 0, the first address isn't gaurunteed to be at 0 tho
																		// so for the correct array index we need to subtract the last address
																		// (The addresses are sequential ;) )
			switch (*p_state)
			{
			case STATE_COMPLETED:										// Already completed this sensor
				continue;
			case STATE_FAILED:											// It failed at some point so we start over
			case STATE_NOT_ATTEMPTED:									// It's not yet attempted 
				*p_state = request_measurement(address); 
				break;
			case STATE_MEASURING:										// we sent the request to start measuring
				*p_state = sensor_ready(address); 
				break;
			case STATE_AVAILABLE:										// Measurement completed -> start reading :D
			{
				int8_t len = read_sensor(address, data + offset, datalen - offset);
				if (len > 0)											// completed with this sensor :D
				{
					data[offset] = address;								// Data format:
					//*((uint32_t*)(data + offset + 1)) = time;			// ID | TIME | DATA_LEN | DATA
					data[offset + sizeof(uint32_t) + 1] = len;

					*p_state = STATE_COMPLETED;
					offset += len + DATA_HEADER_LEN;
					continue;
				}
				else if (len < 0)										// woudln't fit in the buffers
					return offset;
				else // len == 0
					*p_state = STATE_FAILED;							// Something went wrong

				break;
			}
			default:
				break;
			}

			_delay_ms(5);												// wait between devices 
			_wdt_reset();
			completed = 0;												// Something has changed
		}
	}

	return offset;
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

	// TODO: Add timeout
	if (twi_master_receive_byte(slave_address, &data, CLOSE) != TWST_OK)
		return STATE_MEASURING;

	return (twi_master_send_byte(slave_address, COMMAND_OK, CLOSE) != TWST_OK)
		? STATE_FAILED
		: (data == COMMAND_OK)
			? STATE_AVAILABLE
			: STATE_FAILED;
}

static int8_t read_sensor(uint8_t slave_address, uint8_t* buf, uint8_t len)
{
	uint8_t sensorlen;
	if (twi_master_receive_byte(slave_address, &sensorlen, CLOSE) != TWST_OK)
		return -1;

	if (sensorlen + DATA_HEADER_LEN > len)
		return 0;

	return (twi_master_receive(slave_address, buf + DATA_HEADER_LEN, sensorlen, CLOSE) == TWST_OK)
		? sensorlen
		: -1;
}