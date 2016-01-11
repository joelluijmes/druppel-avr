/******************************************************************************
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/11/23, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
//#include "user_softap.h"
#include "user_sta.h"
#include "user_tcpserver.h"
#include "user_interface.h"

#include "driver/uart.h"

static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]


void ICACHE_FLASH_ATTR init_done_cb() {
    // Printing IP info
    //user_softap_ipinfo(); 


    wifi_station_dhcpc_stop(); 
    user_sta_setup_static_ip();
    wifi_station_connect();

    //os_printf("autoconnect: %d ", wifi_station_get_auto_connect());

    //os_delay_us(2000*1000);
}

LOCAL void event_cb(System_Event_t *event) {
    switch (event->event) {
    case EVENT_STAMODE_CONNECTED:
        os_printf("Event: EVENT_STAMODE_CONNECTED\n");
        break;
    case EVENT_STAMODE_DISCONNECTED:
        os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
        os_printf("Reason: %d\n", IP2STR(&event->event_info.disconnected.reason));
        break;
    case EVENT_STAMODE_AUTHMODE_CHANGE:
        os_printf("Event: EVENT_STAMODE_AUTHMODE_CHANGE\n");
    case EVENT_STAMODE_GOT_IP:
        os_printf("Event: EVENT_STAMODE_GOT_IP\n");

        // Starts listing to specific port
        user_tcpserver_init(80);
        break;
    default:
        os_printf("Unexpected event: %d\n", event->event);
        break;
    }
}

void user_rf_pre_init(void) 
{
    //Full RF calibration... take 200ms
    system_phy_set_powerup_option(3);
}

void user_init(void)
{
    //uart_init(BIT_RATE_115200, BIT_RATE_115200);
    uart_div_modify(0, UART_CLK_FREQ / 115200);     // Enable dev stream to uart 0 
    os_printf("\r\nUser SOFTAP init...\n"); 


    // ESP8266 softAP set config.
    //user_softap_init(); 
    user_sta_init(); 

    uart0_sendStr("\n"); 

    system_init_done_cb(init_done_cb);
    wifi_set_event_handler_cb(event_cb);

    os_printf("User init done...\n\n"); 
}

