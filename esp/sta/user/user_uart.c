/******************************************************************************
 *
 * FileName: user_softap.c
 *
 * Description: Setup softap.
 *
 * Modification history:
 *     2015/12/12, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "user_interface.h"
#include "user_uart.h"
 
#include "driver/uart.h"

 void uart1_write_char(char c);


void ICACHE_FLASH_ATTR 
user_uart_init(void)
{




    os_install_putc1((void *)uart1_write_char);


	uart0_sendStr("UART 0  ja\r\n");


	uart1_sendStr_no_wait("UART 1 ja\r\n");
}
