#include "twi_usi.h"

void init()
{
	USISR = 1 << USISIF | 1 << USIOIF | 1 << USIOIF;
	USICR = 1 << USIWM1 | 1 << USICLK | 1 << USICLK;
}

// Initializes as master
// void twi_master_init()
// {
	
// }

// Initializes as slave with address 
// void twi_slave_init(uint8_t slave_addr);

// // Writes data to bus (note: should be in transmit mode)
// void twi_write(uint8_t data);

// // Starts transmission mode (MASTER)
// TWRESULT twi_mt_start(uint8_t slave_addr);

// // Starts receiver mode (MASTER)
// TWRESULT twi_mr_start(uint8_t slave_addr);

// // Releases the bus (must be master)
// void twi_stop();

// // Reads data from the bus (sends ACK)
// uint8_t twi_read();

// // Peeks data from the bus (sends NACK)
// uint8_t twi_peek();
