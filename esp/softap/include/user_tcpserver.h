#ifndef __USER_TCPSERVER_H__
#define __USER_TCPSERVER_H__

#include "user_interface.h"



void user_tcpserver_init(uint32 port); 

void tcpserver_listen(); 

void tcpserver_discon_cb(void *arg);

void tcpserver_recon_cb(void *arg, sint8 err); 
void tcpserver_recv_cb(void *arg, char *data, unsigned short length);


#endif
