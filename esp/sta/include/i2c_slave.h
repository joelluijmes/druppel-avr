#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__


void i2c_slave_init(void); 
void i2c_interupt_reading_start();

void user_i2c_debug(void);

void i2c_slave_intr_init(void);
void i2c_interupt_reading_address(void);

#endif
