#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#include "twi_module.h"
#else
	#error "Device not supported"
#endif