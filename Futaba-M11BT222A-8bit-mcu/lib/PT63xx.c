
/*
 * PT63xx.c
 *
 * Created: 21.11.2023 22:41:28
 *  Author: Vanya
 */ 
#include "PT63xx.h"
#include "gpio_driver.h"
#include <stdint.h>

uint8_t vfd_buff[64];

uint8_t digit_order[] = {0xcf,0xd5,0xd6,0xd2,0xd3,0xd0};
uint8_t char_order[] = {0xcc,0xc9,0xc6,0xc3,0xc0,0xcf,0xd5,0xd6,0xd2,0xd3,0xd0};
uint8_t digit_table[] = {119,18,107,91,30,93,125,19,127,31};
uint8_t char_table_up[] = {49,24,209,209,240,225,225,17,241,241,0,0,0,0,0,0,0,241,213,33,149,161,161,225,240,132,16,168,32,186,178,49,241,177,241,225,133,48,168,176,138,138,137};
uint8_t char_table_low[] = {70,2,69,66,3,67,71,2,71,67,0,0,0,0,0,0,0,7,82,68,82,69,5,70,7,16,70,37,68,6,38,70,5,102,37,67,16,70,12,46,40,16,72 };

uint8_t discFillRightValue [] = {0x0, 0x1, 0x3, 0x7, 0x0F ,0x1F ,0x3F, 0x7F, 0xFF};
uint8_t discFillLeftValue [] = {0x0, 0x80, 0xC0, 0xE0, 0xF0 ,0xF8, 0xFC, 0xFE, 0xFF};	
uint8_t discFillSegValue [] = {0x0, 0x1, 0x2, 0x4, 0x8 ,0x10 ,0x20, 0x40, 0x80};	
	
	
gpio vfd_stdb = {(uint8_t *)&PORTD , PORTD6};
gpio vfd_cs = {(uint8_t *)&PORTB , PORTB1};
gpio vfd_clk = {(uint8_t *)&PORTB , PORTB0};
gpio vfd_do = {(uint8_t *)&PORTD , PORTD7};






void pt63Init(void){
	 gpio_set_pin_direction(&vfd_stdb, PORT_DIR_OUT);
	 gpio_set_pin_direction(&vfd_cs, PORT_DIR_OUT);
	 gpio_set_pin_direction(&vfd_clk, PORT_DIR_OUT);
	 gpio_set_pin_direction(&vfd_do, PORT_DIR_OUT);
	 
	 gpio_set_pin_level(&vfd_stdb, true);
	 vfd_buff[0] = 0x40;
	 vfd_buff[1] = 0xc0;
	 writeData((uint8_t *)&vfd_buff, 49);
	 writeCmd(0x0a);
	 
	 
	 
}
void pt63SetBrightness(uint8_t brightness){
	brightness = (brightness <= MAX_BRIGHTNESS) ? brightness : MAX_BRIGHTNESS; 
	writeCmd(brightness | 0x88);
}

void setIcon(uint8_t tile_address, uint8_t tile_status){
	vfd_buff[0] = 0x40;
	vfd_buff[1] = tile_address;
	vfd_buff[2] = (tile_status) ? 0xFF : 0; 
	writeData((uint8_t *)&vfd_buff, 3);
}
void setTile(uint8_t tile_address, uint16_t tile_data){
	vfd_buff[0] = 0x40;
	if(tile_address == TILE_DISK){
		vfd_buff[1] = tile_address;
		vfd_buff[2] = tile_data;
		writeData((uint8_t *)&vfd_buff, 3);
	}else{
		vfd_buff[1] = tile_address;
		vfd_buff[2] = tile_data >> 8;
		vfd_buff[3] = tile_data & 0xff;
		writeData((uint8_t *)&vfd_buff, 4);
	}
}

uint8_t discGetSegment(uint8_t arg, uint8_t fillingType){
	arg = (arg >= 8) ? 8 : arg;
	switch(fillingType) {
		case FILL_RIGHT:
			return discFillRightValue[arg];
		break;
		case FILL_LEFT:
		return discFillLeftValue[arg];
		break;
		case FILL_SEGMENT:
			return discFillSegValue[arg];
		break;
		default:
			return discFillSegValue[arg];
	}
}
	
void writeChars(uint8_t *data, uint8_t len){
	uint8_t n, tmp;
	for(n=0; n<len; n++){
		if(data[n] == 32){
			tmp = 0x0a;
		}else{
			tmp = data[n] - 48;
		}
	vfd_buff[0] = 0x40;
	vfd_buff[1] = char_order[n];
	vfd_buff[2] = char_table_up[tmp];
	vfd_buff[3] = char_table_low[tmp];
	writeData((uint8_t *)&vfd_buff, 4);
	}
}	

void writeDigits(uint8_t *data, uint8_t dot, uint8_t len){
	uint8_t n, tmp;
	for(n=0; n<len; n++){
		tmp = data[n] - 48;
		vfd_buff[0] = 0x40;
		vfd_buff[1] = digit_order[n];
		vfd_buff[2] = (dot) ? digit_table[tmp] | 0x80 :  digit_table[tmp];
		writeData((uint8_t *)&vfd_buff, 3);
	}
}
	
void writeCmd(uint8_t cmd){
	gpio_set_pin_level(&vfd_cs, true);
	vfd_write_byte(cmd);
	gpio_set_pin_level(&vfd_cs, false);
}


void writeData(uint8_t *data, uint8_t len){
	uint8_t n;
	gpio_set_pin_level(&vfd_cs, true);
	vfd_write_byte(data[0]);
	gpio_set_pin_level(&vfd_cs, false);
	
	gpio_set_pin_level(&vfd_cs, true);
	for(n=1; n<len; n++)
	{
		vfd_write_byte(data[n]);
	}
	gpio_set_pin_level(&vfd_cs, false);
	
}

void vfd_write_byte(uint8_t data){
	unsigned char n;
	for(n=0; n<8; n++)
	{
		gpio_set_pin_level(&vfd_clk, true);
		if((1 << n) & data){
			gpio_set_pin_level(&vfd_do, true);
			}else{
			gpio_set_pin_level(&vfd_do, false);
		}
		gpio_set_pin_level(&vfd_clk, false);
		
	}
}