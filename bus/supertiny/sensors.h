#pragma once

#include <avr/io.h>
#include <util/delay.h>

#include "../../util/wdt.h"
#include "../../util/twi/twi.h"

uint8_t sensor_fill(uint8_t* data, uint8_t len);