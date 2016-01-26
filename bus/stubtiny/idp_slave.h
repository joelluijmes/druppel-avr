#pragma once

#include "../idp.h"
#include "../../util/twi/twi.h"

typedef uint8_t (*measure_t)(uint8_t* data, uint8_t len);

typedef enum state state;
enum state
{
	STATE_IDLE,
	STATE_MEASURING,
	STATE_SENDING,
	STATE_COMPLETED, 
	STATE_FAILED
};

void idp_init(uint8_t slave_addr, uint8_t num_data, measure_t measure);
state idp_process(uint8_t* data, uint8_t data_len);