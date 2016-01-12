/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/
/*
    • 0 – Default restart – Normal start-up on power up
    • 1 – Watch dog timer – Hardware watchdog reset
    • 2 – Exception – An exception was detected
    • 3 – Software watch dog timer – Software watchdog reset
    • 4 – Soft restart
    • 5 – Deep sleep wake up
*/
    // DS1307 rtc

/*
2. uart debug info. output:
Usually,for iot project , UART0 can output the debug information. But in AT mode , UART0 communicates with the PC or MCU, so the debug info. should be output via UART1.
(1).SET UART1 PIN FUNC:
PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
(2).change output port for os_printf:
os_install_putc1((void *)uart1_write_char);
*/ 

#include "ets_sys.h"
#include "osapi.h"
#include "user_sta.h"
#include "user_tcpclient.h"
#include "user_interface.h"
#include "user_global_definitions.h"

#include "user_config.h"

#include "user_state.h"

static void ICACHE_FLASH_ATTR 
init_done_cb() {
    // Wifi connect to ap
    os_delay_us(2000*1000); 

    update_state(STATE_CONNECT);                               // Connecting to ap and set sleep mode to no sleep
}

LOCAL void event_cb(System_Event_t *event) {
    switch (event->event) {
    case EVENT_STAMODE_CONNECTED:
        os_printf("Event: EVENT_STAMODE_CONNECTED\n");
        break;
    case EVENT_STAMODE_DISCONNECTED:
        os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
        os_printf("Reason: %d\n", event->event_info.disconnected.reason);
        break;
    case EVENT_STAMODE_AUTHMODE_CHANGE:
        os_printf("Event: EVENT_STAMODE_AUTHMODE_CHANGE\n");
    case EVENT_STAMODE_GOT_IP:
        os_printf("Event: EVENT_STAMODE_GOT_IP\n");
        break;
    default:
        os_printf("Unexpected event: %d\n", event->event);
        break;
    }

    if(event->event == EVENT_STAMODE_GOT_IP)
        update_state(STATE_CONNECTED);                             // Enable i2c interrupts
    else 
        update_state(STATE_DISCONNECTED);                         // Disable i2c interrupts
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void) 
{
    system_phy_set_powerup_option(3);                               //Full RF calibration... take 200ms
}

void ICACHE_FLASH_ATTR
user_init(void)
{
    //uart_init(BIT_RATE_115200, BIT_RATE_115200);
    uart_div_modify(0, UART_CLK_FREQ / 115200);                     // Enable dev stream to uart 0 
    os_printf("\r\nUser init...\n"); 

    user_sta_init();                                                // ESP8266 station mode init.

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);            // SET GPIO function, not uart...
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);            // SET GPIO function, not uart...


    system_init_done_cb(init_done_cb);
    wifi_set_event_handler_cb(event_cb);
    os_printf("User init done...\n\n");
}