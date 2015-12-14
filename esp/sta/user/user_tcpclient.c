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
#include "user_tcpclient.h"
#include "espconn.h"
#include "mem.h"


LOCAL struct espconn esp_conn;
LOCAL esp_tcp esptcp;

ip_addr_t tcp_server_ip;

void ICACHE_FLASH_ATTR
tcpclient_recv_cb(void *arg, char *data, unsigned short length)
{
	struct espconn *pespconn = arg;
	//received some data from tcp connection

	os_printf("TCP recv data length: %d \r\n", length);
	os_printf("%s \n", data);
}

void ICACHE_FLASH_ATTR
tcpclient_sent_cb(void *arg)
{
    os_printf("Data is sent! \r\n");
}

void ICACHE_FLASH_ATTR
tcpclient_discon_cb(void *arg)
{
	// Memory freed automaticaly
	os_printf("TCP disconnect succeed\n");
}

void ICACHE_FLASH_ATTR
tcpclient_sent_data(struct espconn *pespconn)
{
	char *pbuf = (char *)os_zalloc(packet_size);

	char buffer[] = "GET / HTTP/1.1\r\nUser-Agent: curl/7.37.0\r\nHost: %s\r\nAccept: */*\r\n\r\n"; 
	os_sprintf(pbuf, buffer, "cn.bing.com");

	espconn_sent(pespconn, pbuf, os_strlen(pbuf));

	os_free(pbuf);
}

void ICACHE_FLASH_ATTR
tcpclient_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;
    os_printf("TCP connected! \r\n");

    espconn_regist_recvcb(pespconn, tcpclient_recv_cb);
    espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
   	espconn_regist_disconcb(pespconn, tcpclient_discon_cb);
   
   	// TCP connected so sent the data
    tcpclient_sent_data(pespconn);
}

void ICACHE_FLASH_ATTR
user_tcpclient_init(void)
{
    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = espconn_port(); 	// Random local port
    esp_conn.proto.tcp->remote_port = SERVER_PORT;

    const char ip[] = SERVER_IP;
	os_memcpy(esp_conn.proto.tcp->remote_ip, ip, 4);
	//os_printf("%d", ip[0]);

    espconn_regist_connectcb(&esp_conn, tcpclient_connect_cb);

    // Make tcp connection
    // This is a asynchronous call wich will performed later
    sint8 ret = espconn_connect(&esp_conn); 

    os_printf("ESP is connecting to remote server [%d]!\r\n", ret);
}