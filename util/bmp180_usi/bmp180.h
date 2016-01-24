#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "../twi/twi.h"

double read_temperature();
double read_presure();