/******************************************************************************
 *
 * FileName: user_status.c
 *
 * Description: Keep the wifi status.
 *
 * Modification history:
 *     2016/1/12, v1.0 create this file.
*******************************************************************************/

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
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

    // case I2C_READING_ADDRESS:
    //     ETS_GPIO_INTR_ATTACH(i2c_slave_reading_address, SCL_PIN);      // GPIO2 interrupt handler
    //     gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
    //     i2c_bit_number = 7; 
    //     i2c_byte_number = -1; 
    //     break;
    // case I2C_READING_BYTES:
    //     ETS_GPIO_INTR_ATTACH(i2c_slave_reading_address, SCL_PIN);      // GPIO2 interrupt handler
    //     gpio_pin_intr_state_set(SCL_PIN, 1);                            // Interrupt on positive edge
    //     i2c_bit_number = 7; 
    //     break;
    // case I2C_WRITING_BYTES:
    //     ETS_GPIO_INTR_ATTACH(i2c_slave_writing_address, SCL_PIN);      // GPIO2 interrupt handler
    //     gpio_pin_intr_state_set(SCL_PIN, 2);                            // Interrupt on negative edge
    //     i2c_bit_number = 8; // One for disable ack 
    //     break;
    default:
        break;
    }
    wifi_state = state; 
}
