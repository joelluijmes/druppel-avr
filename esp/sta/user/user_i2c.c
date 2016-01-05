/******************************************************************************
 *
 * FileName: user_softap.c
 *
 * Description: Setup softap.
 *
 * Modification history:
 *     2015/12/12, v1.0 create this file.
*******************************************************************************/

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "user_interface.h"
#include "user_i2c.h"


#include "driver/i2c_master.h"


void ICACHE_FLASH_ATTR 
user_i2c_test(void)
{
	//ets_delay_us(30000);//  wait measure time, 24ms at most

    uint8 addr = DS1307_CTRL_ID << 1 | 1; 
    os_printf("%d\n", addr); 

    uint8 ack;
    uint16 i;

    i2c_master_start();
    //i2c_master_writeByte(0xAB);
    //0x68

    i2c_master_writeByte(DS1307_CTRL_ID << 1 | 1); // Shift one to left and add 1 for reading mode... 

    //i2c_master_writeByte(DS1307_CTRL_ID + 1);
    
    if (i2c_master_getAck()) {
        os_printf("addr not ack when tx write cmd %d \n", ack);
        i2c_master_stop();
        return;// false;
    }

    os_printf("Succesfully");

    for (i = 0; i < 4; i++) {
        os_printf("%d\n", i2c_master_readByte());

        i2c_master_setAck((i == (4 - 1)) ? 1 : 0);
    }

    i2c_master_stop(); 

    // i2c_master_stop();
    // i2c_master_wait(40000);

    // i2c_master_start();
    // i2c_master_writeByte(0xAB + 1);
    // ack = i2c_master_getAck();

    // if (ack) {
    //     os_printf("addr not ack when tx write cmd \n");
    //     i2c_master_stop();
    //     return;// false;
    // }

    // for (i = 0; i < 4; i++) {
    //     os_printf("%c\n", i2c_master_readByte());

    //     i2c_master_setAck((i == (4 - 1)) ? 1 : 0);
    // }
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

void ICACHE_FLASH_ATTR
user_ds1307_print(RTC *time)
{
    os_printf("DATETIME: %d:%d:%d %d-%d-%d \n", 
        bcd2dec(time->hour & 0x3f),
        bcd2dec(time->minute), 
        bcd2dec(time->second & 0x7f), 
        bcd2dec(time->day),
        bcd2dec(time->month),
        bcd2dec(time->year)
  );
}

void ICACHE_FLASH_ATTR
user_ds1307_read(RTC *time)
{
    /* Scope trigger at 50 qs */

    i2c_master_start();

    // Shift one to left, set write mode
    i2c_master_writeByte(DS1307_CTRL_ID << 1);

    if (i2c_master_getAck()) {
        // Addr not ack when tx write cmd
        os_printf("Device doesn't respond [1] \n"); 
        i2c_master_stop();
        return;
    }

    // Write zero, so the register pointer points to seconds
    i2c_master_writeByte(0x00);

    if (i2c_master_getAck()) {
        os_printf("Device doesn't respond [2] \n"); 
        i2c_master_stop();
        return;
    }

    //i2c_master_wait(5);
    i2c_master_start();

    // Write slave adress and reading mode
    i2c_master_writeByte(DS1307_CTRL_ID << 1 | 1); 

    if (i2c_master_getAck()) {
        os_printf("Device doesn't respond [3] \n"); 
        i2c_master_stop();
        return;
    }

    time->second = i2c_master_readByte();
    i2c_master_send_ack();   

    time->minute = i2c_master_readByte(); 
    i2c_master_send_ack();   

    time->hour = i2c_master_readByte(); 
    i2c_master_send_ack();   
    
    i2c_master_readByte(); // Week number or day in week
    i2c_master_send_ack();   

    time->day = i2c_master_readByte(); 
    i2c_master_send_ack();   
    time->month = i2c_master_readByte(); 
    i2c_master_send_ack();   
    time->year = i2c_master_readByte(); 
    i2c_master_send_nack();

    // Stopping the bus
    i2c_master_stop(); 
}

void ICACHE_FLASH_ATTR 
user_i2c_init(void)
{
	os_printf("I2C user init\n");


	i2c_master_gpio_init(); 

    struct DS1307 time; 


    user_ds1307_read(&time);
    user_ds1307_print(&time); 

    //100khz = 10 us 

    //system_soft_wdt_stop();
    //uint8 i = 0;
    //for(i = 0; i < 10; i++) 
    while(1)
    {
        os_delay_us(2000*1000); 
        os_printf("Reading values.. \n"); 	


        user_ds1307_read(&time);
        user_ds1307_print(&time); 

        system_soft_wdt_restart();
        //system_soft_wdt_feed();         // Reset watchdog timer to 0 so it doesn't restart the esp
    }
}
