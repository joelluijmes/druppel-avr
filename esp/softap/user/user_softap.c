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
#include "user_softap.h"



void ICACHE_FLASH_ATTR 
user_softap_init(void)
{
	if(wifi_get_opmode() != SOFTAP_MODE) 
	{
		os_printf("Setting SOFTAP_MODE\n");
		wifi_set_opmode(SOFTAP_MODE);
	}

	user_softap_setup_config(); 

    //os_printf("Wifi physical level mode: %d \n", (int) wifi_get_phy_mode());
}

void ICACHE_FLASH_ATTR 
user_softap_setup_config(void)
{
	// Build our Access Point configuration details
	struct softap_config config;
	wifi_softap_get_config(&config); // Get config first.

	os_memset(config.ssid, 0, 32);
	os_memset(config.password, 0, 64);

	os_strcpy(config.ssid, SSID, strlen(SSID));
	os_strcpy(config.password, WIFI_KEY, strlen(WIFI_KEY));
	config.ssid_len = 0;
	config.channel = 11;
    //config.authmode = AUTH_WPA_WPA2_PSK; // Secure auth mode isn't yet working
	config.authmode = AUTH_OPEN;
	config.ssid_hidden = 0;
	config.max_connection = 4;
	//config.beacon_interval = 300; // instable in some situaties
	config.beacon_interval = 100; 
	wifi_softap_set_config_current(&config);
}

void ICACHE_FLASH_ATTR 
user_softap_ipinfo(void) 
{
	struct ip_info info; 
	wifi_get_ip_info(SOFTAP_IF, &info);
	//os_printf("%d, %d, %d \n", info.ip.addr, info.netmask.addr, info.gw.addr); // Getting uint_32 values
	os_printf("IP: " IPSTR " SUBNET: " IPSTR " GW: " IPSTR " \n", IP2STR(&info.ip), IP2STR(&info.netmask), IP2STR(&info.gw));
}