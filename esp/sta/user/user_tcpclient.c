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

#include "user_global_definitions.h"
#include "i2c_slave.h"

static uint8_t tcp_buffer[70];
static uint8_t tcp_bytes; 

static struct espconn esp_conn;
static esp_tcp esptcp;

static bool DISCONNECT_AFTER_SENT;

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

        // struct DS1307 time; 
        // user_ds1307_read(&time);

        // char *pbuf = (char *)os_zalloc(packet_size);

        // os_sprintf(pbuf, "DATETIME: %d:%d:%d %d-%d-%d \n", 
        //     bcd2dec(time.hour & 0x3f),
        //     bcd2dec(time.minute), 
        //     bcd2dec(time.second & 0x7f), 
        //     bcd2dec(time.day),
        //     bcd2dec(time.month),
        //     bcd2dec(time.year)
        // );

        // DISCONNECT_AFTER_SENT = true; 
        // espconn_sent(pespconn, pbuf, os_strlen(pbuf));


        DISCONNECT_AFTER_SENT = true; 
        espconn_sent(pespconn, tcp_buffer, tcp_bytes);
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

    wifi_status = WIFI_READY;

    i2c_update_status(I2C_READING_START);
}

void ICACHE_FLASH_ATTR
tcpclient_sent_data(struct espconn *pespconn, uint8 *data, uint8_t length)
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
    tcpclient_sent_data(pespconn, buffer, os_strlen(buffer));
    //os_free(buffer); 
}

void ICACHE_FLASH_ATTR
user_tcpclient_init(uint8_t *buf, uint8_t buflen)
{
    wifi_status = WIFI_BUSY;
    DISCONNECT_AFTER_SENT = false; 

    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = espconn_port(); 	// Random local port
    esp_conn.proto.tcp->remote_port = SERVER_PORT;

    const char ip[] = SERVER_IP;
	os_memcpy(esp_conn.proto.tcp->remote_ip, ip, 4);

    espconn_regist_connectcb(&esp_conn, tcpclient_connect_cb);


    tcp_bytes = buflen; 
    os_memcpy(tcp_buffer, buf, buflen);

    os_printf("%d, %d %d", buflen, buf[0], buf[1]);

    // Make tcp connection
    // This is a asynchronous call wich will performed later
    sint8 ret = espconn_connect(&esp_conn); 

    os_printf("ESP is connecting to remote server [%d]!\r\n", ret);
}