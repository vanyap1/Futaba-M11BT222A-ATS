
/*
 * PT63xx.h
 *
 * Created: 21.11.2023 22:41:19
 *  Author: Vanya
 */ 

#include <avr/io.h>


#ifndef PT63_H_
#define PT63_H_

#define MAX_BRIGHTNESS	7



#define  TILE_TRANS		0xd1
#define  TILE_AUTO		0xd4
#define  TILE_EPG		0xd7
#define  TILE_USB		0xdd
#define  TILE_MAIL		0xe0
#define  TILE_TITLE		0xce
#define  TILE_CHP		0xc8
#define  TILE_TRK		0xc5
#define  TILE_CH		0xc2
#define  TILE_RATE		0xcb


#define  TILE_NETRING	0xde
#define  TILE_DISK		0xd8
#define  TILE_OTHER		0xd9
#define  TILE_BINFIELD	0xdb

#endif

void pt63Init(void);
void pt63SetBrightness(uint8_t brightness);
void setIcon(uint8_t tile_address, uint8_t tile_status);
void setTile(uint8_t tile_address, uint16_t tile_data);
void writeChars(uint8_t *data, uint8_t len);
void writeDigits(uint8_t *data, uint8_t dot, uint8_t len);
void writeCmd(uint8_t cmd);
void writeData(uint8_t *data, uint8_t len);
void vfd_write_byte(uint8_t data);