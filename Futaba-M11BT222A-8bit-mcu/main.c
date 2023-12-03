/*
 * Futaba-M11BT222A-8bit-mcu.c
 *
 * Created: 07.11.2023 23:22:58
 * Author : Vanya
 */ 

#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

#include "string.h"
#include "stdbool.h"

#include "adc_hal.h"
#include "gpio_driver.h"
#include "PT63xx.h"
#include "ISL29034.h"
#include "rtc.h"
//#include "twi_hal0.h"
#include "twi_hal1.h"
#include "uart_hal.h"


uint8_t light_addr = 0x44;

gpio rtc_int = {(uint8_t *)&PORTD , PORTD2};
gpio ld1 = {(uint8_t *)&PORTB , PORTB5};		//debug led
uint8_t redDotLine = 1;

rtc_date sys_rtc = {
	.date = 23,
	.month = 11,
	.year = 23,
	.dayofweek = 5,
	.hour = 23,
	.minute = 35,
	.second = 00
};
	
uint8_t rtc_int_request = 0;
uint16_t counter = 0;
uint8_t disc = 0;

ISR(INT0_vect){
	rtc_int_request=1; //RTC New data ready
}




int main(void)
{
   char char_array[128]="\0";
   uart_init(250000,0);
   twi1_init(400000);
   gpio_set_pin_direction(&ld1, PORT_DIR_OUT);
   
   EICRA |= (0b10 << ISC00);
   EIMSK = 0x01;
   sei();
   
   //rtc_set(&sys_rtc);																//write time to RTC
   rtc_int_enable(&sys_rtc ,0);														//Serup RTC
   adc_init();
   
   pt63Init();
   pt63SetBrightness(10);
   ISL29034_init(POWER_RUN, RES_16_BIT, LUX_1000);
   
   setIcon(TILE_AUTO, true);	 
   setIcon(TILE_USB, true);
    
	while (1) 
    {
		
		
		
		//setIcon(TILE_TRANS, true);
		//setIcon(TILE_AUTO, true);
		//setIcon(TILE_EPG, true);
		//setIcon(TILE_USB, true);
		//setIcon(TILE_MAIL, true);
		//setIcon(TILE_TITLE, true);
		//setIcon(TILE_CHP, true);
		//setIcon(TILE_TRK, true);
		//setIcon(TILE_CH, true);
		//setIcon(TILE_RATE, true);
		//
		//setTile(TILE_NETRING, 0xffff);
		
		//setTile(TILE_BINFIELD, 0xffff);
		
		//
		//_delay_ms(50);
		//setIcon(TILE_TRANS, false);
		//setIcon(TILE_AUTO, false);
		//setIcon(TILE_EPG, false);
		//setIcon(TILE_USB, false);
		//setIcon(TILE_MAIL, false);
		//setIcon(TILE_TITLE, false);
		//setIcon(TILE_CHP, false);
		//setIcon(TILE_TRK, false);
		//setIcon(TILE_CH, false);
		//setIcon(TILE_RATE, false);
		//
		//setTile(TILE_NETRING, 0x0);
		//setTile(TILE_OTHER, 0x0);//
		//setTile(TILE_BINFIELD, 0x0);
		//setTile(TILE_DISK, 0x0);
		
		if (rtc_int_request){
			rtc_sync(&sys_rtc);
			
			//sprintf(char_array, "%02d-%02d", sys_rtc.month, sys_rtc.date);
			sprintf(char_array, "%05d", read_light_intensity());
			
			writeChars((void *)char_array, 5);
			sprintf(char_array, "%02d%02d%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			writeDigits((void *)char_array, 1, 6);
			setTile(TILE_BINFIELD, counter);
			setTile(TILE_OTHER, (redDotLine << 8));
			setTile(TILE_DISK, discGetSegment(disc, FILL_LEFT));
			disc++;
			if(disc >= 9) {disc = 0;}
			
			
			redDotLine = redDotLine << 1;
			redDotLine = (redDotLine > 8) ? 1 : redDotLine;
			
			counter++;
			rtc_int_request=0;
		}
		
		
		
		
		
		
    }
}

