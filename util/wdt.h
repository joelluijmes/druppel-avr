#pragma once

#include <avr/wdt.h>

#ifdef WDT_ACTIVE
	#define _wdt_enable(value) wdt_enable(value)
	#define _wdt_reset() wdt_reset()
	#define _wdt_disable() do { MCUSR = 0; wdt_disable(); } while (0)
#else
	#define _wdt_enable(value)
	#define _wdt_reset() 
	#define _wdt_disable() 
#endif