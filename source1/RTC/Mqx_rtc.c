/******************************************************************************
 *
 *     Filename:       $Mqx_rtc.c
 *     Created on:     $Date: Mar 25, 2014    (11:00:17 AM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file includes function Read and write to RTC time register
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "rtc.h"
#include "Mqx_rtc.h"
#include "HAL_CC4000.h"
#include "HAL_CC4000_NMEA.h"
#include "UI_Display.h"
#include "UI_Display_Time.h"
#include "Sd_Card.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_RTCDEV
#error This application requires BSPCFG_ENABLE_RTCDEV defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
/* For tamper event functionality on MCF51EMxx device */
#if PSP_MQX_CPU_IS_MCF51EM
static void test_tamper (void);
static void print_tamper_event(RTC_TAMPER_TYPE tamper_status, RTC_TIME_STRUCT_PTR tamper_time_ptr);
#endif /* PSP_MQX_CPU_IS_MCF51EM */
#if PSP_HAS_IRTC == 1
static void irtc_test(void);
#endif
#if BSPCFG_ENABLE_LCD
extern void lcd_task(uint_32);
#endif

#if PSP_HAS_IRTC == 1
static const uint_8 backup_data[] = "Backup Data";
static uint_8 read_data[sizeof(backup_data)];
#endif
char update_rtc_from_gps_status = 0;

/*-----------------------------------------------------------------------------
 *  Function:     update_rtc_from_gps
 *  Brief:        Update RTC time and date from GPS data
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Update_rtc_from_gps(void)
{
    char temp_date[16], temp_time[16];
    getRMCGPSDate(temp_date, 16);
    if (strlen(temp_date) == 6)
    {
        getRMCGPSUTCTime(temp_time, 16);
        if (strlen(temp_time) == 9)
        {
            
            Update_rtc(temp_time, temp_date);
            Get_timestamp_from_rtc();
        }
        
    }
    set_absolute_time();	//Update the Date and time from RTC to MQX timer 
    
}

/*-----------------------------------------------------------------------------
 *  Function:     get_timestamp_from_rtc
 *  Brief:        Read timestamp from RTC
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Get_timestamp_from_rtc(void)
{
    
    DATE_STRUCT temp_timestamp;
    _rtc_get_time_mqxd(&temp_timestamp);
    snprintf((char *)Data_header.Time_stamp.Date,15, "%02d%02d%02d", temp_timestamp.YEAR, temp_timestamp.MONTH,
    		temp_timestamp.DAY);
    snprintf((char *)Data_header.Time_stamp.Time,15, "%02d%02d%02d", temp_timestamp.HOUR,
            temp_timestamp.MINUTE, temp_timestamp.SECOND);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     rtc_init
 *  Brief:        Initialise the RTC 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Rtc_init(void)
{
    if (MQX_OK != _rtc_init(RTC_INIT_FLAG_CLEAR | RTC_INIT_FLAG_RESET | RTC_INIT_FLAG_ENABLE))
    {
        printf("\nRTC Init Failed..!!");
    }
}

void Set_Time_Zone(int32_t Offset)
{
	RTC_TIME_STRUCT time;
	// Reading time in sec
	_rtc_get_time (&time);
	// setting new time
	time.seconds = (time.seconds+Offset);	
	_rtc_set_time(&time);
	update_system_date_time_from_rtc();
	set_absolute_time();
}

/*-----------------------------------------------------------------------------
 *  Function:     update_rtc
 *  Brief:        Update RTC Time and Date
 *  Parameter:    Address of Time and Date variables 
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Update_rtc(char *time, char *date)
{
    uint_16 temp_day = 0, temp_month = 0, temp_year = 0, temp_hour = 0, temp_minute = 0, temp_sec =
            0;
    DATE_STRUCT Temp_rtc_buffer = { 0 };
    
    Temp_rtc_buffer.HOUR = (uint_16) (((uint_16) (time[0] - 48) * 10) + (uint_16) (time[1] - 48));
    
    Temp_rtc_buffer.MINUTE = (uint_16) (((uint_16) (time[2] - 48) * 10) + (uint_16) (time[3] - 48));
    
    Temp_rtc_buffer.SECOND = (uint_16) (((uint_16) (time[4] - 48) * 10) + (uint_16) (time[5] - 48));
    
    Temp_rtc_buffer.MILLISEC =
            (uint_16) (((uint_16) (time[7] - 48) * 10) + (uint_16) (time[8] - 48));
    
    Temp_rtc_buffer.DAY = (uint_16) (((uint_16) (date[0] - 48) * 10) + (uint_16) (date[1] - 48));
    
    Temp_rtc_buffer.MONTH = (uint_16) (((uint_16) (date[2] - 48) * 10) + (uint_16) (date[3] - 48));
    
    Temp_rtc_buffer.YEAR = (uint_16) ((((uint_16) (date[4] - 48) * 10) + (uint_16) (date[5] - 48))
            + 2000);

//    printf("\nTime: %d %d %d\n", Temp_rtc_buffer.HOUR, Temp_rtc_buffer.MINUTE,
//            Temp_rtc_buffer.SECOND);
//    printf("\nDate: %d %d %d\n", Temp_rtc_buffer.DAY, Temp_rtc_buffer.MONTH, Temp_rtc_buffer.YEAR);
    _rtc_set_time_mqxd(&Temp_rtc_buffer);
    set_absolute_time();	//Update the Date and time from RTC to MQX timer 
    
}

/*-----------------------------------------------------------------------------
 *  Function:     print_rtc_time_date
 *  Brief:        Print the current RTC time and Date
 *  Parameter:    Address of Time and Date variables 
 *  Return:       None
 -----------------------------------------------------------------------------*/
void print_rtc_time_date(void)
{
    DATE_STRUCT time_rtc;
    _rtc_get_time_mqxd(&time_rtc);
//    printf("% 02d:%02d\n", time_rtc.MINUTE, time_rtc.SECOND);
    printf("RTC : %02d.%02d.%4d %02d:%02d:%02d\n", time_rtc.DAY, time_rtc.MONTH, time_rtc.YEAR,
            time_rtc.HOUR, time_rtc.MINUTE, time_rtc.SECOND);
    
}

/*FUNCTION**********************************************************************
 *
 * Function Name    : Get_lock_time
 * Returned Value   : gps lock time
 * Comments         : This function returns lock time for GPS
 *
 *END**************************************************************************/
uint_16 Get_lock_time(void)
{
    uint_16 lock_time=0;
    DATE_STRUCT temp_timestamp;
    
    /* read  RTC lock time */
    _rtc_get_time_mqxd(&temp_timestamp);
    
    lock_time = temp_timestamp.HOUR *3600;
    lock_time = lock_time + temp_timestamp.MINUTE *60;
    lock_time = lock_time + temp_timestamp.SECOND;
    
    return lock_time;
    
}


/*-----------------------------------------------------------------------------
 *  Function:     update_system_date_time_from_rtc
 *  Brief:        Update System Date and Time in UI Screen from RTC
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void update_system_date_time_from_rtc(void)
{
    DATE_STRUCT temp_timestamp;
    _rtc_get_time_mqxd(&temp_timestamp);

    Settings_Month_Count 	 = temp_timestamp.MONTH;
    Settings_Date_Count	 	 = temp_timestamp.DAY;
    Settings_Year_Count 	 = temp_timestamp.YEAR%100;
    Settings_Hours_Count 	 = temp_timestamp.HOUR;
    Settings_Minutes_Count	 = temp_timestamp.MINUTE;
    Settings_Seconds_Count   = temp_timestamp.SECOND;
    if(temp_timestamp.HOUR >= 12)
    {
    	Settings_AM_PM_Count = 1;
    }
    else
    {
    	Settings_AM_PM_Count = 0;    
    }
    
    if(Time_format == TWELVE_HR_FORMAT)
    {
        if(Settings_Hours_Count > 12)
        {
    		Settings_Hours_Count= (Settings_Hours_Count +12)%24;
        }
        if((Settings_Hours_Count == 0) && (Settings_AM_PM_Count == TIME_AM))
        {
    		Settings_Hours_Count= 12;
        }
    }

}

/*-----------------------------------------------------------------------------
 *  Function:     update_system_date_from_rtc
 *  Brief:        Update System Date in UI Screen from RTC
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void update_system_date_from_rtc(void)
{
    DATE_STRUCT temp_timestamp;
    _rtc_get_time_mqxd(&temp_timestamp);

    Settings_Month_Count 	 = temp_timestamp.MONTH;
    Settings_Date_Count	 	 = temp_timestamp.DAY;
    Settings_Year_Count 	 = temp_timestamp.YEAR%100;
}

/*-----------------------------------------------------------------------------
 *  Function:     update_system_time_from_rtc
 *  Brief:        Update System Date in UI Screen from RTC
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void update_system_time_from_rtc(void)
{
    DATE_STRUCT temp_timestamp;
    _rtc_get_time_mqxd(&temp_timestamp);

    Settings_Hours_Count 	 = temp_timestamp.HOUR;
    Settings_Minutes_Count	 = temp_timestamp.MINUTE;
    Settings_Seconds_Count   = temp_timestamp.SECOND;
    if(temp_timestamp.HOUR >= 12)
    {
    	Settings_AM_PM_Count = 1;
    }
    else
    {
    	Settings_AM_PM_Count = 0;    
    }
    
    if(Time_format == TWELVE_HR_FORMAT)
    {
        if(Settings_Hours_Count > 12)
        {
    		Settings_Hours_Count= (Settings_Hours_Count +12)%24;
        }
        if((Settings_Hours_Count == 0) && (Settings_AM_PM_Count == TIME_AM))
        {
    		Settings_Hours_Count= 12;
        }
    }

}

/*-----------------------------------------------------------------------------
 *  Function:     set_absolute_time
 *  Brief:        This sets the absolute MQX time, which is the time used to create files in SD card etc etc
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void set_absolute_time(void)
{
    TIME_STRUCT mqx_time;
    DATE_STRUCT date;
    _rtc_get_time_mqxd(&date);		//Fill the Date structure with date and time from RTC
    _time_from_date(&date, &mqx_time);	//Convert the RTC date and time to seconds format in TIME_STRUCT
    _time_set(&mqx_time);			//Set the MQX time in seconds.
    printf("\n#######SETTING MQX TIME in Update RTC Seconds = %u #######\n",mqx_time.SECONDS);  
}



/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
