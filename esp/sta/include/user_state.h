#ifndef __USER_STATE_H__
#define __USER_STATE_H__

#define CLOSE 0
#define KEEP_ALIVE 1

enum state
{
    STATE_IDLE = 0, 
    STATE_CONNECT = 1,
    STATE_DISCONNECT = 2,
    STATE_CONNECTED = 3, 
    STATE_DISCONNECTED = 4,
    STATE_BUSY = 5
};
typedef enum state state;

state get_state(); 
void update_state(state state);

#endif
