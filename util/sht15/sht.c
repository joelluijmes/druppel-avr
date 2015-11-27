#include "sht.h"

#define SHT_CMD_MASK 0x1F
#define SHT_TEMP 0x03
#define SHT_HUMI 0x05
#define SHT_RSTATUS 0x7
#define SHT_WSTATUS 0x06
#define SHT_RESET 0x1A

static uint16_t readSht(const sht* sht, uint8_t command);
static void sendCommand(const sht* sht, uint8_t command);
static void waitResponse(const sht* sht);
static uint16_t readResponse(const sht* sht);
static void transmissionStart(const sht* sht);


double sht_readTemperature(const sht* sht)
{
	uint16_t raw = readSht(sht, SHT_TEMP);
	return ((0.01 * raw) - 39.7);
}

double sht_readHumidity(const sht* sht)
{
	uint16_t raw = readSht(sht, SHT_HUMI);
	return ((-0.0000015955*raw*raw) + (0.0367*raw) - 2.0468);
}

double sht_compensateHumidity(const sht* sht, double temperature)
{
	uint16_t raw = readSht(sht, SHT_HUMI);
	double humidity = ((-0.0000015955*raw*raw) + (0.0367*raw) - 2.0468);

	return (((temperature - 25.0) * (0.01 + (0.00008 * raw))) + humidity);
}

static uint16_t readSht(const sht* sht, uint8_t command)
{
	transmissionStart(sht);
	sendCommand(sht, command);
	waitResponse(sht);
	return readResponse(sht);
}

static void sendCommand(const sht* sht, uint8_t command)
{	
	pin_output(&sht->pinDATA);
	command &= SHT_CMD_MASK;								// makes sure address is 000

	// pin_output(&sht->pinDATA);
	// pin_low(&sht->pinDATA);
	// pin_low(&sht->pinSCK);
	for (uint8_t i = 0; i < 8; ++i)
	{
		pin_low(&sht->pinSCK);
		if (command & 0x80)
			pin_high(&sht->pinDATA);
		else
			pin_low(&sht->pinDATA);

		pin_high(&sht->pinSCK);

		command <<= 1;
	}

	pin_low(&sht->pinSCK);

}

static void waitResponse(const sht* sht)
{
	pin_input(&sht->pinDATA);								// get acknowledge

	while (pin_read(&sht->pinDATA))
		;													// slave pulls data low

	pin_high(&sht->pinSCK);
	pin_low(&sht->pinSCK);

	while (!pin_read(&sht->pinDATA))
		;

	while (pin_read(&sht->pinDATA))							// wait for completion
		_delay_ms(5);				
}

static uint16_t readResponse(const sht* sht)
{
	uint8_t response[3];
	pin_low(&sht->pinSCK);
	pin_input(&sht->pinDATA);

	for (uint8_t res = 0; res < 3; ++res)
	{
		response[res] = 0;
		for (uint8_t i = 0; i < 8; ++i)
		{
			_delay_us(10);
			pin_high(&sht->pinSCK);
			response[res] |= pin_read(&sht->pinDATA);
			pin_low(&sht->pinSCK);
			
			if (i != 7)
				response[res] <<= 1;
		}

		pin_output(&sht->pinDATA);
		pin_low(&sht->pinDATA);
		_delay_us(10);
		pin_high(&sht->pinSCK);
		_delay_us(10);
		pin_low(&sht->pinSCK);

		pin_input(&sht->pinDATA);
	}

	return (response[0] << 8) | response[1];
}

// 3.2 Sending a Command
// To initiate a transmission, a Transmission Start sequence 
// has to be issued. It consists of a lowering of the DATA line 
// while SCK is high, followed by a low pulse on SCK and 
// raising DATA again while SCK is still high
static void transmissionStart(const sht* sht)
{
	pin_low(&sht->pinSCK);
	pin_output(&sht->pinDATA);

	pin_high(&sht->pinDATA);	

	pin_high(&sht->pinSCK);	
	pin_low(&sht->pinDATA);

	pin_low(&sht->pinSCK);
	pin_high(&sht->pinSCK);
	
	pin_high(&sht->pinDATA);
	pin_low(&sht->pinSCK);
}
