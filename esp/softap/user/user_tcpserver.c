/******************************************************************************
 *
 * FileName: user_tcpserver.c
 *
 * Description: Listen to tcp connections and print this.
 *
 * Modification history:
 *     2015/12/12, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "user_interface.h"
#include "user_tcpserver.h"
#include "espconn.h"


LOCAL struct espconn esp_conn;
LOCAL esp_tcp esptcp;

void ICACHE_FLASH_ATTR
tcpserver_recv_cb(void *arg, char *data, unsigned short length)
{
	struct espconn *pespconn = arg;
	// Received some data from tcp connection 
	os_printf("TCP recv data length: %d \r\n", length);
	os_printf("%s \n", data);
	// char pusrdata[] = "Please test this";
	// espconn_sent(pespconn, pusrdata, strlen(pusrdata));
}

void ICACHE_FLASH_ATTR
tcpserver_recon_cb(void *arg, sint8 err)
{
	//error occured , tcp connection broke. 
	os_printf("reconnect callback, error code %d !!! \r\n",err);
}

void ICACHE_FLASH_ATTR
tcpserver_discon_cb(void *arg)
{
	// Memory freed automaticaly
	os_printf("TCP disconnect succeed\n");
}

void ICACHE_FLASH_ATTR
tcpserver_listen(void *arg)
{
    struct espconn *pesp_conn = arg;
    os_printf("TCP listen\r\n");

    espconn_regist_recvcb(pesp_conn, tcpserver_recv_cb);
    espconn_regist_reconcb(pesp_conn, tcpserver_recon_cb);
    espconn_regist_disconcb(pesp_conn, tcpserver_discon_cb);
    
    //espconn_regist_sentcb(pesp_conn, tcp_server_sent_cb);
    //tcp_server_multi_send();
}

void ICACHE_FLASH_ATTR
user_tcpserver_init(uint32 port)
{
    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = port;
    espconn_regist_connectcb(&esp_conn, tcpserver_listen);

    sint8 ret = espconn_accept(&esp_conn);
    
    os_printf("ESP listen to incoming tcp connections [%d]!\r\n", ret);
}
