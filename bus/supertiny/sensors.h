#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "../twi/twi.h"
#include "../../util/ds1307/ds1307.h"

#define SENSORS_ADDRESS_FROM 0x00
#define SENSORS_ADDRESS_TO 0x10
#define SENSORS_ADDRESS_SIZE 0x10

#define SENSORS_RECEIVE_BUFFER_SIZE 0x10

