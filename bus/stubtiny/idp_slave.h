#pragma once

#include <string.h>

#include "../idp.h"
#include "../../util/twi/twi.h"
#include "..\..\util\wdt.h"

typedef uint8_t (*measure_t)(uint8_t* data, uint8_t len);

#if I2C_ADDRLEN > 1
void idp_init(uint8_t* addresses, measure_t* measure);
#else
void idp_init(uint8_t slave_addr, measure_t measure);
#endif
void idp_process();