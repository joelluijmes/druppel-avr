#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/wdt.h"
#include "../../util/twi/twi.h"
#include "../../util/eeprom_usi/eeprom.h"

#include "communication.h"
#include "sensors.h"
#include "time.h"

#define EEPROM_END_ADDR 0x08
#define EEPROM_BEGIN_ADDR 0x04
#define EEPROM_DEFAULT_ADDR 0x20

#define BUF_LEN 64

static uint16_t read_eeprom_uint16(uint8_t address)
{
	uint8_t buf[2];
	return (eeprom_read(address, buf, 2))
		?  buf[0] << 8 | buf[1]
		: -1;
}

static uint8_t write_eeprom_uint16(uint8_t address, uint16_t data)
{
	uint8_t buf[2] = { data >> 8, data };
	return eeprom_write(address, buf, 2);
}

static uint8_t flush_eeprom()
{
	uint16_t beginAddess = read_eeprom_uint16(EEPROM_BEGIN_ADDR);
	uint16_t endAddress = read_eeprom_uint16(EEPROM_END_ADDR);

	uint8_t buf[BUF_LEN];
	uint16_t currentAddress = beginAddess;
	while (currentAddress < endAddress)
	{
		_wdt_reset();
		uint16_t left = endAddress - currentAddress;
		uint8_t len = (left < BUF_LEN) 
			? left
			: BUF_LEN;

		if (!eeprom_read(currentAddress, buf, len))
			return 0;

		if (!communication_available())
			return 0;

		if (!communication_send(buf, len))
			return 0;

		currentAddress += len;
		write_eeprom_uint16(EEPROM_BEGIN_ADDR, currentAddress);
		_delay_ms(5);
	}

	return 1;
}

int main()
{
	_wdt_enable(WDTO_30MS);
	//write_eeprom_uint16(EEPROM_BEGIN_ADDR, EEPROM_DEFAULT_ADDR);
	//_delay_ms(50);
	//
	//write_eeprom_uint16(EEPROM_END_ADDR, EEPROM_DEFAULT_ADDR);
	//_delay_ms(50);
	
	uint16_t addr = read_eeprom_uint16(EEPROM_END_ADDR);
	uint8_t data[BUF_LEN];

	while (0 && 1)
	{
		communication_available();											// trigger communication (to get active i.e.)
		uint8_t len = sensor_fill(data, BUF_LEN);

		eeprom_write(addr, data, len);
		_delay_ms(5);
		_wdt_reset();														// reset after delay -> dont keep the watchdog waiting

		addr += len;
		write_eeprom_uint16(EEPROM_END_ADDR, addr);

		_delay_ms(10);
		_wdt_reset();
		//if (addr > 10)
		if (communication_available())
			flush_eeprom();
	}


	uint8_t buf[64];
	uint8_t len = 64; 
	for(uint8_t i = 0; i < len; i++)
		buf[i] = i; 
	while(1)
	{
		_wdt_reset();
		//_delay_us(150); 
		_delay_ms(5); 
		if (!communication_available())
			continue;

		if (!communication_send(buf, len))
			continue;
	}
}