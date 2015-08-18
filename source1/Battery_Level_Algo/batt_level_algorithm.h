/******************************************************************************
 *
 *   Filename: $ batt_level_algorithm.h
 *   Created on: $Date: June 08, 2015 
 *   Revision: $3
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of battery level monitoring algorithm.
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

/************************* DEFINES CONSTANTS **********************************/
 /* Define temp range*/
 #define TEMP_MAX				(18)
 #define TEMP_MIDLE				(0)
 #define TEMP_MIN	    		(-18)
 #define  TEMP_VALUE_CNT		(93)
 
 /* Define load range*/
 #define LOAD_MIN				(7.5)
 #define LOAD_MIDLE				(15)
 #define LOAD_MAX	    		(25)
 #define LOAD_VALUE_CNT			(31)
 

 /* Define Voltage range*/
 #define VOLTAGE_MAX			(3.0)
 #define VOLTAGE_MIN	    	(2.25)
 #define VOLTAGE_VALUE_CNT		(31)
 
/*************************GLOBAL FUNCTION DEFINITIONS ************************/
void triBattInterp( float temp, float current,float voltage,float *capacityLeft,float *capacityBin);
 
 
 
 
