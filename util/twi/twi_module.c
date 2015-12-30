#include "twi.h"

#define WAIT() do { } while ((TWCR & (1 << TWINT)) == 0)

typedef uint8_t BOOL;

static void init_clock()
{
	TWSR = 0x00;										// No prescaler
	TWBR = ((F_CPU/SCL_CLOCK) - 16) >> 1;				// freq SCL = ((F_CPU/SCL_CLOCK) - 16) / 2
}

static BOOL start_master()
{
	// The devices tries to become master by checking if the bus is free. Then it sends a START condition
	// to claim the bus, if its not free it waits until a STOP condition is issued.
	// Writing 1 to TWINT clears the flag, the TWI will not start as long the TWINT is set.
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;			// Clears INT | Start bit | Enable TWI
	WAIT();

	return !!(TW_STATUS == TW_START || TW_STATUS == TW_REP_START);	// true if we got hold of the bus
}

void twi_master_init()
{
	init_clock();
}

void twi_slave_init(uint8_t slave_addr)
{
	init_clock();
	
	// Enable acknowledge makes the slave ack when it receives its slave address or when a general
	// call has been received (not enabled tho)
	TWCR =  1<< TWINT |1 << TWEA | 1 << TWEN;			// Clears INT | Enable Acknowledge | ENable TWI
	TWAR = slave_addr << 1;								// set its slave address
	WAIT();				
}

void twi_write(uint8_t data)
{
	TWDR = data;										// set the data to transmit
	TWCR = 1 << TWINT | 1 << TWEN;						// starts operation
	WAIT();				
}

TWRESULT twi_mt_start(uint8_t slave_addr)
{
	if (!start_master())								// if we can't get hold of the bus
		return TWST_START_FAILED;						// we exit
	
	twi_write((slave_addr << 1) | TW_WRITE);			// say we're going to write to slave
	WAIT();

	return (TW_STATUS == TW_MT_SLA_ACK)					// depending on we get an ack of the slave
		? TWST_OK
		: TWST_MT_ACK_EXPECTED;							// FAILED
}

TWRESULT twi_mr_start(uint8_t slave_addr)
{
	if (!start_master())								// if we can't get hold of the bus
		return TWST_START_FAILED;						// we exit
	
	twi_write((slave_addr << 1) | TW_READ);				// say we're expect something of the slave
	WAIT();

	return (TW_STATUS == TW_MR_SLA_ACK)					// depending on we get an ack of the slave
		? TWST_OK
		: TWST_MR_ACK_EXPECTED;							// FAILED
}

void twi_stop()
{
	TWCR = 1 << TWINT | 1 << TWSTO | 1 << TWEN;			// Releases the bus
	WAIT();
}

uint8_t twi_read()
{
	TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN;			// Enables TWI | Send ACK after op
	WAIT();

	return TWDR;										// Returns data
}

uint8_t twi_peek()
{
	TWCR = 1 << TWINT | 1 << TWEN;						// Enables TWI (Note: we don't send ack after op, thus nack)
	WAIT();

	return TWDR;
}