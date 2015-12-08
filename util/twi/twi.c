#include "twi.h"

void twi_init()
{
	TWSR = 0x00;										//No prescaler
	TWBR = ((F_CPU/SCL_CLOCK) - 16) >> 1;				//freq SCL = ((F_CPU/SCL_CLOCK) - 16) / 2
	//TWCR = 1 << TWEN | 1 << TWIE | 1 << TWEA;			//Enables | Acknowledges
}

uint8_t twi_start()
{
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;		//Enables TW_interrupt | Start bit | TWI_Enable
	while ((TWCR & (1 << TWINT)) == 0);

	return twi_status();
}

uint8_t twi_start_slave(uint8_t slave_addr)
{
	TWAR = slave_addr << 1;
	TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN;

	while ((TWCR & (1 << TWINT)) == 0) ;
	return twi_status();
}

uint8_t twi_write(uint8_t data)
{
	TWDR = data;
	TWCR = 1 << TWINT | 1 << TWEN;
	while ((TWCR & (1 << TWINT)) == 0) puts(".");			//Waits for completion

	return twi_status();
}

void twi_mt_start(uint8_t slave_addr)
{
	uint8_t status;
	while ((status = twi_write((slave_addr << 1) | TW_WRITE)) != TW_MT_SLA_ACK)
	{
		printf("%x", twi_status());
		twi_start();
	}
}

void twi_mr_start(uint8_t slave_addr)
{
	uint8_t status;
	while ((status = twi_write((slave_addr << 1) | TW_READ)) != TW_MR_SLA_ACK)
		twi_start();
}

void twi_stop()
{
	TWCR = 1 << TWINT | 1 << TWSTO | 1 << TWEN;		
	while ((TWCR & (1 << TWSTO)) == 0) ;
}

uint8_t twi_read()
{
	TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN;		//Sends ack after reading
	while ((TWCR & (1 << TWINT)) == 0) ;
	return TWDR;
}

uint8_t twi_peek()
{
	TWCR = 1 << TWINT | 1 << TWEN;					//Don't send ack after reading
	while ((TWCR & (1 << TWINT)) == 0) ;
	return TWDR;
}