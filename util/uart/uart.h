// Module for uart for avr chips that supports it.
// Author: Joël Luijmes

// Example:
// GLOBAL:
// FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

// MAIN:
// 	uart_init();
// 	stdout = &mystdout;

// 	printf("Hello World!\n");


// Input:
// GLOBAL:
// FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

// MAIN:
//  uart_init();
//  stdin = &mystdin;

//  char buf[0x20];
//  scanf("%s", buf);



#pragma once

#include <util/setbaud.h>
#include <stdio.h>
#include <avr/io.h>

void uart_init();
int uart_putchar(char c, FILE* stream);
char uart_getchar(FILE* stream);