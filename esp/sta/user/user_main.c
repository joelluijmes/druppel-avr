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
#include "gpio.h"
//#include "user_uart.h"
#include "user_sta.h"
#include "user_tcpclient.h"
#include "user_i2c.h"
#include "user_interface.h"

#include "user_config.h"

#include "i2c_slave.h"

//#include "espconn.h"
//#include "mem.h"
//#include "driver/uart.h"

#define WIFI_BUSY 0x01
#define WIFI_READY 0x02

uint8_t wifi_status;

static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]


static void ICACHE_FLASH_ATTR 
init_done_cb() {

    i2c_slave_init();
    //user_i2c_init();


    // // Wifi connect to ap
    // os_delay_us(3000*1000);
    // wifi_station_disconnect();
    // wifi_station_connect();

    //os_printf("autoconnect: %d ", wifi_station_get_auto_connect());
}

LOCAL void event_cb(System_Event_t *event) {
    switch (event->event) {
    case EVENT_STAMODE_CONNECTED:
        os_printf("Event: EVENT_STAMODE_CONNECTED\n");
        break;
    case EVENT_STAMODE_DISCONNECTED:
        os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
        //os_printf("IP: %d.%d.%d.%d\n", IP2STR(&event->event_info.got_ip.ip));
        os_printf("Reason: %d\n", IP2STR(&event->event_info.disconnected.reason));

        break;
    case EVENT_STAMODE_AUTHMODE_CHANGE:
        os_printf("Event: EVENT_STAMODE_AUTHMODE_CHANGE\n");
    case EVENT_STAMODE_GOT_IP:
        wifi_status = WIFI_READY;               // For i2c slave to respond
        os_printf("Event: EVENT_STAMODE_GOT_IP\n");
        user_tcpclient_init(); 
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
    os_printf("\r\nUser init...\n"); 

    // ESP8266 station mode init.
    user_sta_init();

    // wifi_status = WIFI_BUSY; 
    wifi_status = WIFI_READY; 


    //user_uart_init(); 

    system_init_done_cb(init_done_cb);
    wifi_set_event_handler_cb(event_cb);
    os_printf("User init done...\n\n");
}