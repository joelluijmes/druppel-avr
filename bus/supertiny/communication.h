#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../../util/twi/twi.h"

uint8_t communication_send(uint8_t* data, uint8_t len);
uint8_t communication_available();