/******************************************************************************
 *
 *   Filename: $UI_Timer.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains header information about timer 2 related functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#ifndef UI_TIMER_H_
#define UI_TIMER_H_

#define NOT_TIMEOUT 0
#define TIME_OUT 1
#define BUTTON_DE_BOUNCE_TIME (180)
extern volatile uint_32 GPS_OFF_timer_count;
extern volatile uint_32 UI_timer_count;
unsigned int Check_UI_Timer_Timeout(void);
void ui_timer_stop(void);
void Button_bounce_timer_stop(void);
void Button_bounce_timer_start(uint_32 time)  ;
void Sleep_timer_stop(void);
void Sleep_timer_start(uint_32 time) ;
void ui_timer_stop(void);
void ui_timer_start(uint_32 time) ;
void GPS_OFF_timer_start(uint_32 time);
void GPS_Lock_timer_start(uint_32 time)  ;
void GPS_ON_timer_start(uint_32 time) ;
void Battery_check_timer_stop(void);
void Battery_check_timer_start(uint_32 time);
void Long_beep_timer_stop(void);
void Long_beep_timer_start(uint_32 time) ;
void Short_beep_timer_stop(void);
void Short_beep_timer_start(uint_32 time);
void ui_timer_init(void);
void ui_timer_de_init(void);
unsigned int Check_Sleep_Timer_Timeout(void);
unsigned int Battery_check_Timer_Timeout(void);
unsigned int Check_UI_Timer_Timeout(void);
unsigned int Check_Button_Timer_Timeout(void);
extern uint8_t Short_Beep_Ring_Count;
extern unsigned char Processing_Display;
#define TEST_RESULT_UNKNOWN 0
#define TEST_RESULT_PASSED 1
#define TEST_RESULT_FAILED 2
void Processing_Display_timer_start(uint_32 time);
void Test_Gps(void);
void Test_Sd_card(void);
void gps_power_off(void);
void ui_Hwtimer_start(void);
void ui_Hwtimer_stop(void);
extern uint8_t Sensor_screan_timeout;

#endif /* UI_TIMER_H_ */

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
