/******************************************************************************
 *
 * FileName: user_softap.c
 *
 * Description: Setup softap.
 *
 * Modification history:
 *     2015/12/12, v1.0 create this file.
*******************************************************************************/
 /*
 wi-fi association expired
 802.11 disassociation: previous authentication expired - 
 A client that is attempting to join the service set identifier (SSID) is incorrectly entering the pre-shared key (PSK), 
 or a client left the BSSID without sending a deauthentication frame (could be from a client shutting down or leaving the AP)
 */ 

#include "osapi.h"
#include "user_interface.h"
#include "user_sta.h"


void ICACHE_FLASH_ATTR 
user_sta_init(void)
{
	if(wifi_get_opmode() != STATION_MODE) 
	{
		os_printf("Setting STATION_MODE\n");
		wifi_set_opmode(STATION_MODE);
	}

	wifi_station_set_auto_connect(0);
	user_sta_setup_config(); 

    //os_printf("Wifi physical level mode: %d \n", (int) wifi_get_phy_mode());
}

void ICACHE_FLASH_ATTR
user_sta_setup_config(void)
{
	wifi_set_opmode(STATION_MODE);
	// Build our Access Point configuration details
	struct station_config config; 
	wifi_station_get_config(&config);

	os_memset(config.ssid, 0, 32);
	os_memset(config.password, 0, 64);
	char SSI[32] = "DRUPPEL";
	os_memcpy(config.ssid, SSI, strlen(SSI));
	//os_memcpy(config.password, WIFI_KEY, strlen(WIFI_KEY));
	//strncpy

	config.bssid_set = 0; 
	wifi_station_set_config(&config);
}

void ICACHE_FLASH_ATTR 
user_softap_ipinfo(void) 
{
	struct ip_info info; 
	wifi_get_ip_info(SOFTAP_IF, &info);
	//os_printf("%d, %d, %d \n", info.ip.addr, info.netmask.addr, info.gw.addr); // Getting uint_32 values
	os_printf("IP: " IPSTR " SUBNET: " IPSTR " GW: " IPSTR " \n", IP2STR(&info.ip), IP2STR(&info.netmask), IP2STR(&info.gw));
}