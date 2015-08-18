/******************************************************************************
 *
 *   Filename: $UI_Settings.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of settings menu functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include "bitmap.h"
#include "spi_lcd.h"
#include "config.h"
#include "UI_Display.h"
#include "UI_task.h"
#include "Button_Layout.h"
#include "UI_Timer.h"
#include "UI_Settings.h"

/*-----------------------------------------------------------------------------* 
 * Function:    increment_Sample_Rate
 * Brief:       increment Sample Rate for adc data by one step
 * Parameter:   None
 * Return:      new sampling rate
 -----------------------------------------------------------------------------*/
uint_8 increment_Sample_Rate(void)
{
	if (Settings_Data.Sampling_Rate < MAXIMUM_SAMPLING_RATE)
		Settings_Data.Sampling_Rate += SAMPLING_RATE_STEP;

	if (Settings_Data.Sampling_Rate > MAXIMUM_SAMPLING_RATE)
		Settings_Data.Sampling_Rate = MAXIMUM_SAMPLING_RATE;

	return Settings_Data.Sampling_Rate;
}
/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Sample_Rate
 * Brief:       decrement Sample Rate for adc data by one step
 * Parameter:   None
 * Return:      new sampling rate
 -----------------------------------------------------------------------------*/
uint_8 decrement_Sample_Rate(void)
{
	if (Settings_Data.Sampling_Rate > MINIMUM_SAMPLING_RATE)
		Settings_Data.Sampling_Rate -= SAMPLING_RATE_STEP;

	if (Settings_Data.Sampling_Rate < MINIMUM_SAMPLING_RATE)
		Settings_Data.Sampling_Rate = MINIMUM_SAMPLING_RATE;

	return Settings_Data.Sampling_Rate;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_Optical_Trigger_Threshold
 * Brief:       increment Optical Trigger Threshold data by one step
 * Parameter:   None
 * Return:      new threshold
 -----------------------------------------------------------------------------*/

float increment_Optical_Trigger_Threshold(void)
{
	if (Settings_Data.QRDbreakV < MAXIMUM_QRDBREAK_V)
		Settings_Data.QRDbreakV += QRDBREAK_V_STEP;

	if (Settings_Data.QRDbreakV > MAXIMUM_QRDBREAK_V)
		Settings_Data.QRDbreakV = MAXIMUM_QRDBREAK_V;

	return Settings_Data.QRDbreakV;
}
/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Optical_Trigger_Threshold
 * Brief:       decrement Optical Trigger Threshold data by one step
 * Parameter:   None
 * Return:      new threshold
 -----------------------------------------------------------------------------*/
float decrement_Optical_Trigger_Threshold(void)
{
	if (Settings_Data.QRDbreakV > MINIMUM_QRDBREAK_V)
		Settings_Data.QRDbreakV -= QRDBREAK_V_STEP;

	if (Settings_Data.QRDbreakV < MINIMUM_QRDBREAK_V)
		Settings_Data.QRDbreakV = MINIMUM_QRDBREAK_V;

	return Settings_Data.QRDbreakV;
}
/*-----------------------------------------------------------------------------* 
 * Function:    increment_Test_Time
 * Brief:       increment Test Time by one step
 * Parameter:   None
 * Return:      new test time
 -----------------------------------------------------------------------------*/
uint_8 increment_Test_Time(void)
{
	if (Settings_Data.Test_Time < MAXIMUM_TEST_TIME)
		Settings_Data.Test_Time += TEST_TIME_STEP;

	if (Settings_Data.Test_Time > MAXIMUM_TEST_TIME)
		Settings_Data.Test_Time = MAXIMUM_TEST_TIME;

	return Settings_Data.Test_Time;
}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Test_Time
 * Brief:       decrement Test Time by one step
 * Parameter:   None
 * Return:      new test time
 -----------------------------------------------------------------------------*/
uint_8 decrement_Test_Time(void)
{
	if (Settings_Data.Test_Time > MINIMUM_TEST_TIME)
		Settings_Data.Test_Time -= TEST_TIME_STEP;
	if (Settings_Data.Test_Time < MINIMUM_TEST_TIME)
		Settings_Data.Test_Time = MINIMUM_TEST_TIME;

	return Settings_Data.Test_Time;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_Force_Sensor_Calibration
 * Brief:       increment kforce5 by one step
 * Parameter:   None
 * Return:      new kforce5
 -----------------------------------------------------------------------------*/
float increment_Force_Sensor_Calibration(void)
{
	if (Settings_Data.Kforce5 < MAXIMUM_K_FORCE5)
		Settings_Data.Kforce5 += K_FORCE5_STEP;
	if (Settings_Data.Kforce5 > MAXIMUM_K_FORCE5)
		Settings_Data.Kforce5 = MAXIMUM_K_FORCE5;

	return Settings_Data.Kforce5;
}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Force_Sensor_Calibration
 * Brief:       decrement kforce5 by one step
 * Parameter:   None
 * Return:      new kforce5
 -----------------------------------------------------------------------------*/
float decrement_Force_Sensor_Calibration(void)
{
	if (Settings_Data.Kforce5 > MINIMUM_K_FORCE5)
		Settings_Data.Kforce5 -= K_FORCE5_STEP;
	if (Settings_Data.Kforce5 < MINIMUM_K_FORCE5)
		Settings_Data.Kforce5 = MINIMUM_K_FORCE5;
	return Settings_Data.Kforce5;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_Calibration_Time
 * Brief:       increment calibration time by one step
 * Parameter:   None
 * Return:      new calib time
 -----------------------------------------------------------------------------*/
float increment_Calibration_Time(void)
{
	if (Settings_Data.Calibration_Time < MAXIMUM_CALIBERATION_TIME)
		Settings_Data.Calibration_Time += CALIBERATION_TIME_STEP;

	if (Settings_Data.Calibration_Time > MAXIMUM_CALIBERATION_TIME)
		Settings_Data.Calibration_Time = MAXIMUM_CALIBERATION_TIME;

	return Settings_Data.Calibration_Time;
}
/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Calibration_Time
 * Brief:       decrement calibration time by one step
 * Parameter:   None
 * Return:      new calib time
 -----------------------------------------------------------------------------*/
float decrement_Calibration_Time(void)
{
	if (Settings_Data.Calibration_Time > MINIMUM_CALIBERATION_TIME)
		Settings_Data.Calibration_Time -= CALIBERATION_TIME_STEP;

	if (Settings_Data.Calibration_Time < MINIMUM_CALIBERATION_TIME)
		Settings_Data.Calibration_Time = MINIMUM_CALIBERATION_TIME;

	return Settings_Data.Calibration_Time;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_Compass_Samples
 * Brief:       increment number of compass samples by one step
 * Parameter:   None
 * Return:      new compass sample count
 -----------------------------------------------------------------------------*/
uint_8 increment_Compass_Samples(void)
{
	if (Settings_Data.Compass_Samples < MAXIMUM_COMPASS_SAMPLES)
		Settings_Data.Compass_Samples += COMPASS_SAMPLES_STEP;

	if (Settings_Data.Compass_Samples > MAXIMUM_COMPASS_SAMPLES)
		Settings_Data.Compass_Samples = MAXIMUM_COMPASS_SAMPLES;

	return Settings_Data.Compass_Samples;
}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_Compass_Samples
 * Brief:       decrement number of compass samples by one step
 * Parameter:   None
 * Return:      new compass sample count
 -----------------------------------------------------------------------------*/
uint_8 decrement_Compass_Samples(void)
{
	if (Settings_Data.Compass_Samples > MINIMUM_COMPASS_SAMPLES)
		Settings_Data.Compass_Samples -= COMPASS_SAMPLES_STEP;

	if (Settings_Data.Compass_Samples < MINIMUM_COMPASS_SAMPLES)
		Settings_Data.Compass_Samples = MINIMUM_COMPASS_SAMPLES;
	return Settings_Data.Compass_Samples;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_QRDpercentlow
 * Brief:       increment QRDpercentlow by one step
 * Parameter:   None
 * Return:      new QRDpercentlow
 -----------------------------------------------------------------------------*/
float increment_QRDpercentlow(void)
{
	if (Settings_Data.QRDpercentLow < MAXIMUM_QRDPERCENT_LOW)
		Settings_Data.QRDpercentLow += QRDPERCENT_LOW_CALIBERATION_STEP;

	if (Settings_Data.QRDpercentLow > MAXIMUM_QRDPERCENT_LOW)
		Settings_Data.QRDpercentLow = MAXIMUM_QRDPERCENT_LOW;

	return Settings_Data.QRDpercentLow;
}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_QRDpercentlow
 * Brief:       decrement QRDpercentlow by one step
 * Parameter:   None
 * Return:      new QRDpercentlow
 -----------------------------------------------------------------------------*/
float decrement_QRDpercentlow(void)
{
	if (Settings_Data.QRDpercentLow > MINIMUM_QRDPERCENT_LOW)
		Settings_Data.QRDpercentLow -= QRDPERCENT_LOW_CALIBERATION_STEP;
	if (Settings_Data.QRDpercentLow < MINIMUM_QRDPERCENT_LOW)
		Settings_Data.QRDpercentLow = MINIMUM_QRDPERCENT_LOW;
	return Settings_Data.QRDpercentLow;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_QRDpercentHigh
 * Brief:       increment QRDpercentHigh by one step
 * Parameter:   None
 * Return:      new QRDpercentHigh
 -----------------------------------------------------------------------------*/
float increment_QRDpercentHigh(void)
{
	if (Settings_Data.QRDpercentHigh < MAXIMUM_QRDPERCENT_HIGH)
		Settings_Data.QRDpercentHigh += QRDPERCENT_HIGH_CALIBERATION_STEP;

	if (Settings_Data.QRDpercentHigh > MAXIMUM_QRDPERCENT_HIGH)
		Settings_Data.QRDpercentHigh = MAXIMUM_QRDPERCENT_HIGH;

	return Settings_Data.QRDpercentHigh;
}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_QRDpercentHigh
 * Brief:       decrement QRDpercentHigh by one step
 * Parameter:   None
 * Return:      new QRDpercentHigh
 -----------------------------------------------------------------------------*/
float decrement_QRDpercentHigh(void)
{
	if (Settings_Data.QRDpercentHigh > MINIMUM_QRDPERCENT_HIGH)
		Settings_Data.QRDpercentHigh -= QRDPERCENT_HIGH_CALIBERATION_STEP;
	if (Settings_Data.QRDpercentHigh < MINIMUM_QRDPERCENT_HIGH)
		Settings_Data.QRDpercentHigh = MINIMUM_QRDPERCENT_HIGH;
	return Settings_Data.QRDpercentHigh;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_battery_threshold
 * Brief:       increment battery threshold by one step
 * Parameter:   None
 * Return:      new battery threshold
 -----------------------------------------------------------------------------*/
float increment_battery_threshold(void)
{
	if (Settings_Data.battery_threshold < MAXIMUM_BATTERY_THRESHOLD)
		Settings_Data.battery_threshold += BATTERY_THRESHOLD_CALIBERATION_STEP;

	if (Settings_Data.battery_threshold > MAXIMUM_BATTERY_THRESHOLD)
		Settings_Data.battery_threshold = MAXIMUM_BATTERY_THRESHOLD;

	return Settings_Data.battery_threshold;

}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_battery_threshold
 * Brief:       decrement battery threshold by one step
 * Parameter:   None
 * Return:      new battery threshold
 -----------------------------------------------------------------------------*/
float decrement_battery_threshold(void)
{
	if (Settings_Data.battery_threshold > MINIMUM_BATTERY_THRESHOLD)
		Settings_Data.battery_threshold -= BATTERY_THRESHOLD_CALIBERATION_STEP;
	if (Settings_Data.battery_threshold < MINIMUM_BATTERY_THRESHOLD)
		Settings_Data.battery_threshold = MINIMUM_BATTERY_THRESHOLD;
	return Settings_Data.battery_threshold;
}

/*-----------------------------------------------------------------------------* 
 * Function:    increment_battery_threshold
 * Brief:       increment battery threshold by one step
 * Parameter:   None
 * Return:      new battery threshold
 -----------------------------------------------------------------------------*/
float increment_xfraction_value(void)
{
	if (Settings_Data.xfraction < MAXIMUM_XFRACTION_VALUE)
		Settings_Data.xfraction += XFRACTION_VALUE_CALIBERATION_STEP;

	if (Settings_Data.xfraction > MAXIMUM_XFRACTION_VALUE)
		Settings_Data.xfraction = MAXIMUM_XFRACTION_VALUE;

	return Settings_Data.xfraction;

}

/*-----------------------------------------------------------------------------* 
 * Function:    decrement_battery_threshold
 * Brief:       decrement battery threshold by one step
 * Parameter:   None
 * Return:      new battery threshold
 -----------------------------------------------------------------------------*/
float decrement_xfraction_value(void)
{
	if (Settings_Data.xfraction > MINIMUM_XFRACTION_VALUE)
		Settings_Data.xfraction -= XFRACTION_VALUE_CALIBERATION_STEP;
	if (Settings_Data.xfraction < MINIMUM_XFRACTION_VALUE	)
		Settings_Data.xfraction = MINIMUM_XFRACTION_VALUE	;
	return Settings_Data.xfraction;
}
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
