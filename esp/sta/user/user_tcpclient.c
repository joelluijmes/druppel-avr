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


static bool DISCONNECT_AFTER_SENT;                                      // Close tcp connection after send callback
static state tcp_state;                                                 // TCP connection state

static struct espconn esp_conn;                                         // Holding the tcp connection
static esp_tcp esptcp;

state ICACHE_FLASH_ATTR
tcpclient_get_state()
{
    return tcp_state;
}

void ICACHE_FLASH_ATTR
tcpclient_update_state(state state)
{
    if(tcp_state == state)                                              // Do nothing if state is the same
        return; 

    // TODO: check illegal states
    switch(state)
    {
    case STATE_IDLE:
        break; 
    case STATE_CONNECT: 
        if(tcp_state == STATE_CONNECTED || tcp_state == STATE_BUSY)
            return; 

        os_printf("Making connection\n"); 
        user_tcpclient_init();                                          // Connect to server
        break;
    case STATE_DISCONNECT: 
        espconn_disconnect(&esp_conn);                                  // Disconnect tcp connection
        break;
    case STATE_CONNECTED:
        break;
    case STATE_DISCONNECTED:
        break;
    default:
        break;
    }
    tcp_state = state; 
}

void ICACHE_FLASH_ATTR
tcpclient_recv_cb(void *arg, char *data, unsigned short length)         // Received some data from tcp connection
{
	struct espconn *pespconn = arg;                                     // Pointer to esp connection

	os_printf("TCP recv data length: %d \r\n", length);
	os_printf("%s \n", data);

    if(length == 2 && strcmp("OK", data) == 0)                          // Check if emmer is ready
    {
        // Receive OK so our tcp connection is ready

        tcp_state = STATE_CONNECTED;                                    // Update state to let know tcp connection is ready
    }

}

void ICACHE_FLASH_ATTR
tcpclient_sent_cb(void *arg)
{
    struct espconn *pespconn = arg;                                     // Pointer to esp connection
    os_printf("Data is sent! \r\n");

    tcp_state = STATE_CONNECTED;                                        // Update state to let know tcp connection is ready

    if(DISCONNECT_AFTER_SENT == CLOSE)
        espconn_disconnect(pespconn);                                   // Close tcp connection
}

void ICACHE_FLASH_ATTR
tcpclient_discon_cb(void *arg)
{
	// Memory freed automaticaly
	os_printf("TCP disconnect succeed\n");

    tcp_state = STATE_DISCONNECTED;                                     // Update state to disconnected
}

void ICACHE_FLASH_ATTR
tcpclient_send_data(uint8_t *data, uint8_t length, uint8_t keepAlive)
{
    if(tcp_state != STATE_CONNECTED) {
        os_printf("TCP: no connection, can't send data\n");
        return; 
    }
    DISCONNECT_AFTER_SENT = keepAlive;

    tcp_state = STATE_BUSY;
    espconn_send(&esp_conn, data, length);
}

void ICACHE_FLASH_ATTR
tcpclient_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;
    tcp_state = STATE_CONNECTED; 
    os_printf("TCP connected! \r\n");

    espconn_regist_recvcb(pespconn, tcpclient_recv_cb);
    espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
   	espconn_regist_disconcb(pespconn, tcpclient_discon_cb);
   
    char buffer[] = "AR";
    tcpclient_send_data(buffer, os_strlen(buffer), KEEP_ALIVE);           // TCP send request if tcp server is ready
}

void ICACHE_FLASH_ATTR
user_tcpclient_init()
{
    DISCONNECT_AFTER_SENT = false; 

    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = espconn_port(); 	// Random local port
    esp_conn.proto.tcp->remote_port = SERVER_PORT;

    const char ip[] = SERVER_IP;
	os_memcpy(esp_conn.proto.tcp->remote_ip, ip, 4);

    espconn_regist_connectcb(&esp_conn, tcpclient_connect_cb);          // Register callback function

    sint8 ret = espconn_connect(&esp_conn);                             // Make tcp connection, this is a asynchronous call wich will performed later

    os_printf("ESP is connecting to remote server [%d]!\r\n", ret);
}