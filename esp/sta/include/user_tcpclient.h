#ifndef __USER_TCPCLIENT_H__
#define __USER_TCPCLIENT_H__

//#define SERVER_IP 
#define SERVER_IP {192, 168, 4, 1}
#define SERVER_PORT 80
#define packet_size (2 * 1024)

#include "user_state.h"
#include "user_interface.h"
#include "espconn.h"

state tcpclient_get_state();
void tcpclient_update_state(state state);
void user_tcpclient_init(); 
void tcpclient_sent_data(struct espconn *pespconn, uint8 *data, uint8_t length);

#endif
