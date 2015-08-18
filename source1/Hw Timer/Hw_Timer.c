

/******************************************************************************
 *
 *     Filename:       Hw_Timer.c
 *     Created on:     Dec 18, 2013
 *     Revision:       v0.1
 *     Author:         GadgEon
 *
 *     Description:    $$Configure Hardware timer for ADC acquisition
 *
 *     Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "Hw_Timer.h"
#include "lsm303.h"
#include "config.h"
#include "Sd_Card.h"
#include "adc_k70.h"
//#include "tmp006.h"
#include "data_acquisition.h"
//#include "DDR/ddr_config.h"
#include "algorithm.h"
#include "config.h"
#include "common_headers.h"

//volatile uint_32 adc_count0=0,adc_count1=0,adc_count2=0;
volatile uint_32 ticks=0,Lsm_Sample_Rate=5000;
long int test_count=25000;


extern HWTIMER_DEVIF_STRUCT BSP_HWTIMER1_DEV;  //device struct
HWTIMER hwtimer1,hwtimer2;                     //hwtimer handle


void hwtimer1_callback(pointer);
void hwtimer2_callback(pointer);



/*-----------------------------------------------------------------------------
 *  Function:     Init_Hw_Timer
 *  Brief:        Initialize hardware timer 1 for creating time interval for adc sampling.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Init_Hw_Timer(void)
{
	/* Initialization of hwtimer1*/
	printf("Initialization of hwtimer1   :");
	if (MQX_OK != hwtimer_init(&hwtimer1, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
	{
		printf(" FAILED!\n");
	}
	else
	{
//		printf(" OK\n");
	}

//	printf("Try to set frequency %d Hz to hwtimer1\n",HWTIMER1_FREQUENCY);
	hwtimer_set_freq(&hwtimer1, BSP_HWTIMER1_SOURCE_CLK, Adc_Sampling_Freq);
//	printf("Read frequency from hwtimer1 : %d Hz\n", hwtimer_get_freq(&hwtimer1));
//	printf("Read period from hwtimer1    : %d us\n", hwtimer_get_period(&hwtimer1));
//	printf("Read modulo from hwtimer1    : %d\n", hwtimer_get_modulo(&hwtimer1));
	
	/* Register isr for hwtimer1 */
//	printf("Register callback for hwtimer1\n");
	hwtimer_callback_reg(&hwtimer1,(HWTIMER_CALLBACK_FPTR)hwtimer1_callback, NULL);

	/* Initialization of hwtimer2*/
//	printf("Initialization of hwtimer2   :");
//	if (MQX_OK != hwtimer_init(&hwtimer2, &BSP_HWTIMER2_DEV, BSP_HWTIMER2_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
//	{
//		printf(" FAILED!\n");
//	}
//	else
//	{
//		printf(" OK\n");
//	}
	
}

void De_Init_Hw_Timer(void)
{
    /* Deinitialization of hwtimers */
    hwtimer_deinit(&hwtimer1);
}

/* Starts Hw Timer at specified Frequency*/
void Start_Hw_Timer_1(void)
{	
	printf("\nHw Started.....\n");
	hwtimer_start(&hwtimer1);
}


/* Stops Hw Timer*/
void Stop_Hw_Timer_1(void)
{	
	hwtimer_stop(&hwtimer1);
	printf("\nHw Stoped.....\n");
}



/* Clear pending callback for hwtimer1*/
void Cancel_Hw_Timer(void)
{	
	hwtimer_callback_cancel(&hwtimer1); // Clear pending callback for hwtimer1
}

/* Blocks hwtimer1*/
void Block_Hw_Timer(void)
{	
	hwtimer_callback_block(&hwtimer1); 
}


/* Unblocks blocked hwtimer1*/
void UnBlock_Hw_Timer(void)
{	
	hwtimer_callback_unblock(&hwtimer1); 
}



/*-----------------------------------------------------------------------------
 *  Function:     set_adc_sample_Rate
 *  Brief:        This function sets sampling rate in Hz 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void set_adc_sample_Rate(uint_16 sample_rate)
{
	hwtimer_set_freq(&hwtimer1, BSP_HWTIMER1_SOURCE_CLK, sample_rate);
//	printf("Read frequency from hwtimer1 : %d Hz\n", hwtimer_get_freq(&hwtimer1));

}

/*-----------------------------------------------------------------------------
 *  Function:     hwtimer1_callback
 *  Brief:        This function reads adc samples at specified sampling rate
 *  Parameter:    None
 *  Return:       inline
-----------------------------------------------------------------------------*/

void hwtimer1_callback(pointer )
{
	ADC_RESULT_STRUCT adc_out;
	static uint_16 Force_temp=0;
	static uint_16 Distance_temp=0;
	static uint_16 QRD_temp=0;
#if ALGO_TEST
	
#else
	/*  Read Force sensor data*/
	if (read(force_sens, &adc_out,sizeof(adc_out)))
	{				
		AlgorithmData->force_buff[Acq_Data_Count]=(uint_16)adc_out.result;	
	}
	else
	{
		AlgorithmData->force_buff[Acq_Data_Count]=0;
	}
	
	/*  Read IR sensor data*/
	if (read(ir_sens, &adc_out,sizeof(adc_out)))
	{
		AlgorithmData->distanceBuff[Acq_Data_Count]=(uint_16)adc_out.result;
	}
	else
	{
		AlgorithmData->distanceBuff[Acq_Data_Count]=0;
	}
	
	/*  Read QRD sensor data*/
	if (read(optical_sens, &adc_out,sizeof(adc_out)))
	{
		AlgorithmData->qrdBuff[Acq_Data_Count]=(uint_16)adc_out.result;
	}
	else
	{
		AlgorithmData->qrdBuff[Acq_Data_Count]=0;
	}
	
	if(LSM_Updating_Flag)
	{
		AlgorithmData->AX[Acq_Data_Count]=Ax;
		AlgorithmData->AY[Acq_Data_Count]=Ay;
		AlgorithmData->AZ[Acq_Data_Count]=Az;
		AlgorithmData->MX[Acq_Data_Count]=Mx;
		AlgorithmData->MY[Acq_Data_Count]=My;
		AlgorithmData->MZ[Acq_Data_Count]=Mz;
	}
	else
	{
		AlgorithmData->AX[Acq_Data_Count]=Ax;
		AlgorithmData->AY[Acq_Data_Count]=Ay;
		AlgorithmData->AZ[Acq_Data_Count]=Az;
		AlgorithmData->MX[Acq_Data_Count]=Mx;
		AlgorithmData->MY[Acq_Data_Count]=My;
		AlgorithmData->MZ[Acq_Data_Count]=Mz;
	}
	Acq_Data_Count++;
#endif
	
	/*  Check for Test time..*/
	if(++ticks>=test_count)
	{   
		hwtimer_stop(&hwtimer1);
		stop_data_acquision();
		Test_Complete_flag = 1;
	}
	
	/*  Updates compass samples at specified sampling rate..*/
//	if(ticks>= Lsm_Sample_Rate)
//	{   
//	    	
//		Lsm_Sample_Rate +=LSM_COUNT;	
//	    Read_LSM=1;  
//	    if(lsm_data_count<MAX_LSM_SAMPLE_COUNT)
//	    {
////	    	Data_Write_file.LSM_MeterData[lsm_data_count].Tick_Count=ticks;
//	    }	
//	}  	
}
