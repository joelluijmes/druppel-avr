#pragma once

#include <avr/io.h>

#if defined (__AVR_ATtiny85__)

	#define DDR_USI DDRB
	#define PORT_USI PORTB
	#define PIN_USI PINB
	#define MASK_SDA PB0
	#define MASK_SCK PB2

#else
	
	#error Device is not supported

#endif
