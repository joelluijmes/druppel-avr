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
//#include "at.h"


static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]



static void ICACHE_FLASH_ATTR initDone_cb() {

    os_printf("Wifi mode: %d,  %d \n", wifi_get_opmode(), (int) wifi_get_phy_mode());

    struct ip_info info; 
    wifi_get_ip_info(SOFTAP_IF, &info);
    os_printf("%d, %d, %d", info.ip.addr, info.netmask.addr, info.gw.addr);
    //info.ip.addr = 192.168.4.1;
    // IP4_ADDR(info.ip, 192,168,4,1);
    // //info.netmask.addr = 255.255.255.0;
    // IP4_ADDR(info.netmask, 255, 255, 255, 0);
    // //info.gw.addr = 192.168.4.1;
    // IP4_ADDR(info.gw, 192, 168, 4, 1);

    // IPAddress ip(192,168,0,128);  //Node static IP
    // IPAddress gateway(192,168,0,1);
    // IPAddress subnet(255,255,255,0);

    //info.ip.addr = ipaddr_addr("192.168.4.1"); //static_cast<uint32_t>(ip(192,168,4,1));
    //info.netmask.addr = ipaddr_addr("255.255.255.0"); //static_cast<uint32_t>(subnet(255,255,255,0));
    //info.gw.addr = ipaddr_addr("192.168.4.1"); //static_cast<uint32_t>(gateway(192,168,4,1));

    //IP4_ADDR((ip_addr_t *)info.gw.addr, 192,168,4,3);

    wifi_set_ip_info(SOFTAP_IF, &info);

    if(wifi_station_dhcpc_stop()) {
        os_printf("stopped\n");
    }

    if(wifi_station_dhcpc_status() == DHCP_STARTED) {
        os_printf("DHCP STARTED\n");
    }

    //wifi_set_opmode(SOFTAP_MODE);

    if(wifi_get_opmode() != SOFTAP_MODE) {
        os_printf("Setting up wifi AP mode");
        wifi_set_opmode(SOFTAP_MODE); 

        // Build our Access Point configuration details
        struct softap_config config; 
        os_strcpy(config.ssid, "DRUPPEL", 7);
        os_strcpy(config.password, "druppelproject", 14);
        config.ssid_len = 0;
        config.channel = 6;
        config.authmode = AUTH_WPA2_PSK;
        config.ssid_hidden = 0;
        config.max_connection = 4;
        config.beacon_interval = 300; 
        wifi_softap_set_config_current(&config);
    } 

}

void user_rf_pre_init(void) 
{
    
}


void ICACHE_FLASH_ATTR
user_set_softap_config(void)
{
    // Build our Access Point configuration details
    struct softap_config config;

    wifi_softap_get_config(&config);    // get config

    os_memset(config.ssid, 0, 32);
    os_memset(config.password, 0, 64);
    os_strcpy(config.ssid, "DRUPPEL", 7);
    os_strcpy(config.password, "druppelproject", 14);
    config.ssid_len = 0;
    //config.authmode = AUTH_WPA2_PSK;
    config.authmode = AUTH_OPEN;
    config.ssid_hidden = 0;
    config.max_connection = 4;
    config.beacon_interval = 100; 
    wifi_softap_set_config_current(&config);
    //wifi_softap_set_config(&config);
}




void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);     // Enable dev stream to uart 0 

    os_printf("\r\nUser init..."); 

    // ESP8266 softAP set config.
    user_set_softap_config();


    //at_customLinkMax = 5;
    //at_init();
    //at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_custom_cmd[0]));


    os_printf("\n\n\n\n"); 

    system_init_done_cb(initDone_cb);
}

