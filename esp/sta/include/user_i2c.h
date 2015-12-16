#ifndef __USER_I2C_H__
#define __USER_I2C_H__

#define DS1307_CTRL_ID 0x68

//580 mA DS1307

// struct ip_addr {
//     uint32 addr;
// };

// typedef struct ip_addr ip_addr_t;

// struct ip_info {
//     struct ip_addr ip;
//     struct ip_addr netmask;
//     struct ip_addr gw;
// };

struct DS1307 {
	uint8 second; 
	uint8 minute; 
	uint8 hour; 
	uint8 day;
	uint8 month; 
	uint8 year;
};

typedef struct DS1307 RTC;


void user_i2c_init(void);
void user_i2c_test(void); 
void user_ds1307_print(RTC *time); 
void user_ds1307_read(RTC *time);

#endif
