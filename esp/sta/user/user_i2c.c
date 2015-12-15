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

    uint8 ack;
    uint16 i;

    i2c_master_start();
    i2c_master_writeByte(0xAB);
    //0x68
    //i2c_master_writeByte(0x68);
    
    ack = i2c_master_getAck();

    if (ack) {
        os_printf("addr not ack when tx write cmd %d \n", ack);
        i2c_master_stop();
        return;// false;
    }

    os_printf("Succesfully");

    i2c_master_writeByte(0x23);
    i2c_master_writeByte(0x33);

    i2c_master_stop();
    i2c_master_wait(40000);

    i2c_master_start();
    i2c_master_writeByte(0xAB + 1);
    ack = i2c_master_getAck();

    if (ack) {
        os_printf("addr not ack when tx write cmd \n");
        i2c_master_stop();
        return;// false;
    }

    for (i = 0; i < 4; i++) {
        os_printf("%c\n", i2c_master_readByte());

        i2c_master_setAck((i == (4 - 1)) ? 1 : 0);
    }
}

void ICACHE_FLASH_ATTR 
user_i2c_init(void)
{
	os_printf("I2C user init\n ");

	/*
	trigger at 50 qs
	*/

	i2c_master_gpio_init(); 

	system_soft_wdt_stop();

	 uint8 i = 0;
	 for(i = 0; i < 10; i++) 
	 {
		os_delay_us(2000*1000); 
		os_printf("test.. \n"); 	
		
		user_i2c_test();

		system_soft_wdt_restart();
		// os_delay_us(2000*1000); 
		// os_printf("ok");
	 }

	// while(1)
	// {
	// 	//os_delay_us(200*1000); 
	// 	GPIO_OUTPUT_SET(2, 0);
	// 	GPIO_OUTPUT_SET(2, 1);
	// }
}
