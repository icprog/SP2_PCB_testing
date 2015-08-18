/******************************************************************************
 *
 *   Filename: $UI_Settings.h
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains header information of settings menu functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#ifndef UI_SETTINGS_H_
#define UI_SETTINGS_H_

#define MINIMUM_SAMPLING_RATE 1
#define MAXIMUM_SAMPLING_RATE 10
#define SAMPLING_RATE_STEP 1

#define MINIMUM_TEST_TIME 1
#define MAXIMUM_TEST_TIME 15
#define TEST_TIME_STEP 1

#define MINIMUM_K_FORCE5 0.01
#define MAXIMUM_K_FORCE5 1.0	//instead of 0.350 for helping floating comparison
#define K_FORCE5_STEP 0.01

#define MINIMUM_QRDBREAK_V 0
#define MAXIMUM_QRDBREAK_V   3.5   //instead of 3  ;for helping floating comparison
#define QRDBREAK_V_STEP 0.05

#define MINIMUM_CALIBERATION_TIME 0
#define MAXIMUM_CALIBERATION_TIME 1.5              //instead of 1.0 for helping floating comparison
#define CALIBERATION_TIME_STEP 0.1

#define MINIMUM_COMPASS_SAMPLES 1
#define MAXIMUM_COMPASS_SAMPLES 10
#define COMPASS_SAMPLES_STEP 1

#define MINIMUM_QRDPERCENT_LOW                  0
#define MAXIMUM_QRDPERCENT_LOW                  0.5  //instead of 0.350 for helping floating comparison
#define QRDPERCENT_LOW_CALIBERATION_STEP        0.01

#define MINIMUM_QRDPERCENT_HIGH                 0
#define MAXIMUM_QRDPERCENT_HIGH                 0.95	//instead of 0.350 for helping floating comparison
#define QRDPERCENT_HIGH_CALIBERATION_STEP       0.05

#define MINIMUM_BATTERY_THRESHOLD               2.75
#define MAXIMUM_BATTERY_THRESHOLD               2.75	//instead of 0.350 for helping floating comparison
#define BATTERY_THRESHOLD_CALIBERATION_STEP     0.1

#define MINIMUM_XFRACTION_VALUE	                0.02
#define MAXIMUM_XFRACTION_VALUE	                0.98	//instead of 0.350 for helping floating comparison
#define XFRACTION_VALUE_CALIBERATION_STEP       0.02

#define SELECTION_SAMPLING_RATE 				0
#define SELECTION_OPTICAL_TRIGGER_THRESHOLD 	1
#define SELECTION_TEST_TIME						2
#define SELECTION_FORCE_SENSOR_CALIBERATION		3
#define SELECTION_CALIBERATION_TIME				4
#define SELECTION_COMPASS_SAMPLES				5
#define SELECTION_QRDPERCENT_LOW	            6
#define SELECTION_QRDPERCENT_HIGH	            7
#define SELECTION_BATTERY_THRESHOLD	            8
#define SELECTION_XFRACTION_VALUE	            9

uint_8 increment_Sample_Rate(void);
uint_8 decrement_Sample_Rate(void);
float increment_Optical_Trigger_Threshold(void);
float decrement_Optical_Trigger_Threshold(void);
uint_8 increment_Test_Time(void);
uint_8 decrement_Test_Time(void);
float increment_Force_Sensor_Calibration(void);
float decrement_Force_Sensor_Calibration(void);
float increment_Calibration_Time(void);
float decrement_Calibration_Time(void);
uint_8 increment_Compass_Samples(void);
uint_8 decrement_Compass_Samples(void);
float increment_QRDpercentlow(void);
float decrement_QRDpercentlow(void);
float increment_QRDpercentHigh(void);
float decrement_QRDpercentHigh(void);
float increment_battery_threshold(void);
float decrement_battery_threshold(void);
float increment_xfraction_value(void);
float decrement_xfraction_value(void);


#endif /* UI_SETTINGS_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
