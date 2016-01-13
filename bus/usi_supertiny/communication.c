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

static tcp_state read_status();
static uint8_t send_command(tcp_state state);

uint8_t communication_send_data(uint8_t* data, uint8_t len)
{
	if(communication_avaible())													// Check if there is a tcp connection
		return 0;

	if(twi_master_send_byte(I2C_ADDR_ESP8266, len, KEEP_ALIVE) != TWST_OK)		// First send the count bytes to send
		return 0; 

	if(twi_master_send(I2C_ADDR_ESP8266, data, len, CLOSE) != TWST_OK)			// Sending the data
		return 0;
	return 1; 
}

uint8_t communication_avaible() 
{
	return read_status() != STATE_CONNECTED; 									// Return true if there is a tcp connection active
}

static tcp_state read_status()
{
	uint8_t data;
	if(twi_master_receive_byte(I2C_ADDR_ESP8266, &data, CLOSE) != TWST_OK)		// Read tcp status of the esp8266
		return 0; 
	return data;
}

static uint8_t send_command(tcp_state state)
{
	uint8_t buffer[2] = {255, state};											// Send first 255 to write a command otherwise it will send data to emmer

	if(twi_master_send(I2C_ADDR_ESP8266, buffer, 2, CLOSE) != TWST_OK)			// Send the command
		return 0;
	return 1;
}
