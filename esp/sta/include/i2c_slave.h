#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

void i2c_slave_init(void);
void i2c_slave_stop(void);
void i2c_update_status(uint8_t status);

#define SDA_PIN 0x02
#define SCL_PIN 0x00
// #define SDA_PIN 0x05
// #define SCL_PIN 0x04

#define I2C_SLAVE_ADDRESS 0x10

#endif
