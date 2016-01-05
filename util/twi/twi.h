#pragma once

// Status codes of what this module will return
typedef uint8_t TWRESULT;
#define TWST_OK	1
#define TWST_START_FAILED 2

#define TWST_MASTER_NACK 3

#define TWST_SL_TRANSMITTING 4
#define TWST_SL_RECEIVING 5

#define TWST_STOP_FAILED 6