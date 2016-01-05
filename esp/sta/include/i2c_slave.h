#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__


uint8_t i2c_read_pin(uint8_t pin);
void i2c_update_status(uint8_t status);
void i2c_interupt_reading_address();
void i2c_interupt_reading_start();
void print_debug_info(void *arg);
void i2c_slave_intr_init(void);
void i2c_slave_init(void);
void user_i2c_debug(void);

#endif
