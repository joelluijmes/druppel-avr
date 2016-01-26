#pragma once

#include <avr/wdt.h>

#ifdef WDT_ACTIVE
	#define _wdt_enable(value) wdt_enable(value)
	#define _wdt_reset() wdt_reset()
	#define _wdt_disable() wdt_disable()
#else
	#define _wdt_enable(value)
	#define _wdt_reset() 
	#define _wdt_disable() 
#endif