#include "uart.h"

static char _prevChar;

void uart_init()
{
	UBRR0H = UBRRH_VALUE; // set high baud
	UBRR0L = UBRRL_VALUE; //set low baud

	#ifdef USE_2X
	UCSR0A |= _BV(U2X0);
	#else
	UCSR0A &= ~_BV(U2X0);
	#endif

	UCSR0B = _BV(TXEN0) | _BV(RXEN0); //enable duplex
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); //8-N-1
}

int uart_putchar(char c, FILE* stream)
{
	if (c == '\n' && _prevChar != '\r') // we should send the CR too
		uart_putchar('\r', stream);

	loop_until_bit_is_set(UCSR0A, UDRE0);	//wait till prev char is read
	UDR0 = c;
	_prevChar = c;

	return 0;
}

char uart_getchar(FILE* stream)
{
	loop_until_bit_is_set(UCSR0A, RXC0);	//wait if there is data
	return UDR0;
}