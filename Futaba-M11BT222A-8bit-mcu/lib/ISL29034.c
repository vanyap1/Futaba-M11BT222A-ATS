/*
 * ISL29034.c
 *
 * Created: 02.02.2023 20:55:56
 *  Author: Ivan Prints
 */ 

#include "ISL29034.h"
#include "twi_hal1.h"

uint8_t reg_conf[] = {0x00 , 0x00};
	
	
void ISL29034_init(const uint8_t run_mode, const enum data_resolution_enum measure_range, const enum lux_resolution_enum lux_range){
	reg_conf[0] |= (run_mode << OP0) | (INT_CLR << ISL29034_INT) | (CYCLES_1 << PRST0);
	reg_conf[1] |= (measure_range << RES0) | (lux_range << RANGE0);
	twi1_write(ISL29034_ADDR,COMMAND_1,reg_conf,sizeof(reg_conf));
}

uint16_t read_light_intensity(void){
	uint8_t reg_raw[2];
	twi1_read(ISL29034_ADDR, DATA_LSB, reg_raw, sizeof(reg_raw));
	return (reg_raw[1] << 8) + reg_raw[0];
}
uint8_t read_id (void){
	uint8_t reg_raw[1];
	twi1_read(ISL29034_ADDR, ISL29034_ID, reg_raw, sizeof(reg_raw));
	return reg_raw[0];
}