#ifndef __USER_STATE_H__
#define __USER_STATE_H__

enum state
{
    STATE_IDLE = 0, 
    STATE_CONNECT = 1,
    STATE_DISCONNECT = 2,
    STATE_CONNECTED = 3, 
    STATE_DISCONNECTED = 4,
    STATE_BUSY = 4,
};
typedef enum state state;

// enum state
// {
// 	STATE_IDLE = 0, 
// 	STATE_CONNECT = 1,
// 	STATE_DISCONNECT = 2,
//     STATE_CONNECTED = 3, 
//     STATE_DISCONNECTED = 4,
//     STATE_BUSY = 4,

//     STATE_TCP_BUSY = 5, 
//     STATE_TCP_READY = 5, 
//     STATE_TCP_CONNECTED = 6, 
//     STATE_TCP_DISCONNECTED = 6, 


//     // STATE_NOT_ATTEMPTED = 0,
//     // STATE_NOT_RESPONDING = 1,
//     // STATE_NOT_CONNECTED = 2,
//     // STATE_TCP_CONNECTING = 4,
//     // STATE_TCP_CONNECTED = 5,
//     // STATE_TCP_DISCONNECTED = 6,
//     // STATE_SENDING_DONE = 7,
// };
// typedef enum state state;

state get_state(); 
void update_state(state state);

#endif
