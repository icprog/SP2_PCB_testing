/******************************************************************************
 *
 *   Filename: $Buzzer_Function.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains codes for handling buzzer
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "common_headers.h"

LWGPIO_STRUCT Buzzer_Output;
#define BUZZER_OUTPUT				 (GPIO_PORT_A | GPIO_PIN12)
#define BUZZER_OUTPUT_MUX_IRQ 		 (LWGPIO_MUX_A12_GPIO)
#define LONG_BEEP_TIME                1000

//PTA12

void Buzzer_Init(void)
{
    /* opening pins for output */
    if (!lwgpio_init(&Buzzer_Output, BUZZER_OUTPUT, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW))
    {
        printf("Initializing button GPIO as output failed.\n");
        
    }
    // for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&Buzzer_Output, BUZZER_OUTPUT_MUX_IRQ);
    
}

void Buzzer_toggle(void)
{
    lwgpio_toggle_pin_output(BUZZER_OUTPUT);
}

void Buzzer_On(void)
{
    lwgpio_set_pin_output(BUZZER_OUTPUT, LWGPIO_VALUE_HIGH);
    
}

void Buzzer_Off(void)
{
    lwgpio_set_pin_output(BUZZER_OUTPUT, LWGPIO_VALUE_LOW);
    
}

void Buzzer_Short_Beep(uint8_t count)
{
//	Buzzer_On();
    Short_beep_timer_start(SHORT_BEEP_TIME); // time in millisec
    Short_Beep_Ring_Count = count;
}

void Buzzer_Long_Beep(void)
{
//	Buzzer_On();
    Long_beep_timer_start(LONG_BEEP_TIME);
}

void Test_Buzzer(void)
{
	printf("\n***********STARTING BUZZER TEST***********\n");
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BUZZER TEST",strlen("BUZZER TEST"));
	_time_delay(1000);
	Draw_string_new(10,80,"STARTING BUZZER TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	
//	ui_timer_init();
	Buzzer_Init();
	Buzzer_On();
	printf("\n Test 1: Buzzer Short Beep.");
	Buzzer_Short_Beep(0);
	_time_delay(2000);
	printf("\n Test 2: Buzzer Long Beep.");
	Buzzer_Long_Beep();
	_time_delay(2000);
	Buzzer_Off();
//	ui_timer_de_init();
	Draw_string_new(10,200,"BUZZER TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
	printf("\n***********COMPLETED BUZZER TEST***********\n");
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(1000);
}

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
