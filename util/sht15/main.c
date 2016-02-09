#include <avr/io.h>
#include <util/delay.h>

#include "../pin.h"
#include "sht.h"

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	stdout = &mystdout;
	uart_init();

	pin sck =
	{
		.ddr = &DDRD,
		.out = &PORTD,
		.in = &PIND,
		.mask = 1 << 3
	};
	pin data =
	{
		.ddr = &DDRD,
		.out = &PORTD,
		.in = &PIND,
		.mask = 1 << 4
	};

	sht sht =
	{
		.pinSCK = sck,
		.pinDATA = data
	};

	pin_output(&sck);
	pin_output(&data);

	while (1)
	{
		double tt = sht_readTemperature(&sht);
		int temp = (int)(tt*100);
		int humi = (int)(sht_readHumidity(&sht)*100);
		int comp = (int)(sht_compensateHumidity(&sht, tt)*100);

		printf("Temperature: %d\tHumidity: %d\tCompensated: %d\n", temp, humi, comp);
		_delay_ms(1000);
	}

	return 0;
}