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


#include "user_i2c.h"


LOCAL struct espconn esp_conn;
LOCAL esp_tcp esptcp;

LOCAL bool DISCONNECT_AFTER_SENT;

ip_addr_t tcp_server_ip;

void ICACHE_FLASH_ATTR
tcpclient_recv_cb(void *arg, char *data, unsigned short length)
{
	struct espconn *pespconn = arg;
	//received some data from tcp connection

	os_printf("TCP recv data length: %d \r\n", length);
	os_printf("%s \n", data);

    if(length == 2 && strcmp("OK", data) == 0) 
    {
        // Receive OK so we can sent the sensor data:)
        // TODO send data to supertiny to check if receive data is allowed...

        struct DS1307 time; 
        user_ds1307_read(&time);

        char *pbuf = (char *)os_zalloc(packet_size);

        os_sprintf(pbuf, "DATETIME: %d:%d:%d %d-%d-%d \n", 
            bcd2dec(time.hour & 0x3f),
            bcd2dec(time.minute), 
            bcd2dec(time.second & 0x7f), 
            bcd2dec(time.day),
            bcd2dec(time.month),
            bcd2dec(time.year)
        );


        DISCONNECT_AFTER_SENT = true; 
        espconn_sent(pespconn, pbuf, os_strlen(pbuf));
    }

}

void ICACHE_FLASH_ATTR
tcpclient_sent_cb(void *arg)
{
    struct espconn *pespconn = arg;
    os_printf("Data is sent! \r\n");

    if(DISCONNECT_AFTER_SENT) 
        espconn_disconnect(pespconn);
}

void ICACHE_FLASH_ATTR
tcpclient_discon_cb(void *arg)
{
	// Memory freed automaticaly
	os_printf("TCP disconnect succeed\n");

    system_soft_wdt_restart(); 
    os_delay_us(2500*1000); 
    system_soft_wdt_restart(); 
    os_delay_us(2500*1000); 
    system_soft_wdt_restart(); 
    os_delay_us(2500*1000); 
    // system_soft_wdt_restart(); 

    system_soft_wdt_feed(); 
    user_tcpclient_init(); 
}

void ICACHE_FLASH_ATTR
tcpclient_sent_data(struct espconn *pespconn, uint8 *data)
{
    //sint8 espconn_sent(struct espconn *espconn, uint8 *psent, uint16 length);
	//char *pbuf = (char *)os_zalloc(packet_size);

	// char buffer[] = "AR";  // AR request
	// os_sprintf(pbuf, buffer);

	espconn_sent(pespconn, data, os_strlen(data));

	//os_free(pbuf);
}

void ICACHE_FLASH_ATTR
tcpclient_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;
    os_printf("TCP connected! \r\n");

    espconn_regist_recvcb(pespconn, tcpclient_recv_cb);
    espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
   	espconn_regist_disconcb(pespconn, tcpclient_discon_cb);
   
    char buffer[] = "AR";

   	// TCP connected so sent the data
    tcpclient_sent_data(pespconn, buffer);
    //os_free(buffer); 
}

void ICACHE_FLASH_ATTR
user_tcpclient_init(void)
{
    DISCONNECT_AFTER_SENT = false; 

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