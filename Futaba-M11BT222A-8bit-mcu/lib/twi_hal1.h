/*
 * twi_hal.h
 *
 * Created: 2020/11/01 18:07:26
 *  Author: DevilBinder
 */ 


#ifndef TWI_HAL1_H_
#define TWI_HAL1_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>

#define TWI_TIMEOUT 1600

#define TWI_START		0x08
#define TWI_RSTART		0x10
#define TWIT_ADDR_ACK	0x18
#define TWIT_ADDR_NACK	0x20
#define TWIT_DATA_ACK	0x28
#define TWIT_DATA_NACK	0x30

#define TWIR_ADDR_ACK	0x40
#define TWIR_ADDR_NACK	0x48
#define TWIR_DATA_ACK	0x50
#define TWIR_DATA_NACK	0x58

#define TWI_ERROR		0x38
#define TWI_NONE		0xF8

enum{
	TWI_OK,
	TWI_ERROR_START,
	TWI_ERROR_RSTART,
	TWI_NACK
};

uint8_t twi1DataReceived();

void twi1_init(uint32_t speed);
uint8_t twi1_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);
uint8_t twi1_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);
uint8_t twi1_transfer(uint8_t addr,uint8_t *data,uint16_t len);
void twi1_slave_init(uint8_t addr, uint8_t *data, uint32_t speed);

#endif /* TWI_HAL_H_ */