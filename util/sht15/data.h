#include <stdint.c>

typedef struct data data;
struct data
{
	uint8_t id;
	uint8_t dataId;
	uint8_t dataSize;
	uint8_t* data;
};