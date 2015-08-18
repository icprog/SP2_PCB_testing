/******************************************************************************
 *
 *   Filename: $Sensor_Configuration.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of reading output from several sensors 
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include "adc_k70.h"
#include "Sensor_Configuration.h"

/*-----------------------------------------------------------------------------* 
 * Function:    GetSensor_Output
 * Brief:       Read several sensor outputs
 * Parameter:   pointers to hold pressure ,depth,qrd,raw pressure and raw depth
 * Return:      status
 -----------------------------------------------------------------------------*/
uint_8 GetSensor_Output(float *pressure, float *depth, uint_16 *Qrd, uint_16 *raw_pressure,
		uint_16 *raw_depth)
{
    ADC_RESULT_STRUCT adc_out;
    uint_8 tempBuffer[4];
    uint_16 tempData;
    float Vpressure_Difference;
 
    *pressure = 0;
    *depth = 0;
    *Qrd = 0;
    *raw_pressure = 0;
    *raw_depth = 0;
    if (read(force_sens, &adc_out, sizeof(adc_out)))
    {
//        tempBuffer[0] = (adc_out.result >> 8);
//        tempBuffer[1] = (adc_out.result & 0x00FF);
    	tempData=adc_out.result;
        Calculate_Pressure_from_raw(tempData, &Vpressure_Difference, pressure);
        *raw_pressure = (uint_16)(tempData * (RAW_DATA_TO_VOLTAGE_MULTIPLIER) * 1000);
        
    }
    else
    {
        return 1;
    }
    
    if (read(ir_sens, &adc_out, sizeof(adc_out)))
    {
        *raw_depth = adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER;
        tempBuffer[0] = (adc_out.result >> 8);
        tempBuffer[1] = (adc_out.result & 0x00FF);
        Calculate_Distance_from_raw_Data(tempBuffer, depth);
        tempData = (tempBuffer[0] * 0x0100) + tempBuffer[1];
        *raw_depth = (uint_16)(tempData * (RAW_DATA_TO_VOLTAGE_MULTIPLIER) * 1000);
        
    }
    else
    {
        return 1;
    }
    

    
    if (read(ros_sens1, &adc_out, sizeof(adc_out)))
    {
        
        *Qrd = (uint_16)((float) adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER * 1000);

        
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
