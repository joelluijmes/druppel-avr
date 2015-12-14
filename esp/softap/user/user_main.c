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
#include "user_softap.h"
#include "user_interface.h"

static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]


static void ICACHE_FLASH_ATTR init_done_cb() {

    os_printf("Wifi mode: %d,  %d \n", wifi_get_opmode(), (int) wifi_get_phy_mode());

    struct ip_info info; 
    wifi_get_ip_info(SOFTAP_IF, &info);
    os_printf("%d, %d, %d", info.ip.addr, info.netmask.addr, info.gw.addr);
}

void user_rf_pre_init(void) 
{

}

void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);     // Enable dev stream to uart 0 

    os_printf("\r\nUser init...\n"); 

    // ESP8266 softAP set config.
    user_softap_init(); 


    system_init_done_cb(init_done_cb);

    os_printf("User init done...\n\n"); 
}

