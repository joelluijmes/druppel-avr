/******************************************************************************
 *
 * FileName: user_tcpclient.c
 *
 * Description: Setting up tcp connection and send data... 
 *
 * Modification history:
 *     2015/12/14, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
//#include "mem.h
#include "user_tcpclient.h"
#include "user_state.h"

#include "user_global_definitions.h"

#define WD_MAX_FAILED 4

static struct espconn esp_conn;                                         // Holding the tcp connection
static esp_tcp esptcp;

static bool DISCONNECT_AFTER_SENT;                                      // Close tcp connection after send callback
static state tcp_state;                                                 // TCP connection state
static volatile os_timer_t wdtimer;
static uint8_t esp_conn_wd_state;
static uint8_t wd_conn_state; 
static uint8_t wd_failed; 


static void tcpclient_recv_cb(void *arg, char *data, unsigned short length);
static void tcpclient_sent_cb(void *arg);
static void tcpclient_discon_cb(void *arg);
static void tcpclient_connect_cb(void *arg);
static void tcpclient_wd_timer(void);

state
tcpclient_get_state()
{
    return tcp_state;
}

void ICACHE_FLASH_ATTR
tcpclient_update_state(state state)
{
    switch(state)
    {
    case STATE_IDLE:
        break; 
    case STATE_CONNECT:
        if(tcp_state == STATE_CONNECTED
            || tcp_state == STATE_BUSY 
            || tcp_state == STATE_TEST_CONN
            || (tcp_state == STATE_CONNECT && esp_conn.state != ESPCONN_CLOSE)
        ) {
            return;
        }

        DEBUG_1(os_printf("Making connection state: %d \n", tcp_state));
        user_tcpclient_init();                                          // Connect to server
        break;
    case STATE_DISCONNECT: 
        os_printf("TCP: soft wd disconnecting\n");
        espconn_disconnect(&esp_conn);                                  // Disconnect tcp connection
        break;
    case STATE_CONNECTED:
        DEBUG_1(os_printf("TCP: Connected to EMMER\n"));
        break;
    case STATE_DISCONNECTED:
        break;
    case STATE_BUSY:
        break; 
    case STATE_TEST_CONN:
        break; 
    default:
        return;                                                         // Do not update the state because it's not allowed
        break;
    }
    tcp_state = state; 
}

void ICACHE_FLASH_ATTR
user_tcpclient_init()
{
    tcpclient_wd_timer(); 

    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = espconn_port();                    // Random local port
    esp_conn.proto.tcp->remote_port = SERVER_PORT;

    const char ip[] = SERVER_IP;
    os_memcpy(esp_conn.proto.tcp->remote_ip, ip, 4);

    espconn_regist_connectcb(&esp_conn, tcpclient_connect_cb);          // Register callback function

    sint8 ret = espconn_connect(&esp_conn);                             // Make tcp connection, this is a asynchronous call wich will performed later

    DEBUG_2(os_printf("ESP is connecting to remote server [%d]!\r\n", ret));
}

void ICACHE_FLASH_ATTR
tcpclient_send_data(uint8_t *data, uint8_t length, uint8_t keepAlive)
{
    if(tcp_state != STATE_CONNECTED && tcp_state != STATE_TEST_CONN) {
        os_printf("TCP: no connection, can't send data [%d] \n", tcp_state);
        return; 
    }
    DISCONNECT_AFTER_SENT = keepAlive;

    tcp_state = STATE_BUSY;
    espconn_send(&esp_conn, data, length);
}

static void ICACHE_FLASH_ATTR
tcpclient_recv_cb(void *arg, char *data, unsigned short length)         // Received some data from tcp connection
{
	//struct espconn *pespconn = arg;                                     // Pointer to esp connection

    if(length == 2 && strcmp("OK", data) == 0)                          // Check if emmer is ready
    {
        // Receive OK so our tcp connection is ready

        tcpclient_update_state(STATE_CONNECTED);                        // Update state to let know tcp connection is ready                                  
    } else {
        os_printf("TCP recv data length: %d \r\n", length);
        os_printf("%s \n", data);
    }
}

static void ICACHE_FLASH_ATTR
tcpclient_sent_cb(void *arg)
{
    struct espconn *pespconn = arg;                                     // Pointer to esp connection
    if(tcp_state != STATE_TEST_CONN)
        DEBUG_1(os_printf("Data is sent! \r\n"));

    tcp_state = STATE_CONNECTED;                                        // Update state to let know tcp connection is ready
    esp_conn_wd_state = ESPCONN_NONE;                                   // Update for soft wd timer...

    if(DISCONNECT_AFTER_SENT == CLOSE) {
        os_printf("DISCONNECT_AFTER_SENT == CLOSE \n"); 
        espconn_disconnect(pespconn);                                   // Close tcp connection
    }
}

static void ICACHE_FLASH_ATTR
tcpclient_discon_cb(void *arg)
{
	DEBUG_1(os_printf("TCP: disconnect succeed [%d]\n", DISCONNECT_AFTER_SENT));

    tcp_state = STATE_DISCONNECTED;                                     // Update state to disconnected
}

static void ICACHE_FLASH_ATTR
tcpclient_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;
    tcp_state = STATE_TEST_CONN; 
    DEBUG_1(os_printf("TCP: connected!\r\n"));

    espconn_regist_recvcb(pespconn, tcpclient_recv_cb);
    espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
   	espconn_regist_disconcb(pespconn, tcpclient_discon_cb);
   
    char buffer[] = "AR";
    tcpclient_send_data(buffer, os_strlen(buffer), KEEP_ALIVE);           // TCP send request if tcp server is ready
}

static void ICACHE_FLASH_ATTR 
tcpclient_check_state(void)
{
    if(tcp_state == STATE_BUSY && esp_conn_wd_state == ESPCONN_WRITE && esp_conn.state == ESPCONN_WRITE) 
    {
        wd_failed++; 
        if(wd_failed >= WD_MAX_FAILED) {
            DEBUG_0(os_printf("TCP: check_state, disconnect\n"));
            tcpclient_update_state(STATE_DISCONNECT); 
            //os_printf("tcp state: %d %d %d \n", esp_conn.state, ESPCONN_CLOSE, tcpclient_get_state());
        }
    } else {
        wd_failed = 0; 
    }

    esp_conn_wd_state = esp_conn.state;
    wd_conn_state = esp_conn.state; 
}

static void ICACHE_FLASH_ATTR 
tcpclient_wd_timer(void)
{
    wd_failed = 0;                                                      // Reset failed count
    //Disarm timer
    os_timer_disarm(&wdtimer);

    //Setup timer
    os_timer_setfn(&wdtimer, (os_timer_func_t *)tcpclient_check_state, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //500 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&wdtimer, 200, 1);
}
