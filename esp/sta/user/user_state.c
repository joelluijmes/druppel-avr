/******************************************************************************
 *
 * FileName: user_state.c
 *
 * Description: Keep the wifi status.
 *
 * Modification history:
 *     2016/1/12, v1.0 create this file.
*******************************************************************************/

#include "user_interface.h"

#include "user_state.h"
#include "i2c_slave.h"

static state wifi_state;

state ICACHE_FLASH_ATTR
get_state()
{
    return wifi_state; 
}

void ICACHE_FLASH_ATTR
update_state(state state)
{
    if(wifi_state == state)                                        // Do nothing if state is the same
        return; 

    switch(state) 
    {
    case STATE_IDLE: 
        break; 
    case STATE_CONNECT:
        wifi_set_sleep_type(NONE_SLEEP_T);                          // Setting device to don't sleep when connected (pm open 0 0)
        wifi_station_dhcpc_stop();                                  // Stop DHCP client to set static ip
        user_sta_setup_static_ip();                                 // Setup static ip
        wifi_station_connect();                                     // Connect to specific AP
        break;
    case STATE_DISCONNECT: 
        wifi_station_disconnect();                                  // Disconnect from AP
        break; 
    case STATE_CONNECTED:
        i2c_slave_init(); 
        break;
    case STATE_DISCONNECTED: 
        i2c_slave_stop(); 
        break;
    default:
        break;
    }
    wifi_state = state; 
}
