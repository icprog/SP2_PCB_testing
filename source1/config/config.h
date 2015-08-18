/******************************************************************************
 *
 * 	Filename:       $config.h
 * 	Created on:     $Date: Nov 28, 2013	(11:23:40 AM)
 * 	Revision:       $1
 * 	Author:         $GadgEon
 *
 * 	Description:    $This file contains all user configuration for spi and gpio.
 *
 * 	Copyright (C) 2013 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/


#ifndef __config_h__
#define __config_h__


#include <fio.h>

#define QRD_THRESHOLD	(1.5)
#define SLEEP_TIME		(1)           
#define BAT_MULTIPLIER  (1.61)
#define SD_CARD_TASK    (4)
#define BLE_TASK		(5)
#define DDR_TASK		(7)

#define ENABLE_LSM303D							 (1)
#define ENABLE_LSM303DLHC						 (0)

#define BLE_DEFAULT_BAUD_RATE					 (115200)
#define BLE_CURRENT_BAUD_RATE					 (115200)
#define BLE_NEW_BAUD_RATE						 (115200)


/*Default test parameters of ADU device*/
#define DEFAULT_SAMPLING_RATE                    (2.5)
#define DEFAULT_TEST_TIME                        (5)
#define DEFAULT_K_FORCE5                         (0.30)
#define DEFAULT_CALIBERATION_TIME                (0.5)
#define DEFAULT_COMPASS_SAMPLES                  (3)
#define DEFAULT_QRDBREAK_V                       (0.5)
#define DEFAULT_QRDPERCENT_LOW                   (0.08)
#define DEFAULT_QRDPERCENT_HIGH                  (0.3)
#define DEFAULT_BATTERY_THRESHOLD                (2.75)
#define POWERUP_BATTERY_THRESHOLD                (2.80)
#define DEFAULT_XFRACTION						 (0.26)
#define GRAPH_DETAIL_0							 (0)
#define GRAPH_DETAIL_1							 (1)
#define GRAPH_DETAIL_2							 (2)
#define GRAPH_DETAIL_3							 (3)
#define GRAPH_DETAIL_4							 (4)

#define GRAPH_HEIGHT							 (300)
#define PROCESSING_INTERMEDIATE_DATA_LOCATION	 (0x70000000)
#define SETTINGS_FILE_NAME "a:__ADU_SETTINGS_V75.bin"
#define DFU_CHECK_FILE_NAME "a:__DFU_1A2B3C4.bin"
#define LCD_BAUD_AT_ACQUISITION					 (4000000)
#define LCD_BAUD_AT_IDLE						 (4000000)

#define IDLE_DISPLAY_OFF_TIMEOUT 60000
#define IDLE_KILL_TIMEOUT 300000
#define GPS_OFF_TIMEOUT   180000
#define GPS_ON_TIMEOUT    180000

#define BATTERY_FULL_VOLTAGE 2.9 		// 2.8V and above is considered as Battery Full
#define BATTERY_HALF_FULL_VOLTAGE 2.8	// 2.6V-2.8 is considered as battery half full

#define NOT_INITIALISED							 (0)
#define INITIALISED								 (1)

/*Global variables*/
extern uint32_t 	Adc_sample_rate;      //ADC sample rate in Hz
extern uint8_t Test_period,	LSM_SAMPLE_COUNT;		
extern uint16_t Pressure_averageSize;
extern uint_32  LSM_COUNT; 
extern char Drive_Name[];
extern char Device_id[];  
extern unsigned char  Device_serial_no[];
extern const char User_id[];	
extern const char Software_ver[];
extern _task_id SDTask_ID;
extern _task_id BLE_Task_ID;
extern volatile uint_32 ticks,Lsm_Sample_Rate;
extern long int test_count;
extern float Kforce_3,Kforce_5;
typedef struct settings
{
	volatile float Sampling_Rate;
	volatile float QRDbreakV;
	volatile uint_8 Test_Time;
	volatile float Kforce5;
	volatile float Calibration_Time;
	volatile uint_8 Compass_Samples;
	volatile float QRDpercentLow;
	volatile float QRDpercentHigh;
	volatile float battery_threshold;
	volatile float xfraction;
	volatile uint_8 default_gragh_view;	
	volatile uint_32 Test_number;
	volatile uint_8 Dst_status;
	volatile uint16_t Prev_Zone_List_selection;
	volatile uint_8 BLE_power_status;
	volatile uint_8 GPS_power_status;
	
} User_Settings;

typedef struct Acquisition
{
	volatile uint16_t Force_Data[1];
	volatile uint16_t Distance_Data[1];
	volatile uint16_t QRD_Data[1];
	volatile uint16_t Smooth_Force_Data[1];
	volatile uint16_t IRDMS_mov_min[1];
	volatile uint16_t IRDMS_Smooth[1];
	volatile uint16_t Smooth_QRD[1];
	volatile uint_32 Acq_Count;
	
} Acquisition_Data;

typedef struct Sensor
{
	volatile uint_8 SDcard_status;
	volatile uint_8 TMP_status;
	volatile uint_8 LSM_status_status;
	volatile uint_8 Battery_ADC_status;
	volatile uint_8 Aquisition_ADC_status;
	volatile uint_8 GPS__status;
//	volatile uint_8 SDcard_status;
//	volatile uint_8 SDcard_status;
	
}Sensors;


typedef enum
{
	LOW_POWER_MODE,
	IDLE_MODE_MODE,
	ACQUISITION_MODE,
}CORE_CURRENT_CLOCK;

extern CORE_CURRENT_CLOCK Core_clock_selection;
extern User_Settings Settings_Data;
extern Sensors Sensor_status;

/*function prototype*/
extern void Default_Settings_Data(void);
extern void Update_settings_Data(void);
extern float K_factor;

//extern volatile uint16_t *Force_buff; 
//extern volatile uint16_t *Distance_buff;
//extern volatile uint16_t *Qrd_buff;
//
//extern volatile uint16_t *Smooth_Force_Data;
//extern volatile float 	*V_IRDMS_shift;
//extern volatile float 	*IRDMS_Smooth;
//extern volatile uint16_t *Smooth_QRD;
extern volatile uint_32 Acq_Data_Count;
//extern volatile float 	*Processed_Data;

extern void Init_acq_buff(void);

#endif
