#include "communication.h"

#define I2C_ADDR_ESP8266 0x10

enum tcp_state
{
    STATE_IDLE = 0, 
    STATE_CONNECT = 1,
    STATE_DISCONNECT = 2,
    STATE_CONNECTED = 3, 
    STATE_DISCONNECTED = 4,
    STATE_BUSY = 5,
};
typedef enum tcp_state tcp_state;

static uint8_t read_status(tcp_state state);
static uint8_t send_command(tcp_state state);

static tcp_state tcp_state; 

uint8_t communication_send_data(uint8_t* data, uint8_t len)
{
	if(communication_avaible())
		return 0; 

	if(twi_master_send_byte(I2C_ADDR_ESP8266, len, KEEP_ALIVE) != TWST_OK)
		return 0; 

	if(twi_master_read(I2C_ADDR_ESP8266, data, len, CLOSE) != TWST_OK)
		return 0;
}

uint8_t communication_avaible()
{
	return read_command() == STATE_CONNECTED; 
}

static uint8_t read_status(tcp_state state)
{
	uint8_t state;
	if(twi_master_receive_byte(I2C_ADDR_ESP8266, &state, CLOSE) != TWST_OK)		// Read tcp status of the esp8266
		return 0; 
	return state;
} 

static uint8_t send_command(tcp_state state)
{
	uint8_t buffer[2] = {255, state};										// Send first 255 to write a command otherwise it will send data to emmer

	if(twi_master_send(I2C_ADDR_ESP8266, buffer, 2, CLOSE) != TWST_OK)		// Send the command
		return 0;
	return 1;
}
