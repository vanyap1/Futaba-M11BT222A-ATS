/*
 * ISL29034.h
 *
 * Created: 02.02.2023 20:56:07
 *  Author: Ivan Prints
 */ 

#include <avr/io.h>
//#include "stdbool.h"


#ifndef ISL29034_H_
#define ISL29034_H_



#define ISL29034_ADDR		0x44

#define COMMAND_1			0x00
#define PRST0				0
#define PRST1				1
#define ISL29034_INT		2
#define OP0					5
#define OP1					6
#define OP2					7

#define COMMAND_2			0x01

#define RANGE0				0
#define RANGE1				1
#define RES0				2
#define RES1				3

#define DATA_LSB            0x02
#define DATA_MSB            0x03
#define INT_LT_LSB          0x04
#define INT_LT_MSB          0x05
#define INT_HT_LSB          0x06
#define INT_HT_MSB          0x07
#define ISL29034_ID         0x0f


//Command-I Register (Address: 0x00)
//NUMBER OF INTEGRATION CYCLES (n)
#define CYCLES_1			0
#define CYCLES_4			1
#define CYCLES_8			2
#define CYCLES_16			3
// INTERRUPT STATUS BIT (INT)
#define INT_EN				1
#define INT_CLR				0
//OPERATING MODES BITS
#define POWER_DOWN			0
#define POWER_SAVE			1
#define POWER_RUN			5


//Command-II Register (Address: 0x01)
//RANGE REGISTER BITS
enum lux_resolution_enum {
	LUX_1000,
	LUX_4000,
	LUX_16000,
	LUX_64000,
};
//ADC RESOLUTION DATA
enum data_resolution_enum {
	RES_16_BIT,
	RES_12_BIT,
	RES_8_BIT,
	RES_4_BIT,
};
 
#endif









void ISL29034_init(const uint8_t run_mode, const enum data_resolution_enum measure_range, const enum lux_resolution_enum lux_range);
uint16_t read_light_intensity(void);
uint8_t read_id(void);

