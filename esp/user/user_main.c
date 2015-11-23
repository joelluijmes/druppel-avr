#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
//#include "at.h"
#include "gpio.h"
#include "os_type.h"

// disassembly
// xtensa-lx106-elf\bin\xtensa-lx106-elf-objdump.exe -d projects/at/build/esp8266_at.out > projects/at/build/disassemble.txt

extern volatile uint32_t PIN_OUT;
extern volatile uint32_t PIN_OUT_SET;
extern volatile uint32_t PIN_OUT_CLEAR;

extern volatile uint32_t PIN_DIR;
extern volatile uint32_t PIN_DIR_OUTPUT;
extern volatile uint32_t PIN_DIR_INPUT;

extern volatile uint32_t PIN_IN;

extern volatile uint32_t PIN_0;
extern volatile uint32_t PIN_2;


void ICACHE_FLASH_ATTR user_init(void)
{
	//uart_init(BIT_RATE_115200, BIT_RATE_115200);
	//at_wifiMode = wifi_get_opmode();
	uart_div_modify(0, UART_CLK_FREQ / 115200);  // In Arduino this is Serial.begin(115200);

	os_printf("\n\n\n\nBeginning...\r\n");
	os_printf("ESP8266 ready!\r\n");
	//uart0_sendStr("\r\nready ja\r\n");


	//at_init();

	GPIO_OUTPUT_SET(5, 0);

	GPIO_OUTPUT_SET(5, 1);

}