#include <avr/io.h>
#include <util/delay.h>

#include "../twi/twi.h"
#include "ds1307.h"

int main()
{
	while(1) 
	{
		uint32_t unixtime = read_unix_time(); 
		_delay_ms(1000);
	}
}