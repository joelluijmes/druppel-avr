#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../../util/twi/twi.h"
#include "../../util/uart/uart.h"
#include "../../util/ds1307/ds1307.h"

#include "sensors.h"


FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	uart_init();
	stdout = &mystdout;
	sei();
	DDRB = 0x20;

	twi_master_init();

	//_delay_ms(20);

	uint32_t unixtime = read_unix_time();
	// Print unixtime
	//printf("%u %u\n", (uint16_t) (unixtime >> 16), (uint16_t) unixtime);





}