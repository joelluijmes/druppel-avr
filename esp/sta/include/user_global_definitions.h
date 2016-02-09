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

#define DEBUG_LEVEL0
//#define DEBUG_LEVEL1

#ifdef DEBUG_LEVEL0
    #define DEBUG_0( func__ ) ( func__ )
#else
    #define DEBUG_0( func__ )
#endif
#ifdef DEBUG_LEVEL1
    #define DEBUG_1( func__ ) ( func__ )
#else
    #define DEBUG_1( func__ )
#endif
#ifdef DEBUG_LEVEL2
    #define DEBUG_2( func__ ) ( func__ )
#else
    #define DEBUG_2( func__ )
#endif

#endif
