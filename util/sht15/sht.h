#pragma once

#include <util/delay.h>
#include "../pin.h"

typedef struct sht sht;
struct sht
{
	pin pinSCK;
	pin pinDATA;
};

double sht_readTemperature(const sht* sht);
double sht_readHumidity(const sht* sht);
double sht_compensateHumidity(const sht* sht, double temperature);