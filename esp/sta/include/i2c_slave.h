#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

void print_debug_info(void *arg);
void i2c_slave_intr_init(void);
void i2c_slave_init(void);
void user_i2c_debug(void);

// #define SDA_PIN 0x02
// #define SCL_PIN 0x00
#define SDA_PIN 0x05
#define SCL_PIN 0x04

#endif
