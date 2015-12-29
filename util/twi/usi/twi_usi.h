#pragma once

#include <avr/io.h>
#include <util/delay.h>

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

#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

#define USI_TWI_NO_DATA             0x00  // Transmission buffer is empty
#define USI_TWI_DATA_OUT_OF_BOUND   0x01  // Transmission buffer is outside SRAM space
#define USI_TWI_UE_START_CON        0x02  // Unexpected Start Condition
#define USI_TWI_UE_STOP_CON         0x03  // Unexpected Stop Condition
#define USI_TWI_UE_DATA_COL         0x04  // Unexpected Data Collision (arbitration)
#define USI_TWI_NO_ACK_ON_DATA      0x05  // The slave did not acknowledge  all data
#define USI_TWI_NO_ACK_ON_ADDRESS   0x06  // The slave did not acknowledge  the address
#define USI_TWI_MISSING_START_CON   0x07  // Generated Start Condition not detected on bus
#define USI_TWI_MISSING_STOP_CON    0x08  // Generated Stop Condition not detected on bus

#define TWI_FAST_MODE

#define SYS_CLK   4000.0  // [kHz]
#ifdef TWI_FAST_MODE               // TWI FAST mode timing limits. SCL = 100-400kHz
  #define T2_TWI    ((SYS_CLK *1300) /1000000) +1 // >1,3us
  #define T4_TWI    ((SYS_CLK * 600) /1000000) +1 // >0,6us  
#else                              // TWI STANDARD mode timing limits. SCL <= 100kHz
  #define T2_TWI    ((SYS_CLK *4700) /1000000) +1 // >4,7us
  #define T4_TWI    ((SYS_CLK *4000) /1000000) +1 // >4,0us
#endif

void usi_write(uint8_t data);
uint8_t usi_peek();
uint8_t usi_read();
void usi_init_master();
uint8_t usi_init_mt(uint8_t slave_addr);
void usi_stop();