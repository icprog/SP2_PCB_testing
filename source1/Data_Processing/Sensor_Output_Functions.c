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


/*-----------------------------------------------------------------------------* 
 * Function:    Get_ADC_Outputs
 * Brief:       Read several sensor outputs
 * Parameter:   pointers to hold pressure ,depth,qrd,raw pressure and raw depth
 * Return:      status
 -----------------------------------------------------------------------------*/
uint_8 Get_ADC_Outputs( uint_16 *pressure,uint_16 *depth, uint_16 *ros_1, uint_16 *ros_2,
					    uint_16 *Batt_volt,uint_16 *Batt_curr,uint_16 *Batt_temp )
{
    ADC_RESULT_STRUCT adc_out;
    
    *pressure = 0;
    *depth = 0;
    *ros_1 = 0;
    *ros_2 = 0;
    *Batt_volt = 0;
    *Batt_curr = 0;
    *Batt_temp = 0;
    
	ADC_Init();
	Time_Delay_Sleep(750);
	
    adc_out.result = 0;
    if (read(force_sens, &adc_out, sizeof(adc_out)))
    {
        *pressure = (uint_16)((float) adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER * 1000);      
    }
    else{
        return 1;
    }
    
    adc_out.result = 0;
    if (read(ir_sens, &adc_out, sizeof(adc_out)))
    {
        *depth = (uint_16)((float) adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER * 1000);        
    }
    else
    {
        return 1;
    }
    
    adc_out.result = 0;
    if (read(ros_sens1, &adc_out, sizeof(adc_out)))
    {   
        *ros_1 = (uint_16)((float) adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER * 1000);
       
    }
    else
    {
        return 1;
    }
    
    adc_out.result = 0;
    if (read(ros_sens2, &adc_out, sizeof(adc_out)))
    {    
        *ros_2 = (uint_16)((float) adc_out.result * (float) RAW_DATA_TO_VOLTAGE_MULTIPLIER * 1000);     
    }
    else
    {
        return 1;
    }
    ADC_deinit();
    Battery_ADC_Init();
    Time_Delay_Sleep(750);
    adc_out.result = 0;
    if (read(batt_sens, &adc_out, sizeof(adc_out)))
    {
        *Batt_volt = (uint_16)((float) adc_out.result * (float) BATTERY_VOLTAGE_MULTIPILER * 1000);
       
    }
    else
    {
        return 1;
    }
    
    adc_out.result = 0;
    if (read(batt_current, &adc_out, sizeof(adc_out)))
    {       
        *Batt_curr = (uint_16)((float) adc_out.result * (float) BATTERY_VOLTAGE_MULTIPILER * 1000);     
    }
    else
    {
        return 1;
    }
    
    adc_out.result = 0;   
    if (read(batt_temp, &adc_out, sizeof(adc_out)))
    {       
        *Batt_temp = (uint_16)((float) adc_out.result * (float) BATTERY_VOLTAGE_MULTIPILER * 1000);     
    }
    else
    {
        return 1;
    }
    Battery_ADC_Deinit();
    return 0;
}
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
