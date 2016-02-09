#pragma once
#include <avr/io.h>

#define PORT_LED PORTB
#define DDR_LED DDRB
#define PIN_LED PINB

#ifndef MASK_LED
	#define MASK_LED (1 << 4)
#endif

#define LED_HIGH() (PORT_LED |= MASK_LED)
#define LED_LOW() (PORT_LED &= ~MASK_LED)
#define LED_TOGGLE() (PIN_LED = MASK_LED)
#define LED_OUTPUT() (DDR_LED |= MASK_LED)