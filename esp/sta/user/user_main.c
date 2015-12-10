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
//#include "at.h"


static char hwaddr[6];
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]


static void ICACHE_FLASH_ATTR
scanCB(void *arg, STATUS status) {
    struct bss_info *bssInfo;
    bssInfo = (struct bss_info *)arg;
    // skip the first in the chain … it is invalid
    //bssInfo = STAILQ_NEXT(bssInfo, next);
    while(bssInfo != NULL) {
        os_printf("ssid: %s, %d\n", bssInfo->ssid, bssInfo->rssi);

        bssInfo = STAILQ_NEXT(bssInfo, next);
    }

    //wifi_set_opmode(NULL_MODE);
}

static void ICACHE_FLASH_ATTR 
initDone_cb() {
    //os_printf("Start WiFi Scan\n");
    //wifi_set_opmode(STATION_MODE);
    //wifi_set_opmode_current(STATION_MODE);
    //wifi_station_scan(NULL, scanCB);


    os_printf("Wifi mode: %d,  %d \n", wifi_get_opmode(), (int) wifi_get_phy_mode());

    //struct ip_info info; 
    //wifi_get_ip_info(SOFTAP_IF, &info);
    //os_printf("%d, %d, %d", info.ip.addr, info.netmask.addr, info.gw.addr);

    wifi_station_connect();

    os_printf("autoconnect: %d ", wifi_station_get_auto_connect());

    wifi_station_set_auto_connect(0);


}

void ICACHE_FLASH_ATTR
fpm_wakup_cb_func1(void)
{
    wifi_fpm_close(); 
    wifi_set_opmode(STATION_MODE);
    wifi_station_connect(); 
}

void ICACHE_FLASH_ATTR
enter_sleep_correct(void)
{
    /*
    wifi_station_disconnect();
    wifi_set_opmode(NULL_MODE);// set WiFi mode to null mode.
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);// light sleep

    os_delay_us(2000);
    wifi_fpm_open();// enable force sleep
    wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1);
    //wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1)； // Set wakeup callback
    //wifi_fpm_do_sleep(10*1000);

    int8_t err = wifi_fpm_do_sleep(10000);
    if (err != 0) os_printf("Sleep error: %d\n", err);

    //ets_enter_sleep(); 

    //os_printf("Back!\n");
    // wifi_set_opmode(STATION_MODE);
    // wifi_station_connect();*/ 

    wifi_station_disconnect();
    wifi_set_opmode_current(NULL_MODE);
    os_delay_us(1000);
    os_printf("Sleep!\n");
    os_delay_us(600);
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
    wifi_fpm_open();
    int8_t err = wifi_fpm_do_sleep(100*1000);
    if (err != 0) os_printf("Sleep error: %d\n", err);
    os_printf("Back!\n");
    wifi_set_opmode(STATION_MODE);
    wifi_station_connect();
}



//LOCAL espconn conn1;
//LOCAL esp_tcp tcp1;

void connectCB(void *arg) {
    os_printf("We have connected\n");
}
void errorCB(void *arg, sint8 err) {
    os_printf("We have an error: %d\n", err);
}

static void ICACHE_FLASH_ATTR
recvCB(void *arg, char *pData, unsigned short len) {
    struct espconn *pespconn = (struct espconn *)arg;
    os_printf("Received data!! - length = %d\n", len);

    os_printf("data '%s'", pData);
    if (len == 0 || (pData[0] != '0' && pData[0] != '1')) {
        return;
    }
    //int v = (pData[0] == '1');
    //GPIO_OUTPUT_SET(LED_GPIO, v);
}

static void ICACHE_FLASH_ATTR
at_tcpclient_sent_cb(void *arg) {
    os_printf("sent callback\n");
    struct espconn *pespconn = (struct espconn *)arg;
    //espconn_disconnect(pespconn);
}

static void ICACHE_FLASH_ATTR
at_tcpclient_discon_cb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;
    os_free(pespconn->proto.tcp);

    os_free(pespconn);
    os_printf("TCP disconnect callback\n");
    enter_sleep_correct(); 
}

static void ICACHE_FLASH_ATTR
at_tcpclient_connect_cb(void *arg)
{
    struct espconn *pespconn = (struct espconn *)arg;

    os_printf("TCP connected\r\n");

    espconn_regist_recvcb(pespconn, recvCB);
    espconn_regist_sentcb(pespconn, at_tcpclient_sent_cb);
    espconn_regist_disconcb(pespconn, at_tcpclient_discon_cb);

    char payload[128];
    os_sprintf(payload, MACSTR ",%d,%d\n", MAC2STR(hwaddr));
    //os_printf(payload);
    espconn_sent(pespconn, payload, strlen(payload));


    //uint8_t data[20] = "GET / HTTP/1.1";
    //char data[20] = "GET / HTTP/1.1";
    //os_printf("Sending, '%s' \n", data); 
    ///espconn_sent(pespconn, data, 20);
}

static void ICACHE_FLASH_ATTR
at_tcpclient_recon_cb(void *arg, sint8 err)
{
    os_printf("Error: reconnect callback %d \n", err);
}

static void ICACHE_FLASH_ATTR
sendReading()
{
    struct espconn *pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
    if (pCon == NULL)
    {
        os_printf("CONNECT FAIL\r\n");
        return;
    }
    pCon->type = ESPCONN_TCP;
    pCon->state = ESPCONN_NONE;

    //uint32_t ip = ipaddr_addr("127.0.0.1");

    pCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
    pCon->proto.tcp->local_port = espconn_port();
    pCon->proto.tcp->remote_port = 80;// 1337;

    pCon->proto.tcp->remote_ip[0]=192;                          // Your computer IP
    pCon->proto.tcp->remote_ip[1]=168;                           // Your computer IP
    pCon->proto.tcp->remote_ip[2]=4;                         // Your computer IP
    pCon->proto.tcp->remote_ip[3]=2; 

    //os_memcpy(pCon->proto.tcp->remote_ip, &ip, 4);

    espconn_regist_connectcb(pCon, at_tcpclient_connect_cb);
    espconn_regist_disconcb(pCon, at_tcpclient_discon_cb);
    //espconn_regist_recvcb(pCon, recvCB); 

    espconn_regist_reconcb(pCon, at_tcpclient_recon_cb);

    espconn_connect(pCon);
}

LOCAL void eventCB(System_Event_t *event) {
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

        sendReading();
        break;
    default:
        os_printf("Unexpected event: %d\n", event->event);
        break;
    }
}


void enter_sleep() {
    //wifi_station_disconnect(); and wifi_set_opmode_current(NULL_MODE)

    typedef enum sleep_type sleep_type; 
    enum sleep_type type = MODEM_SLEEP_T;
    type = LIGHT_SLEEP_T; 

    wifi_set_sleep_type(type); 
    ets_enter_sleep();
}


void user_rf_pre_init(void) 
{
    
}
/*
void fpm_wakup_cb_func1(void)
{
    wifi_fpm_close(); // disable force sleep function
    wifi_set_opmode(STATION_MODE); // set station mode
    wifi_station_connect(); // connect to AP
}
void enter_sleep_correct(void)
{
    wifi_station_disconnect();
    wifi_set_opmode(NULL_MODE);// set WiFi mode to null mode.
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);// light sleep
    wifi_fpm_open();// enable force sleep
    wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1)； // Set wakeup callback
    wifi_fpm_do_sleep(10*1000);
}*/ 


void ICACHE_FLASH_ATTR
user_set_station_config(void)
{
    wifi_set_opmode(STATION_MODE);
    // Build our Access Point configuration details
    struct station_config config; 
    wifi_station_get_config(&config);

    strncpy(config.ssid, "DRUPPEL", 32);
    strncpy(config.password, "", 64);
    config.bssid_set = 0; 
    wifi_station_set_config(&config);
}

void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);     // Enable dev stream to uart 0 

    os_printf("\r\nUser init...");
    os_printf("Sleep type: %d", wifi_get_sleep_type()); 

    // ESP8266 softAP set config.
    user_set_station_config();


    os_printf("\n\n\n\n"); 

    system_init_done_cb(initDone_cb);
    wifi_set_event_handler_cb(eventCB);
}