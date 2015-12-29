#ifndef __BMP180_H__
#define __BMP180_H__

#define BMP180_CTRL_ID 0x77

struct CALIBRATION {
	uint8_t chipid; 	// 0x55
	int16_t AC1;		// 7448
	int16_t AC2;		// -1063
	int16_t AC3;		// -14398
	uint16_t AC4;		// 34345
	uint16_t AC5;		// 24634
	uint16_t AC6;		// 21015
	int16_t B1;			// 6515
	int16_t B2;			// 37
	int16_t MB;			// -32768
	int16_t MC;			// -11786
	int16_t MD;			// 3043
};
typedef struct CALIBRATION CALIBRATION;

void read_temperature();

#endif
