#include "twi.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#include "avr/twi_mega.h"
#elif defined (__AVR_ATtiny85__)
	#include "usi/twi_usi.h"
#else
	#error "Device not supported"
#endif