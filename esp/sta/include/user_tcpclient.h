#ifndef __USER_TCPCLIENT_H__
#define __USER_TCPCLIENT_H__

//#define SERVER_IP 
#define SERVER_IP {192, 168, 4, 1}
#define SERVER_PORT 80
#define packet_size (2 * 1024)

#include "user_interface.h"
#include "espconn.h"

void tcpclient_recv_cb(void *arg, char *data, unsigned short length);
void tcpclient_sent_cb(void *arg);
void tcpclient_discon_cb(void *arg);
void tcpclient_sent_data(struct espconn *pespconn, uint8 *data);
void tcpclient_connect_cb(void *arg);
void user_tcpclient_init(void);

#endif
