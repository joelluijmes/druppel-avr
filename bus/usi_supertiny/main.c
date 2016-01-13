#include <util/delay.h>
#include <avr/io.h>

#include "../../util/twi/twi.h"
// #include "../../util/twi/usi/twi_usi.h"
// #include "../../util/eeprom_usi/eeprom.h"
//#include "../../util/ds1307_usi/ds1307.h"
//#include "../../util/bmp180_usi/bmp180.h"
#include "communication.h"

int main()
{
	uint8_t buff[64]; 
	uint8_t i; 
	for(i = 0; i < 64; i++)
		buff[i] = 0xAB; 

	communication_send_data(buff, 64);

	return 0; 
}
