/*
 * Sensor_Configuration.h
 *
 *  Created on: Feb 5, 2014
 *      Author: user
 */

#ifndef SENSOR_CONFIGURATION_H_
#define SENSOR_CONFIGURATION_H_

#define V_PRESSURE_0 	(0.788303)
#define V_PRESSURE_MIN 	(0.003)
#define K_PRESSURE		(0.117)
//#define SA_VALUE		(0.000019635)

#define RAW_DATA_TO_VOLTAGE_MULTIPLIER (0.00005035)
#define BATTERY_VOLTAGE_MULTIPILER (0.000048)

uint_8 GetSensor_Output(float *pressure, float *depth, uint_16 *Qrd, uint_16 *raw_pressure,
		uint_16 *raw_depth);
#endif

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
-----------------------------------------------------------------------------*/
