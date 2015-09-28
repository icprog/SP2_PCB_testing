/******************************************************************************
 *
 *   Filename: $data_acquisition.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of start and stop of data acquisition
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include "lsm303.h"
#include "vector.h"
#include "tmp006.h"
#include "data_acquisition.h"
#include "Hw_Timer.h"
#include "HAL_CC4000.h"
#include "HAL_CC4000_NMEA.h"
#include "config.h"
#include "Sd_Card.h"
#include "Hw_Init.h"
#include "Operation Modes/operation_modes.h"
#include "mqx_prv_new.h"


volatile uchar Test_Complete_flag = 0, Acq_Complete_flag = 0;

/*-----------------------------------------------------------------------------* 
 * Function:    stop_data_acquision
 * Brief:       This function will stop the collection of data from different sensors
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void stop_data_acquision(void)
{
    
    
//    Lsm_Sample_Rate += LSM_COUNT;
//    Read_LSM = 1;
//    if (lsm_data_count < MAX_LSM_SAMPLE_COUNT)
//    {
////        Data_Write_file.LSM_MeterData[lsm_data_count].Tick_Count = ticks;
//    }
//    ticks = 0;
//    Lsm_Sample_Rate = LSM_COUNT;
    BUSY = FALSE;
//    Stop_ADC();
    Acq_Complete_flag = 1;
}
/*-----------------------------------------------------------------------------* 
 * Function:    stop_data_acquision
 * Brief:       This function will start the collection of data from different sensors
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/

void start_data_acquision(void)
{
    SD_Write_Cnt = 0;
    Test_Complete_flag = 0;
    lsm_data_count = 0;
    Get_timestamp_from_rtc();
    ticks = 0;
    clear_lsm_data_buffer();
    
    
    Settings_Data.Test_number++;
    
    /* Reads temp before test..*/
    Tmp006_Read(BEFORE_TEST);

//    Start_ADC();
    BUSY = TRUE;

    ui_timer_start((uint_32) Settings_Data.Calibration_Time * 1000);
    
    Start_Hw_Timer_1();
    
    lwgpio_int_enable(&Lsm303_TMP_pin, TRUE );
}

/*-----------------------------------------------------------------------------* 
 * Function:    print_collected_data_buffer
 * Brief:       This will print some of collected data via debug console
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void print_collected_data_buffer(void)
{
   // uchar k = 0;
    
    printf("\n***************TMP006****************");
    printf("\n\nAmbient Temperature before test :%d",
	(int) Data_header.Temperature.Ambient_Temperature_Before_Test);
    printf("\nObject Temperature before test    :%d",
	(int) Data_header.Temperature.Object_Temperature_Before_Test);
    printf("\n\n***************LSM303****************\n");
//    for (k = 0; k < LSM_SAMPLE_COUNT; k++)
//    {
//        printf("\nAccelerometer                     :%d",
//        		Data_Write_file.LSM_MeterData[k].AX_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].AY_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].AZ_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].MX_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].MY_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].MZ_data);
//        printf("\t%d", Data_Write_file.LSM_MeterData[k].Tick_Count);
//	}
    
    printf("\n\n***************GPS****************\n");
    printf("\nTime stamp                         :%s   %s", Data_header.Time_stamp.Date,
    		Data_header.Time_stamp.Time);
    printf("\nLattitude                          :%s", Data_header.Gps_data.Latitude);
    printf("\nLongitude                          :%s", Data_header.Gps_data.Longitude);
    
    printf("\n***************TMP006****************");
    
    printf("\nAmbient Temperature After test    :%d",
            (int) Data_header.Temperature.Ambient_Temperature_After_Test);
    
    printf("\nObject Temperature After test      :%d",
            (int) Data_header.Temperature.Object_Temperature_After_Test);
}


/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
