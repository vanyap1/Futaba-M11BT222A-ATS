/*
 * rtc.c
 *
 * Created: 29.01.2023 21:33:10
 *  Author: User
 */ 
#include "rtc.h"
#include "twi_hal1.h"
#include <stdint.h>
uint8_t rtc_config_1[] = {0x40 , 0x00};
uint8_t rtc_config_2[] = {0x00};
uint8_t rtc_config_3[] = {0x00};



 	
char* day[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};



void rtc_int_enable(rtc_date *RTCx , const uint16_t int_time){
	rtc_config_3[0] |= (1 << TIE);
	rtc_config_2[0] |= (1 << TD) | (1 << TE);
	
	twi1_write(RTC_ADDR,TIMER_COUNTER0_REG,rtc_config_1,sizeof(rtc_config_1));
	twi1_write(RTC_ADDR,EXT_REG,rtc_config_2,sizeof(rtc_config_2));
	twi1_write(RTC_ADDR,CONTROLL_REG,rtc_config_3,sizeof(rtc_config_3));
	
	
}

void rtc_sync(rtc_date *RTCx){
	uint8_t rtc_raw[7];
	twi1_read(RTC_ADDR, 0x00, rtc_raw, sizeof(rtc_raw));
	RTCx->second=BCDtoDEC(rtc_raw[0]);
	RTCx->minute=BCDtoDEC(rtc_raw[1]);
	RTCx->hour=BCDtoDEC(rtc_raw[2]);
	RTCx->dayofweek=rtc_raw[3];
	RTCx->date=BCDtoDEC(rtc_raw[4]);
	RTCx->month=BCDtoDEC(rtc_raw[5]);
	RTCx->year=BCDtoDEC(rtc_raw[6]);
}
void rtc_set(rtc_date *RTCx){
	uint8_t rtc_raw[7];
	rtc_raw[0]=DECtoBCD(RTCx->second);
	rtc_raw[1]=DECtoBCD(RTCx->minute);
	rtc_raw[2]=DECtoBCD(RTCx->hour);
	rtc_raw[3]=RTCx->dayofweek;
	rtc_raw[4]=DECtoBCD(RTCx->date);
	rtc_raw[5]=DECtoBCD(RTCx->month);
	rtc_raw[6]=DECtoBCD(RTCx->year);
	twi1_write(RTC_ADDR,0x00,rtc_raw,sizeof(rtc_raw));
}

uint8_t BCDtoDEC(uint8_t val)
{
	return ((val / 0x10) * 10) + (val % 0x10);
}

// BCDtoDEC -- convert decimal to binary-coded decimal (BCD)
uint8_t DECtoBCD(uint8_t val)
{
	return ((val / 10) * 0x10) + (val % 10);
}

uint32_t convert_to_timestamp(const rtc_date *date) {
	uint8_t year = date->year;
	uint8_t month = date->month;
	uint8_t day = date->date;
	uint8_t hour = date->hour;
	uint8_t minute = date->minute;
	uint8_t second = date->second;

	uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
		days_in_month[1] = 29;
	}

	uint32_t timestamp = 0;
	timestamp += second;
	timestamp += minute * 60;
	timestamp += hour * 3600;
	for (uint8_t i = 0; i < month - 1; i++) {
		timestamp += days_in_month[i] * 86400;
	}

	for (uint8_t i = 0; i < year; i++) {
		if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)) {
			timestamp += 31622400; 
			} else {
			timestamp += 31536000; 
		}
	}
	timestamp += (day - 1) * 86400;

	return timestamp;
}