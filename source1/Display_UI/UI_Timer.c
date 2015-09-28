/******************************************************************************
 *
 *   Filename: $UI_Timer.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains timer 2 related functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "common_headers.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

static void hwtimer2a_callback(pointer p);

#define HWTIMER2_PERIOD     (1000)     //period set to 1ms to hwtimer 2
#define UI_TIMER_TIME_INTERVELL 4		//in ms
extern HWTIMER_DEVIF_STRUCT BSP_HWTIMER1_DEV;  //device struct

HWTIMER hwtimer2a;                       //hwtimer handle

static volatile uint_32 Sleep_timer_count = (1 * 60000 * UI_TIMER_TIME_INTERVELL);
static volatile uint_32 Battery_check_timer_count = (1 * 5000 * UI_TIMER_TIME_INTERVELL);

volatile uint_32 UI_timer_count = 0;
static volatile uint_32 Long_Beep_timer_count = 0;
static volatile uint_32 Short_Beep_timer_count = 0,Beep_Restart_timer_count=0;
static volatile uint_32 Button_bounce_timer_count = 0;
uint8_t Short_Beep_Ring_Count = 0;
static volatile uint_32 GPS_ON_timer_count = 0;
volatile uint_32 GPS_OFF_timer_count = 0;
static volatile uint_32 GPS_Lock_timer_count = 0,UI_Timer_Processing=0;
static volatile uint_32 BLE_HS_timer_count = 0;
static volatile uint_32 Sensor_screan_timer = 0;
uint8_t Sensor_screan_timeout = 0;

unsigned char Processing_Display=0;
#define BEEP_INTERVAL_1 (400) 
#define BEEP_INTERVAL_2 (600) 

#define BEEP_FIRST_LIMIT   (BEEP_INTERVAL_1/UI_TIMER_TIME_INTERVELL)
#define BEEP_SECOND_LIMIT  (BEEP_INTERVAL_2/UI_TIMER_TIME_INTERVELL)


/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_de_init
 * Brief:       De init UI timer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_Hwtimer_start(void)
{
    hwtimer_start(&hwtimer2a);

}

/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_de_init
 * Brief:       De init UI timer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_Hwtimer_stop(void)
{
    hwtimer_stop(&hwtimer2a);
}

/*-----------------------------------------------------------------------------* 
 * Function:    Check_Button_Timer_Timeout
 * Brief:       Check weather timeout occurred or not
 * Parameter:   None
 * Return:      Timeout status
 -----------------------------------------------------------------------------*/
unsigned int Check_Button_Timer_Timeout(void)
{
    if (Button_bounce_timer_count == 0x00)
    {
        return TIME_OUT;
    }
    return NOT_TIMEOUT;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Check_UI_Timer_Timeout
 * Brief:       Check weather timeout occurred or not
 * Parameter:   None
 * Return:      Timeout status
 -----------------------------------------------------------------------------*/
unsigned int Check_UI_Timer_Timeout(void)
{
    if (UI_timer_count == 0x00)
    {
        return TIME_OUT;
    }
    return NOT_TIMEOUT;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Battery_check_Timer_Timeout
 * Brief:       Check weather timeout occurred or not
 * Parameter:   None
 * Return:      Timeout status
 -----------------------------------------------------------------------------*/
unsigned int Battery_check_Timer_Timeout(void)
{
    if (Battery_check_timer_count == 0x00)
    {
        return TIME_OUT;
    }
    return NOT_TIMEOUT;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Check_Sleep_Timer_Timeout
 * Brief:       Check weather timeout occurred or not
 * Parameter:   None
 * Return:      Timeout status
 -----------------------------------------------------------------------------*/
unsigned int Check_Sleep_Timer_Timeout(void)
{
    
    if (Sleep_timer_count == 0x00)
    {
        return TIME_OUT;
    }
    return NOT_TIMEOUT;
}

void Short_beep_Restart_timer(uint_32 time)
{
	Beep_Restart_timer_count = (time * UI_TIMER_TIME_INTERVELL);
}

/*-----------------------------------------------------------------------------* 
 * Function:    hwtimer2a_callback
 * Brief:       This isr will do operations like decrementing timeout counts
 * Parameter:   pointer from isr
 * Return:      None
 -----------------------------------------------------------------------------*/
static void hwtimer2a_callback(pointer )
{
    
    if (GPS_ON_timer_count != 0x00)
    {
    	GPS_ON_timer_count--;
    }
    if((GPS_Current_State == GPS_POWER_OFF)&&(GPS_ON_timer_count == 0x00)&&(Settings_Data.GPS_power_status == GPS_ON))
    {
		GPS_OFF_timer_start(180000);	// 3 min
		GPS_Current_State = GPS_POWER_ON;
		gpsCC4000OStartFixes(); 
		/* enable interrupt on GPIO peripheral module*/
		lwgpio_int_enable(&FIX_AVL, TRUE );
    }
    if (GPS_OFF_timer_count != 0x00)
    {
    	GPS_OFF_timer_count--;
    }
    
    if (GPS_Lock_timer_count != 0x00)
    {
    	GPS_Lock_timer_count--;
    }
    if((GPS_Current_State == GPS_LOCKING)&&(GPS_Lock_timer_count == 0x00))
    {
    	// disable pin isr
    	// disable locking timer
		/* disable interrupt on GPIO peripheral module*/
//		lwgpio_int_enable(&FIX_AVL, FALSE );
    	GPS_Current_State = GPS_LOCKED;
    }
    if (Button_bounce_timer_count != 0x00)
    {
        Button_bounce_timer_count--;
    }
    
    if (Battery_check_timer_count != 0x00)
    {
        Battery_check_timer_count--;
    }
    
    if (Sleep_timer_count != 0x00)
    {
        Sleep_timer_count--;
    }
    
    if (UI_timer_count != 0x00)
    {
        UI_timer_count--;
    }
    if(UI_Timer_Processing!=0x00)
    {
    	UI_Timer_Processing--;
    }
    else if ((UI_Timer_Processing==0x00)&&(Processing_Display==1))
    {
#if AUTOMATE_TEST
    	Processing_Display_timer_start(200);
#else
    	Processing_Display_timer_start(1000);
#endif
    	_lwevent_set(&app_event, SW_EVENT_MASK);
    }
    
    if(Beep_Restart_timer_count!=0x00)
    {
    	Beep_Restart_timer_count--;
    	if(Beep_Restart_timer_count==0x00)
    	{
    		Short_beep_timer_start(SHORT_BEEP_TIME); // time in millisec
    	}
    }
    
    if (Short_Beep_timer_count != 0x00)
    {
        Short_Beep_timer_count--;
        Buzzer_toggle();
        if (Short_Beep_timer_count == 0x00)
        {
        	if(Short_Beep_Ring_Count==0)
        	{
        		Buzzer_Off();
        	}
        	else
        	{
        		Short_Beep_Ring_Count--;
        		Buzzer_Off();
        		Short_beep_Restart_timer(SHORT_BEEP_OFF_TIME); // time in millisec
        	}
        }
    }
    
    if (Long_Beep_timer_count != 0x00)
    {
        Long_Beep_timer_count--;
        if ((Long_Beep_timer_count < 1600)||(Long_Beep_timer_count>2400))
        {
        	Buzzer_toggle();
		}
		else
		{   
			Buzzer_Off();
		}   
        if (Long_Beep_timer_count == 0x00)
        {
            Buzzer_Off();
        }
    }
    
    if(BLE_HS_timer_count != 0x00)
    {
    	BLE_HS_timer_count--;
    }
    else
    {
    	Ble_wakeup = YES;
    }
    
    if(Sensor_screan_timer != 0)
    {
    	Sensor_screan_timer--;   	
    	if(Sensor_screan_timer == 0)
    	{
    		Sensor_screan_timeout = 1;
    	}
    	else
    	{
    		Sensor_screan_timeout = 0;
    	}
    	
    }
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_de_init
 * Brief:       De init UI timer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_timer_de_init(void)
{
    hwtimer_stop(&hwtimer2a);
    /* Deinitialization of hwtimers */
    hwtimer_deinit(&hwtimer2a);
}
/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_init
 * Brief:       Init UI timer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_timer_init(void)
{
    /* Initialization of hwtimer2a*/
    if (MQX_OK != hwtimer_init(&hwtimer2a, &BSP_HWTIMER2_DEV, BSP_HWTIMER2_ID, 1))
    {
        printf("Timer FAILED!\n");
    }
    
    hwtimer_set_period(&hwtimer2a, BSP_HWTIMER2_SOURCE_CLK,
            (HWTIMER2_PERIOD/UI_TIMER_TIME_INTERVELL));
    hwtimer_callback_reg(&hwtimer2a, (HWTIMER_CALLBACK_FPTR) hwtimer2a_callback, NULL);
    /* Start hwtimer1a*/
    hwtimer_start(&hwtimer2a);
}

/*-----------------------------------------------------------------------------* 
 * Function:    Short_beep_timer_start
 * Brief:       Start timer for short beep
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void Short_beep_timer_start(uint_32 time)                         // time in millisec
{
    
    Short_Beep_timer_count = 1600;//(time / UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Short_beep_timer_stop
 * Brief:       Stop timer for short beep
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Short_beep_timer_stop(void)
{
    Short_Beep_timer_count = 0x00;
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Long_beep_timer_start
 * Brief:       Start timer for long beep
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void Long_beep_timer_start(uint_32 time)                         // time in millisec
{
    
    Long_Beep_timer_count = 4000;//(time / UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Long_beep_timer_stop
 * Brief:       Stop timer for long beep
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Long_beep_timer_stop(void)
{
    Long_Beep_timer_count = 0x00;
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Battery_check_timer_start
 * Brief:       Start timer for battery check
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void Battery_check_timer_start(uint_32 time)                         // time in millisec
{
    
    Battery_check_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Battery_check_timer_stop
 * Brief:       Stop timer for battery check
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Battery_check_timer_stop(void)
{
    Battery_check_timer_count = 0x00;
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_start
 * Brief:       Start timer for ui display
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_timer_start(uint_32 time)                         // time in millisec
{
    
    UI_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    ui_timer_stop
 * Brief:       Stop timer for display
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ui_timer_stop(void)
{
    UI_timer_count = 0x00;
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Sleep_timer_start
 * Brief:       Start timer for sleep time
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void Sleep_timer_start(uint_32 time)                         // time in millisec
{
    
    Sleep_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Sleep_timer_stop
 * Brief:       Stop timer for sleep
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Sleep_timer_stop(void)
{
    Sleep_timer_count = 0x00;
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Button_bounce_timer_start
 * Brief:       Start timer for button bounce
 * Parameter:   time in millisecond
 * Return:      None
 -----------------------------------------------------------------------------*/
void Button_bounce_timer_start(uint_32 time)                         // time in millisec
{
    
    Button_bounce_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Button_bounce_timer_stop
 * Brief:       Stop timer for button bounce
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Button_bounce_timer_stop(void)
{
    Button_bounce_timer_count = 0x00;
    
}


void GPS_ON_timer_start(uint_32 time)                         // time in millisec
{
    
	GPS_ON_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Button_bounce_timer_stop
 * Brief:       Stop timer for button bounce
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void GPS_ON_timer_stop(void)
{
	GPS_ON_timer_count = 0x00;
    
}

void GPS_OFF_timer_start(uint_32 time)                         // time in millisec
{
    
	GPS_OFF_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Button_bounce_timer_stop
 * Brief:       Stop timer for button bounce
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void GPS_OFF_timer_stop(void)
{
	GPS_OFF_timer_count = 0x00;
    
}

void GPS_Lock_timer_start(uint_32 time)                         // time in millisec
{
    
	GPS_Lock_timer_count = (time * UI_TIMER_TIME_INTERVELL);
    
}
/*-----------------------------------------------------------------------------* 
 * Function:    Button_bounce_timer_stop
 * Brief:       Stop timer for button bounce
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void GPS_Lock_timer_stop(void)
{
	GPS_Lock_timer_count = 0x00;
    
}

void Processing_Display_timer_start(uint_32 time)                         // time in millisec
{
    
	UI_Timer_Processing = (time * UI_TIMER_TIME_INTERVELL);
    
}

/*-----------------------------------------------------------------------------* 
 * Function:    BLE_hand_shake_timer
 * Brief:       BLE_hand_shake_timer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void BLE_hand_shake_timer_start(uint_32 time)
{
	
	BLE_HS_timer_count = (time * UI_TIMER_TIME_INTERVELL);
}


/*-----------------------------------------------------------------------------* 
 * Function:    Start_sensor_output_screen_timer.
 * Brief:       Start sensor output screen timer for button press.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
Start_sensor_output_screen_timer(uint_32 time)
{	
	Sensor_screan_timeout = 0;
	Sensor_screan_timer = (time * UI_TIMER_TIME_INTERVELL);
}
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
