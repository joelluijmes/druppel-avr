# Library for TWI

## Compile
`avr-gcc -O2 -Wall -mmcu=atmega328p -DF_CPU=16000000 -c twi.c -o twi.o`

## Static library
`avr-ar rcs libtwi.a twi.o`