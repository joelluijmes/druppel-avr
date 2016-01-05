#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#if defined (__AVR_ATtiny85__)
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PORTB0
#define PORT_USI_SCL        PORTB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2
#else
#error Device is not supported
#endif

typedef uint8_t USIRESULT;
#define USI_NOT_US 0			// TEMP
#define USI_OK 1
#define USI_SLAVE_TRANSMIT 2
#define USI_SLAVE_RECEIVE 3
#define TWST_MR_ACK_EXPECTED 4

USIRESULT usi_init_slave(uint8_t slave_addr);
uint8_t usi_init_mt(uint8_t slave_addr);
uint8_t usi_write_master(uint8_t data);
uint8_t usi_write_slave(uint8_t data);
uint8_t usi_read_slave();
void usi_stop();
