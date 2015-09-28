/******************************************************************************
 *
 *     Filename:       $config.c
 *     Created on:     $Date: Mar 25, 2014    (10:17:33 AM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contains  user configuration functions for ADU device
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "config.h"
#include "algorithm.h"
#include "common_headers.h"

char Drive_Name[20] = {'\0'};
char Device_id[]  = "0000-0000-0001";
const char User_id[]	= "User_001";
const char Software_ver[]= "V-1.0.77";
unsigned char  Device_serial_no[6] ={'\0'};

uint_32 Adc_sample_rate = 2500;
uint_32 Adc_Sampling_Freq = 2500;
uint_32 TEST_COUNT = 12500;
uint_32 LSM_COUNT = 5000;
uint_8 	Test_period = 5, LSM_SAMPLE_COUNT = 5;
User_Settings Settings_Data;
Sensors Sensor_status;
CORE_CURRENT_CLOCK Core_clock_selection;

_task_id SDTask_ID;
_task_id BLE_Task_ID;
float K_factor = (1000 * 0.000019635);

/*-----------------------------------------------------------------------------
 *  Function:     Update_settings_Data
 *  Brief:        Updating different test parameters of the ADU device
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Update_settings_Data(void)
{
    
    Adc_sample_rate = Settings_Data.Sampling_Rate * 1000;
    Test_period = Settings_Data.Test_Time;
    LSM_SAMPLE_COUNT = Settings_Data.Compass_Samples;
    Adc_Sampling_Freq = Adc_sample_rate;
    TEST_COUNT = Adc_Sampling_Freq * Test_period;
    LSM_COUNT = TEST_COUNT / (LSM_SAMPLE_COUNT + 1);
    Lsm_Sample_Rate = 0;
    test_count = TEST_COUNT;
    set_adc_sample_Rate(Adc_Sampling_Freq);
    Kforce_5 = Settings_Data.Kforce5;
    Kforce_3 = Kforce_5 * (3.0 / 5.0);
    Default_graph_view_selection  = Settings_Data.default_gragh_view ; 
}

/*-----------------------------------------------------------------------------
 *  Function:     Default_Settings_Data
 *  Brief:        Updating default test parameters of the ADU device
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Default_Settings_Data(void)
{
    
    Settings_Data.Sampling_Rate = DEFAULT_SAMPLING_RATE;
    Settings_Data.QRDbreakV = DEFAULT_QRDBREAK_V;
    Settings_Data.Test_Time = DEFAULT_TEST_TIME;
    Settings_Data.Kforce5 = DEFAULT_K_FORCE5;
    Settings_Data.Calibration_Time = DEFAULT_CALIBERATION_TIME;
    Settings_Data.Compass_Samples = DEFAULT_COMPASS_SAMPLES;
    Settings_Data.QRDpercentLow = DEFAULT_QRDPERCENT_LOW;
    Settings_Data.QRDpercentHigh = DEFAULT_QRDPERCENT_HIGH;
    Settings_Data.battery_threshold = DEFAULT_BATTERY_THRESHOLD;
    Settings_Data.xfraction = DEFAULT_XFRACTION;
    Settings_Data.default_gragh_view= GRAPH_DETAIL_0;
    Settings_Data.Test_number = 0;
    Settings_Data.Dst_status  = DST_OFF;
    Settings_Data.Prev_Zone_List_selection = 49;	//US-MOUNTAIN TIME ZONE is the default time zone.
    Settings_Data.default_gragh_view = 0;
    Settings_Data.BLE_power_status = BLUETOOTH_OFF;
    Settings_Data.GPS_power_status = GPS_ON;
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/

Algorithm_Data *AlgorithmData		=(Algorithm_Data*)PROCESSING_INTERMEDIATE_DATA_LOCATION;
volatile uint_32 Acq_Data_Count		= 0;




/*-----------------------------------------------------------------------------
 *  Function:     Init_acq_buff
 *  Brief:        Initialising ACQ buffers.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Init_acq_buff(void)
{
	Acq_Data_Count	= 0;
	memset(AlgorithmData,0x00,sizeof(Algorithm_Data));
}

/*-----------------------------------------------------------------------------
 *  Function:     Init_acq_buff
 *  Brief:        Initialising ACQ buffers.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/

void Init_Sensor_status(void)
{
	
	Sensor_status.SDcard_status  = NOT_INITIALISED;
	Sensor_status.TMP_status	 = NOT_INITIALISED;
	Sensor_status.LSM_status_status	 = NOT_INITIALISED;
	Sensor_status.Battery_ADC_status	 = NOT_INITIALISED;
	Sensor_status.Aquisition_ADC_status = NOT_INITIALISED;
	Sensor_status.GPS__status	 = NOT_INITIALISED;

}




