#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../../util/twi/twi.h"

uint8_t sensor_fill(uint32_t time, uint8_t* data, uint8_t len);