#ifndef __USER_GLOBAL_DEFINITIONS_H__
#define __USER_GLOBAL_DEFINITIONS_H__

extern volatile uint32_t PIN_OUT;
extern volatile uint32_t PIN_OUT_SET;
extern volatile uint32_t PIN_OUT_CLEAR;

extern volatile uint32_t PIN_DIR;
extern volatile uint32_t PIN_DIR_OUTPUT;
extern volatile uint32_t PIN_DIR_INPUT;

extern volatile uint32_t PIN_IN;

extern volatile uint32_t PIN_0;
extern volatile uint32_t PIN_2;

extern volatile uint8_t wifi_status; 

#define WIFI_BUSY 0x01
#define WIFI_READY 0x02

#endif
