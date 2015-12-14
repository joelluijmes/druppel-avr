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

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "user_sta.h"

static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]

static void ICACHE_FLASH_ATTR 
init_done_cb() {

    wifi_station_connect();

    os_printf("autoconnect: %d ", wifi_station_get_auto_connect());
}

LOCAL void event_cb(System_Event_t *event) {
    switch (event->event) {
    case EVENT_STAMODE_CONNECTED:
        os_printf("Event: EVENT_STAMODE_CONNECTED\n");
        break;
    case EVENT_STAMODE_DISCONNECTED:
        os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
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
}


void user_rf_pre_init(void) 
{
    
}

void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);     // Enable dev stream to uart 0 
    os_printf("\r\nUser init...\n"); 

    // ESP8266 station mode init.
    user_sta_init();

    system_init_done_cb(init_done_cb);
    wifi_set_event_handler_cb(event_cb);

    os_printf("User init done...\n\n");
}