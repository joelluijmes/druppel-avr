/*
*	Module for twi/i2c interface
*
*	Author: Joël Luijmes
*	Data: 04-06-2015 (updated: 11-06-2015)
*
*	Uses the avr twi interface, designed for mlx90614 -> only supports
*	MasterTransmit and MasterReceiver mode./
*
*	Example; (see mlx90614.c)
*/


#pragma once

#include <util/twi.h>
#include <stdio.h>

#define SCL_CLOCK 100000L

static inline uint8_t twi_status()
{
	return TWSR & 0xF8;
}

void twi_init();
uint8_t twi_start();
uint8_t twi_start_slave(uint8_t slave_addr);
uint8_t twi_write(uint8_t data);
void twi_mt_start(uint8_t slave_addr);
void twi_mr_start(uint8_t slave_addr);
void twi_stop();
uint8_t twi_read();
uint8_t twi_peek();
