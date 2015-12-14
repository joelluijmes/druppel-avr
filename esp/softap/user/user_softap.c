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
}

void ICACHE_FLASH_ATTR 
user_softap_setup_config(void)
{
	// Build our Access Point configuration details
	struct softap_config config; 
	os_strcpy(config.ssid, SSID, strlen(SSID));
	os_strcpy(config.password, WIFI_KEY, strlen(WIFI_KEY));
	config.ssid_len = 0;
	config.channel = 6;
    //config.authmode = AUTH_WPA2_PSK;
	config.authmode = AUTH_OPEN;
	config.ssid_hidden = 0;
	config.max_connection = 4;
	//config.beacon_interval = 300; 
	config.beacon_interval = 100; 
	wifi_softap_set_config_current(&config);
}