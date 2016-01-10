#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

void i2c_slave_init(void);
void i2c_update_status(uint8_t status);

void print_debug_info(void *arg);
void i2c_slave_intr_init(void);
void user_i2c_debug(void);

// #define SDA_PIN 0x02
// #define SCL_PIN 0x00
#define SDA_PIN 0x05
#define SCL_PIN 0x04

#define I2C_SLAVE_ADDRESS 0x10
#define I2C_IDLE 0x00
#define I2C_READING_START 0x01
#define I2C_READING_ADDRESS 0x02
#define I2C_READING_BYTES 0x03
#define I2C_WRITING_BYTES 0x04

#endif
