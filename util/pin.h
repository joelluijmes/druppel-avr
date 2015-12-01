/*
 *	Module to easy access specific pins on a port 
 *	Author: Joël Luijmes
 *	
 *	Example: 
 *	pin led = { &PORTB, 1 << PB5 };
 *	pin_high(led);
 *
 *	Note: for inputs use the input port PINx not PORTx
 *
 */

#pragma once

#include <inttypes.h>

typedef struct pin pin;
struct pin
{
	volatile uint8_t* ddr;
	volatile uint8_t* out;
	volatile uint8_t* in;
	uint8_t mask;
};


static inline void pin_input(const pin* pin)
{
	*pin->ddr &= ~pin->mask;
}

static inline void pin_output(const pin* pin)
{
	*pin->ddr |= pin->mask;
}

static inline void pin_high(const pin* pin)
{
	*pin->out |= pin->mask;
}

static inline void pin_low(const pin* pin)
{
	*pin->out &= ~pin->mask;
}

static inline uint8_t pin_read(const pin* pin)
{
	return !!(*pin->in & pin->mask);
}

static inline void pin_write(const pin* pin, uint8_t value)
{
	if (value == 0)
		pin_low(pin);
	else
		pin_high(pin);
}

static inline void pin_direcion(const pin* pin, uint8_t value)
{
	if (value == 0)
		pin_input(pin);
	else
		pin_output(pin);
}
