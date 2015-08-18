/******************************************************************************
 *
 *   Filename: $algorithm.c
 *   Created on: $Date: Nov 04, 2014 
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of data processing algorithms
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include <fio.h>
#include <timer.h>
#include <math.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algorithm.h"
#include "config.h"
#include "Data_Processessing_Functions.h"
#include "common_headers.h"
#include "matlab_functions.h"
#include "adjust.h"
#include "model.h"
#include "calib.h"



static float  SA = SA_VALUE;
static float Alpha = 0.01;
static uint32_t Trigger_Index = 0;
static const uint16_t Graphwidth = 200;
static const uint16_t Graphmax = 1000;
static float Dtrigger_A = 1435;
static float Dtrigger_M = 1406.659781;
static float Dmin_A = 150;
static float Dmin_M = 204.0101056;
static float Lprobe = 1500;
static float VirdmsSmoothMax = 0.0;
static uint32_t IRDMSflipIndex = 0;

//________________Sampling Rate depended parameters_____________________
static uint16_t IRDMSaverageSize ;
static uint16_t DepthSmoothLength;
static uint16_t QRDmedianSize;
static uint16_t pauseTime;
static uint16_t speedAverageSize;
static uint16_t PressureAverageSize;
static uint16_t PressureMod_averageSize;
static uint16_t fastsmoothWinSize;
//__________________End.______________________________________


static const uint16_t QRDdelayCounts = 200;
static const uint16_t QRD_triggerListCount = 25;
static const uint16_t V_QRD0_length = 100;

static float  Vpressure0 = V_PRESSURE_0;
static float  Vpressure_Min = V_PRESSURE_MIN;
static float  Vpressure_smooth_min = 0;

//uint16_t Sampling_Rate = 5000;
static float D_meas_prv = 0;
//Added for EndDepth Calculation
const float pauseIRDMSignoreV = 0.42;
int32_t delayRejectIndex=0;
const static float depthRangeMin = 1.0;
static float endShorten = 0; //mm to trim from EndIndex
uint16_t EndDepth=0;
uint32_t EndIndex=0;
const float IRDMSconstSpeedV = (0.6);
static float IRDMSCSdepth = 0.0;
static uint32_t Trigger_ROS = 0;
static uint32_t countsCSV=0;
static uint32_t countsCSdepth=0;
static float CSdepth = 0.0;
static float CSslope = 0;
static uint32_t	Trigger_CS =0;
static uint32_t	countsCStotal = 0;
static uint32_t indexCSdepth = 0;

static uint32_t vIrdmsComp_length=0;
static uint32_t depthRough_length=0;
static float IRDMSwinsizeNP1 = 0.29;
uint8_t badStartError = 0;
uint32_t Trigger_Rise=0;
uint32_t indexCSV=0;
int32_t extrapCounts=0;
int32_t triggerOffset =0;
uint32_t pressureCenteringCounts =0;
const uint32_t tipTriggerCounts = 13;
uint32_t Trigger_Pressure = 0;
const float Pressure_percent = 0.02;
float DepthROS =0.0;
float ROSdepthSkew=0.0;
uint32_t Trigger_Shifted=0;
uint32_t dist2ROSNS = 0;
uint32_t dist2ROSShift = 0;
uint32_t DepthRoughFullShifted_length=0;
uint32_t dist2ROSDR=0;
float mergeDepth = -450;
float minimum_DepthRough=0;
uint32_t mergeIndex=0;
uint32_t IRDMS_flipIndex2Trigger=0;
float flipDepth=0;
uint32_t CSpostFlipIndex=0;
uint32_t CSpreFlipIndex=0;
const float flipSlopeDiffThresh = 3;
const float PoffsetV = 0.6;
const float decFrac = 10;
const float stopVal = 1.0;
uint32_t waitCounts = 0;
uint32_t triggerEarly = 0;
uint32_t length_of_IRDMS_Smooth = 0;

float testAvgSpeed =0;
float CStestAvgSpeed=0;
float SHtestAvgSpeed=0;
float CSSHspeedDiff=0;
const float speedMaxLimit = 3000;
const float speedMinLimit = 100;
float PressureMeans1Min=0;

const float A_P4 = -194.1;
const float B_P4 = 0.1304;
const float C_P4 = -0.002314;
const float D_P4 = 197.7;

const float A_Po = -195;
const float B_Po = 0.045;
const float C_Po = -0.002223;
const float D_Po = 198;

const float A_Pr = -191.8;
const float B_Pr = 0.5639;
const float C_Pr = -0.01415;
const float D_Pr = 0.003;
const float E_Pr = 195;

const float xfractionA = 0.16;
const float xfractionB = 0.32;
const float xfractionC = 0.52;
const float xfractionD = 0.65;




/*-----------------------------------------------------------------------------* 
 * Function:    Clear_Global_Data
 * Brief:       This function initialise all global variables
 * Parameter:   none
 * Return:      None
 -----------------------------------------------------------------------------*/
void Initialize_Global_Data(void)
{

//	process_index=0;
//	Kforce_3 = Kforce_5 * (3 / 5.0);
//	SA = SA_VALUE;
//	Alpha = 0.01;
//	Trigger_Index = 0;
//	Dtrigger_A = 1435;
//	Dtrigger_M = 1406.659781;
//	Dmin_A = 150;
//	Dmin_M = 204.0101056;
//	Lprobe = 1500;
//	VirdmsSmoothMax = 0.0;
//	IRDMSflipIndex = 0;
//	Vpressure0 = V_PRESSURE_0;
//	Vpressure_Min = V_PRESSURE_MIN;
//	Vpressure_smooth_min = 0;
//
//
//	D_meas_prv = 0;
//	endShorten = 0; //mm to trim from EndIndex
//	EndDepth=0;
//	IRDMSCSdepth = 0.0;
//	Trigger_ROS = 0;
//	countsCSV=0;
//	countsCSdepth=0;
//	CSdepth = 0.0;
//	CSslope = 0;
//	Trigger_CS =0;
//	countsCStotal = 0;
//	memset(Pixels_buffer,0,((GRAPH_RAW_PIXEL_COUNT*5)+1)*sizeof(uint16_t));
}

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_IRDMS_Distance_and_Depth_from_Voltage
 * Brief:       This function calculate depth from sensor voltage
 * Input:   	VIRDMS_Mod
 * Output:      depthRoughFull
 -----------------------------------------------------------------------------*/
void Calculate_IRDMS_Distance_and_Depth_from_Voltage(void)
{
	uint32_t i=0;
	float D_forward=0;
	float D_forward_prv =BIRDMS + powf(((AlgorithmData->VIRDMS_Mod[0]) - DIRDMS) / AIRDMS, (1 / CIRDMS));
	
	indexCSdepth=0;
	IRDMSCSdepth = BIRDMS+powf(((IRDMSconstSpeedV-DIRDMS)/AIRDMS),(1/CIRDMS)) - Lprobe;
//	printf("\r\n IRDMSCSdepth :%f",IRDMSCSdepth);
	#if ENABLE_PRINTF_DEPTH_ROUGH_FULL
	
	printf("\r\nAlgorithmData->depthRoughFull");
	#endif
	for(i=0; i < vIrdmsComp_length ; i++)
	{
		D_forward = BIRDMS + powf(((AlgorithmData->VIRDMS_Mod[i]) - DIRDMS) / AIRDMS, (1 / CIRDMS));
		if(D_forward>D_forward_prv)
		{
			D_forward = D_forward_prv;
		}
		D_forward_prv = D_forward;
		AlgorithmData->depthRoughFull[i] = D_forward - Lprobe;
		
		if(AlgorithmData->depthRoughFull[i]>IRDMSCSdepth)//remember that depths are negative!
		{
			indexCSdepth=i;
		}
		#if ENABLE_PRINTF_DEPTH_ROUGH_FULL
	
		printf("\r\n%f",AlgorithmData->depthRoughFull[i]);
		#endif
	}
	
		#if ENABLE_PRINTF_DEPTH_ROUGH_FULL
	
		printf("\r\n");
		#endif

}
/*-----------------------------------------------------------------------------* 
 * Function:    Find_15cm_output_marker
 *	Inputs: VIRDMS,Smooth
 *		b. Actions: There are multiple consecutive actions to this step
 *			b.i. 	Find the maximum value in VIRDMS,Smooth and record it as 
 *					VIRDMS,SmoothMax. Also record the index location of this value and 
 *					record it as IRDMSflipIndex.
 *			b.ii. 		Copy VIRDMS,Smoothand call the new data set VIRDMSMod.
 *			b.iii. 		For each value in VIRDMSMod that occurs after index location 
 *					IRDMSflipIndex, apply the following formula:
 *			b.iv 	It will also calculate Trigger_Rise and indexCSV
 *	c. Output: VIRDMS,Mod
 *-----------------------------------------------------------------------------*/
void Find_15cm_output_marker(void)
{
	uint32_t i=0;
	
	VirdmsSmoothMax = 0.0;
	IRDMSflipIndex = vIrdmsComp_length-1;
	Trigger_Rise = 0;
	indexCSV =0;
	for(i=0; i < vIrdmsComp_length ; i++)
	{

		if(AlgorithmData->irdmsSmooth[i]>VirdmsSmoothMax)
		{
				VirdmsSmoothMax = AlgorithmData->irdmsSmooth[i];
				IRDMSflipIndex = i;
				
		}
		//Trigger by beginning of IRDMSSmooth rise
		if(AlgorithmData->irdmsSmooth[i] < (V_IRDMS_OFFSET_V + 0.0001))
		{
			Trigger_Rise =i;
		}
		if(AlgorithmData->irdmsSmooth[i]<IRDMSconstSpeedV)
		{
			indexCSV=i;
		}
	}
	
	//printf("\r\nIRDMSflipIndex :%d\r\nVirdmsSmoothMax:%0.8f \r\nTrigger_Rise:%d\r\n indexCSV:%d\r\n",IRDMSflipIndex,VirdmsSmoothMax,Trigger_Rise,indexCSV);

	//AlgorithmData->VIRDMS_Mod = AlgorithmData->irdmsSmooth; //Copy VIRDMS Smooth and call the new data set VIRDMSMod. (Copy the start address of the irdms smooth and call this as VIRDMS_Mod)
	for(i=IRDMSflipIndex; i < vIrdmsComp_length ; i++)
	{
		AlgorithmData->irdmsSmooth[i]=(2*VirdmsSmoothMax)-AlgorithmData->irdmsSmooth[i];
	}
	AlgorithmData->VIRDMS_Mod = AlgorithmData->irdmsSmooth; //Copy VIRDMS Smooth and call the new data set VIRDMSMod. (Copy the start address of the irdms smooth and call this as VIRDMS_Mod)
	

	#if ENABLE_PRINTF_VIRDMS_MOD
	printf("\n\rAlgorithmData->VIRDMS_Mod");
	for(i=0; i < vIrdmsComp_length ; i++)
	{
		printf("\n\r%0.8f",AlgorithmData->VIRDMS_Mod[i]);
	}
	printf("\n\r");
	#endif
	
}
/*-----------------------------------------------------------------------------* 
 * 	Function:   Look_for_Trigger_from_Pressure_Sensor
 *	Brief: 		Look for Force Sensor Trigger, only triggers if
				once tipTriggerCounts instances of the tip trigger are met
 *	Input:   	V_pressureSmooth , V_pressure
 *	Output:      PressureChange,Trigger_Pressure,Trigger_Index
 *-----------------------------------------------------------------------------*/
void Look_for_Trigger_from_Pressure_Sensor(void)
{
	double Vpressure0_total=0;
	uint32_t i=0,q=0,r=0;
	float tempdata=0;
	const uint32_t sample_count = (uint32_t)(Settings_Data.Sampling_Rate * Settings_Data.Calibration_Time *1000);


	pressureCenteringCounts = (uint32_t)(PressureAverageSize/2.0)+0.5;
	
	for(i=0; i<sample_count; i++)
	{
		Vpressure0_total += AlgorithmData->V_pressure[i];
	}
	Vpressure0 = (float)Vpressure0_total/(float)sample_count;
	
//	printf(" Vpressure0\t:\t%f\r\n",Vpressure0);
	
	for(i=0; i<Acq_Data_Count; i++)
	{
		tempdata = 1-(AlgorithmData->V_pressureSmooth[i]/Vpressure0);
		if(tempdata<0)
		{
			tempdata = 0-tempdata;
		}
		AlgorithmData->PressureChange[i] = tempdata;
	}

	q = 0;  
	if(((int32_t)(Trigger_ROS - ((PressureAverageSize/2.0)+0.5)) )< 0 )
	{
		r = 0;
	}
	else
	{
		r = Trigger_ROS - (uint32_t)((PressureAverageSize/2.0)+0.5); 
	}
	//printf(" r\t:\t%d\r\n",r);
	Trigger_Pressure =1;
	while (q < tipTriggerCounts)
	{
		if (AlgorithmData->PressureChange[r] > Pressure_percent)
		{
			q = q+1;
		}
		if (q == tipTriggerCounts)
		{
			Trigger_Pressure = r + pressureCenteringCounts;
		}
		else
		{
			r = r+1;
		}
		if ((q == tipTriggerCounts) && (Trigger_Pressure < Trigger_ROS))  //change to Trigger_ROS to change priority with CS trigger
		{  
			Trigger_Index = Trigger_Pressure-(uint32_t)(((Settings_Data.Sampling_Rate*1000)/26.0)+0.5);
			Trigger_Pressure = Trigger_Index;
			countsCStotal = abs(Trigger_Pressure - indexCSdepth);
			CSslope = CSdepth/countsCStotal;
		}    
		else if ((q == tipTriggerCounts) && (Trigger_CS < 1) && (Trigger_Pressure < Trigger_ROS))
		{
			Trigger_Index = Trigger_Pressure-(uint32_t)(((Settings_Data.Sampling_Rate*1000)/26.0)+0.5);
			Trigger_Pressure = Trigger_Index;
			countsCStotal = abs(Trigger_Pressure - indexCSdepth);
			CSslope = CSdepth/countsCStotal;
		}  
		else
		{
			r = r+1;
		}
		if (r==Trigger_ROS) //change to Trigger_ROS to change priority with CS trigger
		{
			Trigger_Index = Trigger_CS;
			q = tipTriggerCounts;
		}

	
	
	}
	triggerEarly = 0;
	if (Trigger_Index < sample_count)
    {
		waitCounts = sample_count;
		triggerEarly = 1;
	}
/* 	printf("\r\n********* Triggers *********\r\n");
	printf(" Trigger_ROS \t:\t%d\r\n",Trigger_ROS);
	printf(" Trigger_CS \t:\t%d\r\n",Trigger_CS);
	printf(" Trigger_Index \t:\t%d\r\n",Trigger_Index);
 */
 #if ENABLE_PRINTF_PRESSURECHANGE
	printf("AlgorithmData->PressureChange\r\n");
	for(i=0; i<Acq_Data_Count; i++)
	{
		printf("%f\r\n",AlgorithmData->PressureChange[i]);
	}
#endif
}

/*-----------------------------------------------------------------------------* 
 * Function:    RedefineDepthRough
 * Brief:       Redefine DepthRough and calculate Depth, using final Trigger_Index.
 *				This function calculates DepthROS,ROSdepthSkew, create columns of DepthRoughFullOffset, 
 * 				depthRough which include only values from Trigger_Index and DepthRoughShift
 * Parameter:   None
 * Input 	: 	depthRoughFull ,Trigger_Index
 * Return:      None, 
 * Outputs: 	depthRough, DepthRoughShift
 -----------------------------------------------------------------------------*/
void RedefineDepthRough(void)
{
	uint32_t i=0;
	
	DepthROS = AlgorithmData->depthRoughFull[Trigger_ROS];
	ROSdepthSkew = DepthROS+triggerOffset ;//if negative, then need to shift depths towards surface to match trigger_ROS
	//note that triggerOffset is positive (magnitude only)
	depthRough_length=0;
	for(i=0;i<Trigger_Index;i++)
	{
		AlgorithmData->DepthRoughFullOffset[i] = AlgorithmData->depthRoughFull[i]-ROSdepthSkew;
		if(AlgorithmData->DepthRoughFullOffset[i] > 0)
		{
			Trigger_Shifted =i;
		}
	}
	for(i=Trigger_Index;i<vIrdmsComp_length;i++)
	{
		AlgorithmData->depthRough[depthRough_length] = AlgorithmData->depthRoughFull[i];
		
		if(AlgorithmData->depthRough[depthRough_length]> -triggerOffset )
		{
			dist2ROSNS = depthRough_length;
		}
		
		AlgorithmData->DepthRoughFullOffset[i] = AlgorithmData->depthRoughFull[i]-ROSdepthSkew;
		
		if(AlgorithmData->DepthRoughFullOffset[i] > 0)
		{
			Trigger_Shifted =i;
		}
		AlgorithmData->DepthRoughShift[depthRough_length] = AlgorithmData->DepthRoughFullOffset[i];
		
		if(AlgorithmData->DepthRoughShift[depthRough_length]> -triggerOffset )
		{
			dist2ROSShift = depthRough_length;
		}
		depthRough_length++;

	}
	
#if ENABLE_PRINTF_DEPTHROUGHFULLOFFSET
	
	printf("AlgorithmData->DepthRoughFullOffset");
	for( i = 0; i<vIrdmsComp_length;i++)
	{
		printf("\r\n%f",AlgorithmData->DepthRoughFullOffset[i]);
	}
	printf("\r\n");
	#endif
	#if ENABLE_PRINTF_DEPTHROUGHSHIFT
	
	printf("AlgorithmData->DepthRoughShift");
	for( i = 0; i<depthRough_length;i++)
	{
		printf("\r\n%f",AlgorithmData->DepthRoughShift[i]);
	}
	printf("\r\n");
	#endif

	#if ENABLE_PRINTF_DEPTHROUGH_B
	
	printf("AlgorithmData->DepthRoughB");
	for( i = 0; i<depthRough_length;i++)
	{
		printf("\r\n%f",AlgorithmData->depthRough[i]);
	}
	printf("\r\n");
	#endif
}
/*-----------------------------------------------------------------------------* 
 * Function	: Lock_at_Trigger_ROS
 * Brief	: Modify depthRough using ROS trigger index
 * Parameter: None
 * Input 	: depthRough 
 * Outputs	: Modified depthRough E
 -----------------------------------------------------------------------------*/
void Lock_at_Trigger_ROS(void)
{
	float temp_data=0;
	uint32_t i=0,temp_uint32_t_data=0;
	float DepthROSPLock=0,ROSskewPLock=0;
	uint32_t dist2ROS=0 , DepthRoughUnaltered_len=0 ,mergeListIndex=0;
	
	if (Trigger_Index != Trigger_Pressure)
	{
		if (ROSdepthSkew < 0) //NO_pranjal Example will enter this if section
		{

			depthRough_length=0;
			for(i=Trigger_Shifted;i<vIrdmsComp_length;i++)
			{
				AlgorithmData->depthRough[depthRough_length++] = AlgorithmData->DepthRoughFullOffset[i];
			}
		
		}
		else if (ROSdepthSkew > 0)
		{

			Trigger_Shifted = Trigger_Index - (dist2ROSNS - dist2ROSShift);

			depthRough_length=0;
			for(i=Trigger_Shifted;i<vIrdmsComp_length;i++)
			{
				AlgorithmData->depthRough[depthRough_length++] = AlgorithmData->DepthRoughFullOffset[i];
			}

			for (i = 0; i<dist2ROSNS; i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] + ROSdepthSkew*((dist2ROSNS-i)/dist2ROSNS);
			}
		   
			
		}
		else
		{

			  Trigger_Shifted = Trigger_Index;   
		}

		Trigger_Index = Trigger_Shifted;
	//	printf("\r\n Trigger_Index:%d",Trigger_Index);
	#if ENABLE_PRINTF_DEPTHROUGH_C
	 	printf("AlgorithmData->depthRough C \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
	#endif
		DepthRoughFullShifted_length=0;
		mergeDepth = -450;
		mergeIndex=0;
		for (i = Trigger_Shifted; i<vIrdmsComp_length; i++)
		{
			AlgorithmData->DepthRoughFullShifted[DepthRoughFullShifted_length] = AlgorithmData->depthRoughFull[i];
			if(AlgorithmData->DepthRoughFullShifted[DepthRoughFullShifted_length] > mergeDepth)
			{
				mergeIndex =DepthRoughFullShifted_length;
			}
			DepthRoughFullShifted_length++;
		}
		
		dist2ROSDR=0;
		minimum_DepthRough = AlgorithmData->depthRough[0];
		for(i=0;i<depthRough_length;i++)
		{
			if(AlgorithmData->depthRough[i] > -triggerOffset)
			{
				dist2ROSDR=i;
			}
			if(AlgorithmData->depthRough[i]<minimum_DepthRough)
			{
				minimum_DepthRough = AlgorithmData->depthRough[i];
			}
		}


		if (!(minimum_DepthRough < mergeDepth))
		{
			mergeIndex = depthRough_length;
		}
		//printf("mergeIndex:%d\r\n",mergeIndex);
		temp_data=(mergeDepth-AlgorithmData->DepthRoughFullShifted[dist2ROSDR]); //taking out of loop
		temp_uint32_t_data = (mergeIndex+1);
		for( i = dist2ROSDR; i<temp_uint32_t_data; i++) //for( i = dist2ROSDR; i<(mergeIndex+1); i++)
		{
			AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] + (AlgorithmData->DepthRoughFullShifted[i]-AlgorithmData->depthRough[i])*((AlgorithmData->DepthRoughFullShifted[i]-AlgorithmData->DepthRoughFullShifted[dist2ROSDR])/temp_data);
		}
		#if ENABLE_PRINTF_DEPTHROUGH_D
	 	printf("AlgorithmData->depthRough D \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif
		for( i = mergeIndex+1;i<depthRough_length;i++)
		{
			AlgorithmData->depthRough[i] = AlgorithmData->DepthRoughFullShifted[i];
		}

	}
	else
	{
		if (AlgorithmData->depthRough[0] < 0) 
		{//example case is Galena1p17 00014
			

			DepthROSPLock = DepthROS - AlgorithmData->depthRough[0] ;
			ROSskewPLock = ROSdepthSkew - AlgorithmData->depthRough[0] ;
			for( i = 0; i<depthRough_length;i++)
			{
				AlgorithmData->DepthRoughPLock[i] = AlgorithmData->depthRough[i] - AlgorithmData->depthRough[0] ;
			}
			dist2ROS = Trigger_ROS - Trigger_Index;
			DepthRoughUnaltered_len = 0;
			mergeListIndex =0;
			for(i=Trigger_Index;i<vIrdmsComp_length;i++)
			{
				AlgorithmData->DepthRoughUnaltered[DepthRoughUnaltered_len] = AlgorithmData->depthRoughFull[i];
				if(AlgorithmData->DepthRoughUnaltered[DepthRoughUnaltered_len] > mergeDepth)
				{
					mergeListIndex = DepthRoughUnaltered_len;
				}
				
				DepthRoughUnaltered_len++;
			}
			AlgorithmData->depthRough[0] = AlgorithmData->DepthRoughPLock[0] - ROSskewPLock*(AlgorithmData->DepthRoughPLock[0]/DepthROSPLock);

			for( i = 1; i<(dist2ROS+1);i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->DepthRoughPLock[i] - ROSskewPLock*(AlgorithmData->DepthRoughPLock[i]/DepthROSPLock);

			}

		
			#if ENABLE_PRINTF_DEPTHROUGH_C
			printf("AlgorithmData->depthRough C \r\n");
			for( i = 0; i<depthRough_length;i++)
			{
				printf("%f\r\n",AlgorithmData->depthRough[i]);
			}
			printf("\r\n"); 
			#endif
			minimum_DepthRough = AlgorithmData->depthRough[0];
			for(i=0;i<depthRough_length;i++)
			{
				if(AlgorithmData->depthRough[i]<minimum_DepthRough)
				{
					minimum_DepthRough = AlgorithmData->depthRough[i];
				}
			}
			
			if (minimum_DepthRough < mergeDepth)
			{
				mergeIndex = mergeListIndex;
			}
			else
			{
				mergeIndex = depthRough_length;
			}
		
			for (i = dist2ROS+1;i<(mergeIndex+1);i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] - ROSdepthSkew;
			}
			temp_data = mergeDepth-AlgorithmData->DepthRoughUnaltered[dist2ROS];
			for (i = dist2ROS+1;i<(mergeIndex+1);i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] + (AlgorithmData->DepthRoughUnaltered[i]-AlgorithmData->depthRough[i])*((AlgorithmData->DepthRoughUnaltered[i]-AlgorithmData->DepthRoughUnaltered[dist2ROS])/(temp_data));
			}
		
		#if ENABLE_PRINTF_DEPTHROUGH_D
	 	printf("AlgorithmData->depthRough D \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif

	}	
	else  
	{   

		
		dist2ROS = Trigger_ROS - Trigger_Index;
		
		DepthRoughUnaltered_len = 0;
		mergeListIndex =0;
		for(i=Trigger_Index;i<vIrdmsComp_length;i++)
		{
			AlgorithmData->DepthRoughUnaltered[DepthRoughUnaltered_len] = AlgorithmData->depthRoughFull[i];
 			if(AlgorithmData->DepthRoughUnaltered[DepthRoughUnaltered_len] > mergeDepth)
			{
				mergeListIndex = DepthRoughUnaltered_len;
			}
			 
			DepthRoughUnaltered_len++;
		}
		
		if (ROSdepthSkew > 0)  //example case is Galena1p17 00019
		{
			for( i = 0; i<depthRough_length;i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] -ROSdepthSkew;
			}
			for (i = 0; i< (dist2ROS+1) ; i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] + ROSdepthSkew*((dist2ROS-i)/dist2ROS);
			}
			
			#if ENABLE_PRINTF_DEPTHROUGH_C
			printf("AlgorithmData->depthRough C \r\n");
			for( i = 0; i<depthRough_length;i++)
			{
				printf("%f\r\n",AlgorithmData->depthRough[i]);
			}
			printf("\r\n"); 
			#endif
			
		}
		else //%example case is dawson 00055
		{	 
			temp_data = -triggerOffset/(float)dist2ROS;
			
			for( i = 0; i<depthRough_length;i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] -ROSdepthSkew;
			}
			
			AlgorithmData->depthRough[0] = 0;
			for (i = 1;i<(dist2ROS+1);i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i-1]+temp_data;
			}
			
			#if ENABLE_PRINTF_DEPTHROUGH_C
			printf("AlgorithmData->depthRough C \r\n");
			for( i = 0; i<depthRough_length;i++)
			{
				printf("%f\r\n",AlgorithmData->depthRough[i]);
			}
			printf("\r\n"); 
			#endif

		}
		minimum_DepthRough = AlgorithmData->depthRough[0];
		for(i=0;i<depthRough_length;i++)
		{
			if(AlgorithmData->depthRough[i]<minimum_DepthRough)
			{
				minimum_DepthRough = AlgorithmData->depthRough[i];
			}
		}
		if (minimum_DepthRough < mergeDepth)
		{
			mergeIndex = mergeListIndex;
		}
		else
		{
			mergeIndex = depthRough_length;
		}
		temp_data = mergeDepth-AlgorithmData->DepthRoughUnaltered[dist2ROS];
		for (i = dist2ROS+1;i<(mergeIndex+1);i++)
		{
			AlgorithmData->depthRough[i] = AlgorithmData->depthRough[i] + (AlgorithmData->DepthRoughUnaltered[i]-AlgorithmData->depthRough[i])*((AlgorithmData->DepthRoughUnaltered[i]-AlgorithmData->DepthRoughUnaltered[dist2ROS])/(temp_data));
		}
		
		#if ENABLE_PRINTF_DEPTHROUGH_D
	 	printf("AlgorithmData->depthRough D \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif
		
			for (i = mergeIndex+1;i<DepthRoughUnaltered_len;i++)
			{
				AlgorithmData->depthRough[i] = AlgorithmData->DepthRoughUnaltered[i];
			}
		}
	}
	
	
		#if ENABLE_PRINTF_DEPTHROUGH_E
	 	printf("AlgorithmData->depthRough E \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif
}
/*-----------------------------------------------------------------------------* 
 * Function	: adjust_DepthRough_at_IRDMS_flipIndex
 * Brief	: This function modifies depthRough based on flip index
 * Parameter: None
 * Input 	: depthRough E
 * Outputs	: depthRough G
 -----------------------------------------------------------------------------*/
 void adjust_DepthRough_at_IRDMS_flipIndex(void)
 {
	uint32_t i=0;
	const float CSwinsize = 120;
	float temp_data_CSpostFlipIndex=0 ,temp_data_CSpreFlipIndex=0;
	uint32_t CSpostFlipLength=0,CSpreFlipLength=0;
	float CSpreSlope=0,CSpostSlope=0;
	float biggerSlope=0,smallerSlope=0,flipSlopeDiff =0;
	uint32_t temp_data=0;
	float Min_Flip_DepthRough=0;
	float Max_Flip_DepthRough=0;
	float flipRange=0;
	
	IRDMS_flipIndex2Trigger = IRDMSflipIndex - Trigger_Shifted;
	flipDepth = AlgorithmData->depthRough[IRDMS_flipIndex2Trigger];
/*  	printf(" IRDMSflipIndex:%d\r\n",IRDMSflipIndex);
	printf(" Trigger_Shifted:%d\r\n",Trigger_Shifted);
	printf(" IRDMS_flipIndex2Trigger:%d\r\n",IRDMS_flipIndex2Trigger);
	printf(" flipDepth:%f\r\n",flipDepth);  */
	//%constant speed around flipIndex:
	
	temp_data_CSpostFlipIndex = (flipDepth-(CSwinsize/4.0)); //Taking out of loop
	temp_data_CSpreFlipIndex = (flipDepth+((3*CSwinsize)/4.0)); //Taking out of loop
	

	for( i = 0; i<depthRough_length;i++)
	{
		if(AlgorithmData->depthRough[i] > temp_data_CSpostFlipIndex) //find(DepthRough > (flipDepth-round(CSwinsize/4)));
		{
			CSpostFlipIndex =i;
		}
		
		if(AlgorithmData->depthRough[i] > temp_data_CSpreFlipIndex) //find(DepthRough > (flipDepth-round(3*CSwinsize/4)));
		{
			CSpreFlipIndex =i;
		}
	}
	
	CSpostFlipLength = CSpostFlipIndex - IRDMS_flipIndex2Trigger;
	CSpreFlipLength = IRDMS_flipIndex2Trigger - CSpreFlipIndex;

	CSpreSlope = (3*CSwinsize/4.0)/(float)CSpreFlipLength;
	CSpostSlope = (CSwinsize/4.0)/(float)CSpostFlipLength;

	if (CSpreSlope > CSpostSlope)
	{
		biggerSlope = CSpreSlope;
		smallerSlope = CSpostSlope;
		flipSlopeDiff = biggerSlope/smallerSlope;
	}
	else if (CSpostSlope > CSpreSlope)
	{
		biggerSlope = CSpostSlope;
		smallerSlope = CSpreSlope;
		flipSlopeDiff = biggerSlope/smallerSlope;
	}else if (CSpreSlope == CSpostSlope)
	{
		flipSlopeDiff = 0;
	}

//flipRange = max(DepthRough(IRDMS_flipIndex2Trigger-2*pauseTime:IRDMS_flipIndex2Trigger))-min(DepthRough(IRDMS_flipIndex2Trigger-2*pauseTime:IRDMS_flipIndex2Trigger));
	Max_Flip_DepthRough=AlgorithmData->depthRough[IRDMS_flipIndex2Trigger-(2*pauseTime)];
	Min_Flip_DepthRough=AlgorithmData->depthRough[IRDMS_flipIndex2Trigger-(2*pauseTime)];
	temp_data = IRDMS_flipIndex2Trigger+1;
 	for( i = (IRDMS_flipIndex2Trigger-(2*pauseTime)); i<temp_data;i++)
	{
		if(AlgorithmData->depthRough[i] > Max_Flip_DepthRough)
		{
			Max_Flip_DepthRough = AlgorithmData->depthRough[i];
		}
		if(AlgorithmData->depthRough[i] < Min_Flip_DepthRough)
		{
			Min_Flip_DepthRough = AlgorithmData->depthRough[i];
		}
	} 
	flipRange = Max_Flip_DepthRough - Min_Flip_DepthRough;
/* 	printf("flipRange :%f\r\n",flipRange);
	printf("flipSlopeDiff :%f,flipSlopeDiffThresh=%f\r\n",flipSlopeDiff,flipSlopeDiffThresh);  */
	if ((flipSlopeDiff < flipSlopeDiffThresh)&&(flipRange>1))
	{

		for (i = CSpreFlipIndex;i<(IRDMS_flipIndex2Trigger);i++)
		{
			AlgorithmData->depthRough[i+1] = AlgorithmData->depthRough[i]-CSpreSlope;
		}
		#if ENABLE_PRINTF_DEPTHROUGH_F
	 	printf("AlgorithmData->depthRough F \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif
		for (i = (IRDMS_flipIndex2Trigger+1);i<(CSpostFlipIndex);i++)
		{
			AlgorithmData->depthRough[i+1] = AlgorithmData->depthRough[i]-CSpostSlope;
		}

	}
	
		#if ENABLE_PRINTF_DEPTHROUGH_G
	 	printf("AlgorithmData->depthRough G \r\n");
		for( i = 0; i<depthRough_length;i++)
		{
			printf("%f\r\n",AlgorithmData->depthRough[i]);
		}
		printf("\r\n"); 
		#endif
	
	AlgorithmData->finalDepth = AlgorithmData->depthRough;
 }



/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Sampling_Rate_Dependent_Parameters
 * Brief:       This function calculates algorithm processing processing parameters which depends sampling rate. 
 * Input:   SamplingRate
 * Return:      None, 
 * Outputs: IRDMSmovingMinSize, PressureaverageSize,PressureMod_averageSize,IRDMSaverageSize, QRDmedianSize, DepthSmoothLength, pauseTime,speedAverageSize
 -----------------------------------------------------------------------------*/
static void Calculate_Sampling_Rate_Dependent_Parameters()
{
	uint16_t SamplingRate=0;
	SamplingRate = (Settings_Data.Sampling_Rate*1000);
	
	PressureAverageSize = (uint16_t)((SamplingRate / 300.0)+1.5);
	//make sure that sampling rate dependent parameters are always odd.
	if((PressureAverageSize % 2)== 0) 
		PressureAverageSize++;
		
	PressureMod_averageSize = (uint16_t)((SamplingRate/10.0)+1.5);
	//make sure that sampling rate dependent parameters are always odd.
	if((PressureMod_averageSize % 2)== 0) 
		PressureMod_averageSize++;

	IRDMSaverageSize = (uint16_t)((SamplingRate / 13.0)+1.5);
	//make sure that sampling rate dependent parameters are always odd.
	if((IRDMSaverageSize % 2)== 0)
		IRDMSaverageSize++;
		
	QRDmedianSize = (uint16_t)((SamplingRate / 200.0)+1.5); 
//	if((QRDmedianSize % 2)== 0)
//		QRDmedianSize++;
		

	DepthSmoothLength = (uint16_t)((SamplingRate/20.0)+0.5);
	if((DepthSmoothLength % 2)== 0)
		DepthSmoothLength++;

	pauseTime = (uint16_t)((SamplingRate/20.0)-0.5);
	
	speedAverageSize = (SamplingRate/2.0);
	
	fastsmoothWinSize = (uint16_t)((SamplingRate/4.0)+0.5);

	#if ENABLE_PRINTF_SAMPLING_RATE_DEPENDENT_PARAMETERS
	
	printf("\r\n\r\n Sampling Rate Dependent parameters:");
	printf("\r\n************************************");
	printf("\r\n SamplingRate : %d",SamplingRate); 
	printf("\r\n PressureAverageSize : %d",PressureAverageSize); 
	printf("\r\n PressureMod_averageSize : %d",PressureMod_averageSize); 
	printf("\r\n IRDMSaverageSize : %d",IRDMSaverageSize);
	printf("\r\n QRDmedianSize : %d",QRDmedianSize);
	printf("\r\n DepthSmoothLength : %d",DepthSmoothLength);
	printf("\r\n pauseTime : %d",pauseTime);
	printf("\r\n speedAverageSize : %d",speedAverageSize);
	printf("\r\n fastsmoothWinSize : %d",fastsmoothWinSize);
	
	printf("\r\n\r\n"); 
		
	#endif

}
/*-----------------------------------------------------------------------------* 
 * Function:    Normalize_Initial_Pressure_Voltages
 * Brief:       The goal of this step is to reduce noise from the Pressure Sensor.
 * Input:   	Vpressure,SamplingRate, Calibration_Time and PoffsetV
 * Output:      Modified Vpressure
 -----------------------------------------------------------------------------*/
static void Normalize_Initial_Pressure_Voltages(void)
{
	double Vpressure0_total=0;
	float VPIC=0;
	uint32_t sample_count = (uint32_t)(Settings_Data.Sampling_Rate * Settings_Data.Calibration_Time *1000);
	uint32_t i=0;

	if(sample_count==0)
	{
		sample_count =1;
	}
	
	for(i=0; i<sample_count; i++)
	{
		Vpressure0_total += AlgorithmData->V_pressure[i];
	}
	VPIC = (float)Vpressure0_total/(float)sample_count;
	
	for(i=0; i<Acq_Data_Count; i++)
	{
		 AlgorithmData->V_pressure[i] = AlgorithmData->V_pressure[i]+ PoffsetV - VPIC;
	}
	#if ENABLE_PRINTF_NORMALIZE_INITIAL_PRESSURE_VOLTAGES
		printf(" VPIC\t:\t%f\r\n",VPIC);
		printf("\r\n\r\nV_Pressure_Normal_Device");
		for(i=0;i<Acq_Data_Count;i++)
		{
			printf("\r\n%f",AlgorithmData->V_pressure[i]);
		}
	#endif 
}

/*-----------------------------------------------------------------------------* 
 * Function:    Smooth_Pressure_Sensor
 * Brief:       The goal of this step is to reduce noise from the Pressure Sensor.
 *              Apply a median filter of size PressureAverageSize to the Vpressure column.
 * Parameter:   Vpressure,PressureAverageSize
 * Return:      status and also generate VpressureSmooth file
 -----------------------------------------------------------------------------*/
static void Smooth_Pressure_Sensor(void)
{
	uint32_t i=0; 
	float temp_Avg=0, temp_vpressure_smooth_minimum = 0;
	double  temp_Total = 0;

#if ENABLE_PRINTF_SMOOTHING_PRESSURE
	
	printf("\r\n Smoothing Pressure :");
	printf("\r\n************************************");

#endif

	/*Running Average of first Pressure size*/
	for( i=0 ; i< (PressureAverageSize+1) ; i++ )
	{
		temp_Total += AlgorithmData->V_pressure[i];
	}
	temp_Avg = ((temp_Total / (float)(PressureAverageSize+1)));
	AlgorithmData->V_pressureSmooth[0] = temp_Avg;
	temp_vpressure_smooth_minimum =temp_Avg;
	/* Calculating Running Average */ 
	for( ;i<Acq_Data_Count;i++)
	{  	
		/* Calculating pressure smooth min */
		if (temp_Avg < temp_vpressure_smooth_minimum)
		{
			temp_vpressure_smooth_minimum = temp_Avg;

		}

		temp_Total -= AlgorithmData->V_pressure[i - (PressureAverageSize+1)];
		temp_Total += AlgorithmData->V_pressure[i];
		
		temp_Avg = (float)((temp_Total / (float)(PressureAverageSize+1)));
		AlgorithmData->V_pressureSmooth[(i+1) - (PressureAverageSize+1)] = temp_Avg ;

	}

	/*Filling zero for last Pressure size elements in the force sensor data*/
	for(i=(Acq_Data_Count-(PressureAverageSize)) ; i < Acq_Data_Count ;i++ )
	{

		AlgorithmData->V_pressureSmooth[i] = 0;

	}
	/* Calculating Vpressure Smooth min */
	Vpressure_smooth_min = temp_vpressure_smooth_minimum ;    


#if ENABLE_PRINTF_SMOOTHING_PRESSURE	
	printf("\n\rV_pressureSmooth_Device");

	for( i=0 ; i< Acq_Data_Count ; i++ )
	{
		printf("\n\r%0.8f",AlgorithmData->V_pressureSmooth[i] );			
	}
	
	
	printf("\r\n\r\n");
	printf("\n\rVpressure_smooth_min=%0.8f\r\n",Vpressure_smooth_min);
#endif


}


/*-----------------------------------------------------------------------------* 
 * Function:    Modify_V_IRDMS
 * Brief:       The goal of this step is to reduce noise from the IRDMS
 * Input:  	 Irdms raw data file
 * Output:      V_IRDMSMovingMin file
 -----------------------------------------------------------------------------*/
static void Modify_V_IRDMS(void)
{

	uint32_t i=0;
	float 	V_IRDMS0 = 0.0;
	float   V_IRDMSdiff = 0.0;
	uint16_t	IRDMS_Avarage_Count = 0;
	double	temp_sum = 0;
	uint32_t IRDMStimeDelay=0;
	
	float 	V_IRDMSoffsetV = V_IRDMS_OFFSET_V;
	
	#if ENABLE_PRINTF_IRDMS_COMP	
	printf("\r\n vIrdmsCompT");
	#endif
	
	IRDMS_Avarage_Count = ( Settings_Data.Calibration_Time  * Settings_Data.Sampling_Rate*1000 );	
	for(i=0; i < IRDMS_Avarage_Count ; i++)
	{		
		temp_sum += AlgorithmData->V_IRDMS[i];
	}

	V_IRDMS0 	  = ((float)(temp_sum/(float)IRDMS_Avarage_Count));	
	V_IRDMSdiff   = V_IRDMSoffsetV - V_IRDMS0;
	#if ENABLE_PRINTF_IRDMS_COMP	
	printf("\t V_IRDMS0=%f",V_IRDMS0);
	printf("\t_IRDMSdiff=%f",V_IRDMSdiff);
	#endif
		//compensate for IRDMS time delay:
	IRDMStimeDelay = ((Settings_Data.Sampling_Rate*1000)/(42.3))+0.5;
	vIrdmsComp_length=0;
	for(i=IRDMStimeDelay-1; i < Acq_Data_Count ; i++)
	{			  
		AlgorithmData->vIrdmsComp[vIrdmsComp_length++] = (AlgorithmData->V_IRDMS[i]) + V_IRDMSdiff;

	}

	for( i = 0;i<Trigger_ROS;i++)
	{
		AlgorithmData->vIrdmsComp[i]=V_IRDMS_OFFSET_V;
	}
	#if ENABLE_PRINTF_IRDMS_COMP	
	for(i=0; i < vIrdmsComp_length ; i++)
	{	
		printf("\r\n%0.8f",AlgorithmData->vIrdmsComp[i]);
	}
	#endif
	return ;  
}


/*-----------------------------------------------------------------------------* 
 * Function:    fastsmoothany
 * Brief:       The goal of this step is to reduce noise from the IRDMS
 * Parameter:   
 * Return:      None 
 -----------------------------------------------------------------------------*/
void fastsmoothany(const float *Y,const uint32_t Length_Y,float W, float *SmoothY)
{

	int32_t L = Length_Y;
	int32_t i=0,j=0,k=0,count=0;
	int32_t halfW=0,winLength=0;
	double sumWindow=0;
	float val=0;
		//SmoothY=zeros(size(Y));
	//SmoothY[0] = 0.393648;
	
	uint8_t fill_status=0; //Used to fillup some data for repeated if loop
	
	for (i=0;i<L;i++)
	{

		
	   halfW=(W/2.0)+0.5;
	   if ((i-halfW) < 1)
	   {
		   //sumWindow = sum(Y(1:i+halfW));

	   		if(fill_status==1)
			{
				sumWindow += Y[(i-1)+halfW];
			}
			else
			{
				fill_status=1;
				printf("\r\n Refill Sum %d",fill_status);
				
				sumWindow =0;
				for(k=0;k<(i+halfW);k++)
				{
					sumWindow += Y[k];
				
				}
			}
		   winLength = (i+halfW)  ;//length(Y(i-halfW:end));
		   val = sumWindow/(float)winLength;
		}
	   else if ((i+halfW) > L)
	   {
	   		if(fill_status==2)
			{
				sumWindow -= Y[(i-1)-halfW];
			}
			else
			{
				printf("\r\n Refill Sum %d",fill_status);
				fill_status=2;
				sumWindow =0;
				for(k=(i-halfW);k<Length_Y;k++)
				{
					sumWindow += Y[k];
				
				}
			}
		   winLength = Length_Y -(i-halfW)  ;//length(Y(i-halfW:end));
		   val = sumWindow/(float)winLength;
	
		}
	   else
	   {
		  // sumWindow = sum(Y(i-halfW:i+halfW));
		  	if(fill_status==3)
			{
				sumWindow -= Y[(i-1)-halfW];
				sumWindow += Y[i+halfW];
			}
			else
			{
				printf("\r\n Refill Sum %d",fill_status);
				fill_status=3;
				sumWindow =0;
				count=i+halfW+1;
				for(k=(i-halfW);k<count;k++)
				{
					sumWindow += Y[k];
				}
			}
			
		   val = sumWindow/(float)((2*halfW)+1);
		   

	   }
	   SmoothY[i] = val;
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    fastsmoothchange
 * Brief:       The goal of this step is to reduce noise from the IRDMS
 * Parameter:   
 * Output:      Smooth sensor values 
 -----------------------------------------------------------------------------*/
void fastsmoothchange(const float *Y,const uint32_t Length_Y,float W, float *SmoothY)
{

	int32_t L = Length_Y;
	float W1 = W;
	int32_t i=0,j=0,k=0,count=0;
	int32_t halfW=0,winLength=0;
	double sumWindow=0;
	float val=0;
	
	uint8_t fill_status=0; //Used to fillup some data for repeated if loop
	
	//SmoothY=zeros(size(Y));
	SmoothY[0] = 0.393648;
	
	for (i=1;i<L;i++)
	{
	
/* 		if (i == 0)
		{
		   j=0;
		}
		else
		{
		   j=i-1;
		} */
		
		if (SmoothY[j] < stopVal) //stop decreasing window at SmoothY > 2
		{
			W = W1+(((W1/(float)(decFrac-W1))/(stopVal-SmoothY[0]))*(SmoothY[j]-SmoothY[0])); //comment out this line for constant winsize = W

		}
		
	   halfW=(W/2.0)+0.5;
	   if ((i-halfW) < 1)
	   {
		   //sumWindow = sum(Y(1:i+halfW));
		   val = 0.393648;
		   fill_status=1;
		}
	   else if ((i+halfW) > L)
	   {
			if(fill_status==2)
			{
				sumWindow -= Y[(i-1)-halfW];
			}
			else
			{
			//	printf("\r\n Refill Sum %d",fill_status);
				fill_status=2;
				sumWindow =0;
				for(k=(i-halfW);k<Length_Y;k++)
				{
					sumWindow += Y[k];
				
				}
		   }
		   winLength = Length_Y -(i-halfW)  ;//length(Y(i-halfW:end));
		   val = sumWindow/(float)winLength;
			
		}
	   else
	   {
			if(fill_status==3)
			{
				sumWindow -= Y[(i-1)-halfW];
				sumWindow += Y[i+halfW];
			}
			else
			{
			//  printf("\r\n Refill Sum %d",fill_status);
				fill_status=3;
				sumWindow =0;
				count=i+halfW+1;
				for(k=(i-halfW);k<count;k++)
				{
					sumWindow += Y[k];
				}
	 
			}
		   val = sumWindow/(float)((2*halfW)+1);
			
	   }
	   SmoothY[i] = val;
	   
	}
		
}


/*-----------------------------------------------------------------------------* 
 * Function:    fastsmooth
 * Brief:       The goal of this step is to reduce noise from the IRDMS
 * Parameter:   
 * Return:      None 
 -----------------------------------------------------------------------------*/
void fastsmooth(const float *Y,const uint32_t Length_Y,float W, float *SmoothY)
{

	int32_t L = Length_Y;
	int32_t i=0,j=0,k=0,count=0;
	int32_t halfW=0,winLength=0;
	double sumWindow=0;
	float val=0;
		//SmoothY=zeros(size(Y));
	//SmoothY[0] = 0.393648;
	
	uint8_t fill_status=0; //Used to fillup some data for repeated if loop
	
	for (i=0;i<L;i++)
	{

		
	   halfW=(W/2.0)+0.5;
	   if ((i-halfW) < 1)
	   {
		   //sumWindow = sum(Y(1:i+halfW));
		   val = 0.393648;
		   fill_status=1;
		}
	   else if ((i+halfW) > L)
	   {
	   		if(fill_status==2)
			{
				sumWindow -= Y[(i-1)-halfW];
			}
			else
			{
				printf("\r\n Refill Sum %d",fill_status);
				fill_status=2;
				sumWindow =0;
				for(k=(i-halfW);k<Length_Y;k++)
				{
					sumWindow += Y[k];
				
				}
			}
		   winLength = Length_Y -(i-halfW)  ;//length(Y(i-halfW:end));
		   val = sumWindow/(float)winLength;
	
		}
	   else
	   {
		  // sumWindow = sum(Y(i-halfW:i+halfW));
		  	if(fill_status==3)
			{
				sumWindow -= Y[(i-1)-halfW];
				sumWindow += Y[i+halfW];
			}
			else
			{
				printf("\r\n Refill Sum %d",fill_status);
				fill_status=3;
				sumWindow =0;
				count=i+halfW+1;
				for(k=(i-halfW);k<count;k++)
				{
					sumWindow += Y[k];
				}
			}
			
		   val = sumWindow/(float)((2*halfW)+1);
		   

	   }
	   SmoothY[i] = val;
	}

}
/*-----------------------------------------------------------------------------* 
 * Function:    Smooth_IRDMS
 * Brief:       The goal of this step is to reduce noise from the IRDMS
 * Input:   	V_IRDMS
 * Output:      irdmsSmooth 
 -----------------------------------------------------------------------------*/

static void Smooth_IRDMS(void)
{

	float * irdmsSmooth_temp  = AlgorithmData->tempBuffer2_float;
	float * irdmsSmooth_temp2 = AlgorithmData->tempBuffer1_float;
	uint32_t i=0,belowInitCount=0,tempData=0;
	float minV_IRDMSSmooth=0;
	float 	V_IRDMSoffsetV = V_IRDMS_OFFSET_V;
	

	length_of_IRDMS_Smooth = 0;
	
	compensate_static(AlgorithmData->V_IRDMS_Raw, Acq_Data_Count, AlgorithmData->V_IRDMS, IRDMS_CALIB_PTS, IRDMS_DEVICE_PTS, NCAL_IRDMS);
//	for(i=0; i < Acq_Data_Count ; i++)
//	{
//		AlgorithmData->V_IRDMS[i]= AlgorithmData->V_IRDMS_Raw[i];
//	}
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n compensate_static @ ");
		print_rtc_time_date();	
#endif
//	take_column_data_from_file_double("VIRDMS.CSV",AlgorithmData->V_IRDMS,(uint32_t *)&Acq_Data_Count);

	#if CHECKING_ALGORTIHM_ENABLED
	//Checking Algortihm
	Trigger_ROS =6742; //For matching the next step , it is calculated 6753 in c code
	#endif
	Modify_V_IRDMS();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Modify_V_IRDMS @ ");
		print_rtc_time_date();
#endif
	fastsmoothchange(AlgorithmData->vIrdmsComp,vIrdmsComp_length,fastsmoothWinSize, irdmsSmooth_temp);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n fastsmoothchange @ ");
		print_rtc_time_date();
#endif
	length_of_IRDMS_Smooth = vIrdmsComp_length;
	
	#if ENABLE_PRINTF_IRDMS_SMOOTH1
	printf("\r\n AlgorithmData->irdmsSmooth1_Device");
		for(i=0; i < length_of_IRDMS_Smooth ; i++)
			printf("\r\n%0.8f",irdmsSmooth_temp[i]);
			
		printf("\r\n");
	#endif
	
	fastsmoothchange(irdmsSmooth_temp,length_of_IRDMS_Smooth,fastsmoothWinSize, irdmsSmooth_temp2);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n fastsmoothchange @ ");
		print_rtc_time_date();
#endif
	#if ENABLE_PRINTF_IRDMS_SMOOTH2
	printf("\r\n AlgorithmData->irdmsSmooth2_Device");
	for(i=0; i < length_of_IRDMS_Smooth ; i++)
		printf("\r\n%0.8f",irdmsSmooth_temp2[i]);
		
	printf("\r\n");
	#endif
	fastsmoothchange(irdmsSmooth_temp2,length_of_IRDMS_Smooth,fastsmoothWinSize, AlgorithmData->irdmsSmooth);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n fastsmoothchange @ ");
		print_rtc_time_date();
#endif
	
	minV_IRDMSSmooth = AlgorithmData->irdmsSmooth[0];
	tempData = Trigger_ROS + (Settings_Data.Sampling_Rate*1000) ;
	for(i=1; i< tempData;i++)
	{
		if(AlgorithmData->irdmsSmooth[i]<minV_IRDMSSmooth)
		{
		//	printf("\r\n%0.8f",AlgorithmData->irdmsSmooth[i]);
			minV_IRDMSSmooth = AlgorithmData->irdmsSmooth[i];
		}
	}
	
	if (minV_IRDMSSmooth < V_IRDMSoffsetV)
	{
	
		for(i=0; i < length_of_IRDMS_Smooth ; i++)
		{
			if(AlgorithmData->irdmsSmooth[i]<V_IRDMSoffsetV)
			{
				
				belowInitCount = i;
			}
		}
		badStartError = 1;
		for (i = 0; i<belowInitCount; i++)
		{
			AlgorithmData->irdmsSmooth[i] = V_IRDMSoffsetV;
		}

			 
		fastsmooth(AlgorithmData->irdmsSmooth,length_of_IRDMS_Smooth,fastsmoothWinSize/5.0, irdmsSmooth_temp);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
		printf("\n fastsmooth @ ");
			print_rtc_time_date();
#endif
		fastsmooth(irdmsSmooth_temp,length_of_IRDMS_Smooth,fastsmoothWinSize/10.0, AlgorithmData->irdmsSmooth);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
		printf("\n fastsmooth @ ");
			print_rtc_time_date();
#endif
	}
	else
	{
		badStartError = 0;
	}
	
	#if ENABLE_PRINTF_IRDMS_SMOOTH
		printf("\r\n AlgorithmData->irdmsSmooth_Device,badStartError=%d",badStartError);
		for(i=0; i < length_of_IRDMS_Smooth ; i++)
			printf("\r\n%0.8f",AlgorithmData->irdmsSmooth[i]);
			
		printf("\r\n");
	#endif

//	take_column_data_from_file("V_IRDMSSmooth.txt",AlgorithmData->irdmsSmooth,(uint32_t *)&vIrdmsComp_length);
	return;

}
/*-----------------------------------------------------------------------------* 
 * Function:    compare
 * Brief:       This function return the difference between 2 numbers and used as a supporting function for Q sort  
 * Parameter:   two numbers
 * Return:      difference  between two numbers
 -----------------------------------------------------------------------------*/
static int compare(const void * a, const void * b)
{
	return (*(uint16_t*) b - *(uint16_t*) a);
}

/*-----------------------------------------------------------------------------* 
 * Function:    findMedian
 * Brief:       Find median of an array of numbers which are 16 bit wide
 * Parameter:   Starting address of array and length of the array
 * Return:      median of array
 -----------------------------------------------------------------------------*/

static uint16_t findMedian(uint16_t tempSortArray[], uint8_t length_of_array)
{
	qsort(tempSortArray, length_of_array, sizeof(uint16_t), compare);
	if(length_of_array%2!=0)
	{
		return tempSortArray[(length_of_array / 2)];
	}
	else
	{
		return ((tempSortArray[(length_of_array / 2)]+tempSortArray[(length_of_array / 2)-1])/2);
	}
}
/*-----------------------------------------------------------------------------* 
 * Function:    find_Qrd_Median
 * Brief:       Find median of an array of numbers which are 32  bit wide
 * Parameter:   Starting address of array and length of the array
 * Return:      median of array
 -----------------------------------------------------------------------------*/
static uint32_t find_Qrd_Median(uint32_t tempSortArray[], uint8_t length_of_array)
{
	qsort(tempSortArray, length_of_array, sizeof(uint32_t), compare);  
	return tempSortArray[(length_of_array / 2)];

}

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Qrd_MedianMe
 * Brief:       Find median of an array of numbers which are 32  bit wide
 * Parameter:   Starting address of array and length of the array
 * Return:      median of array
 -----------------------------------------------------------------------------*/
static uint16_t Calculate_Qrd_Median(uint16_t tempSortArray[], uint8_t length_of_array)
{

	uint16_t Qrd_temp_array[QRD_MEDIAN_TEMP_ARRAY_SIZE]={0}; //QRD_MEDIAN_TEMP_ARRAY_SIZE  maximum 51
	memcpy(Qrd_temp_array,tempSortArray,length_of_array*(sizeof(uint16_t)));	
	return(findMedian((unsigned short *)Qrd_temp_array,length_of_array));

}

/*-----------------------------------------------------------------------------* 
 * Function:    Smooth_Qrd
 * Brief:       The goal of this step is to reduce noise from the QRD1114 sensor.
 *              Apply a median filter of size QRDmedianSize to the VQRD column. 
 * Parameter:   VQRD,QRDmedianSize
 * Return:      status and also generate VQRD_Smooth
 -----------------------------------------------------------------------------*/

static void Smooth_Qrd(void)
{

	uint32_t i=0,j=0,tempdata=0,IncrementedQRDmedianSize=0;
	
	tempdata = (Acq_Data_Count - QRDmedianSize); //Taking out of loop; for improving execution speed of for loop
	IncrementedQRDmedianSize = QRDmedianSize +1;
	for( i=0 ; i < tempdata+1; i++)
	{
		AlgorithmData->smoothQRD[i] =  Calculate_Qrd_Median((unsigned short *)(AlgorithmData->qrdBuff+j),IncrementedQRDmedianSize );
		j++;
	}

	for( ; i < Acq_Data_Count ; i++ )
	{
		AlgorithmData->smoothQRD[i] = 0;
	}

	#if ENABLE_PRINTF_SMOOTH_QRD
	printf("\r\n QRD_SMOOTH_DEVICE");
	for(i=0 ; i < Acq_Data_Count ; i++ )
	{
		printf("\r\n%0.8f",AlgorithmData->smoothQRD[i]*RAW_DATA_TO_VOLTAGE_MULTIPLIER); 
	}
	printf("\r\n\r\n");
	#endif


}

/*-----------------------------------------------------------------------------* 
 * Function:    Find_the_trigger
 * Brief:       The goal of this step is to identify the trigger point in the data. 
 *              This point will later be used to calibrate the IRDMS to a known depth measurement. 
 * Input:   	VQRD_Smooth column
 * Output:		Trigger_ROS
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Find_the_trigger(void)
{


	float QRDcheck = 0;
	float V_QRD_0 = 0;
	float QRDpercent = 0;
	float QRDcollum = 0;

	uint32_t V_QRD0_temp=0;
	uint32_t Trigger_point = 0;
	uint32_t Trigger_list[100] = { 0 };
	uint32_t i=0,temp_data=0;
	uint16_t Temp_Index = 0;
	
	uint16_t QRDcenteringCounts=0;

	#if ENABLE_PRINTF_FIND_THE_TRIGGER
		printf("Find_the_trigger");
	#endif
	
	QRDcenteringCounts=(uint16_t)((QRDmedianSize/2.0)+0.5);
	
	/*	Average the VQRD,Smooth signal over the range (QRDdelayCounts:QRDdelayCounts+V_QRD0_length). */
	temp_data = V_QRD0_length + QRDdelayCounts -1;	//Taking out of loop for improving speed of for loop
	for( i= QRDdelayCounts-1; i <temp_data ; i++)
	{
		V_QRD0_temp = V_QRD0_temp + AlgorithmData->smoothQRD[i];
	}

	V_QRD_0 = (V_QRD0_temp * RAW_DATA_TO_VOLTAGE_MULTIPLIER) /(float)V_QRD0_length;


	/*********************************************/

	if (V_QRD_0 > (Settings_Data.QRDbreakV))
	{
		QRDpercent = (Settings_Data.QRDpercentHigh);
	}
	else
	{
		QRDpercent = (Settings_Data.QRDpercentLow);
	}

	/* Create QRDcheck column */
	for( i= QRDdelayCounts-1; i < Acq_Data_Count ; i++)
	{    	
		QRDcollum = (AlgorithmData->smoothQRD[i]) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		QRDcheck  = ((QRDcollum - V_QRD_0) / V_QRD_0);

		/*  Find abs   */
		if (QRDcheck < 0.0)
		{ 
			QRDcheck *= -1;
		}

		//		printf("\nQRDcollum =%f\tQRDcheck  = %f",QRDcollum,QRDcheck);
		//		printf("\n%f",QRDcheck);

		if (QRDcheck > QRDpercent)
		{
			Trigger_list[Temp_Index] = i-QRDdelayCounts;
			Temp_Index++;

			if (Temp_Index >= QRD_triggerListCount)
				break;
		}  	
	}

	Trigger_point = find_Qrd_Median(Trigger_list, Temp_Index);

	Trigger_ROS = Trigger_point + QRDdelayCounts+QRDcenteringCounts;

	#if ENABLE_PRINTF_FIND_THE_TRIGGER
		printf("\nV_QRD_0 =%f",V_QRD_0);
		printf("\n Trigger_ROS %d \r\n", Trigger_ROS);
	#endif

}


/*-----------------------------------------------------------------------------* 
 * Function:    Discretize_Data_and_Scale_for_Graphing
 * Brief:       The goal of this step is to simplify the data into a graph-ready subset with exactly 1505 rows of data. 
 *              Each row represents approximately a one-millimeter depth increment. 
 *              The pressure value in each row is the minimum pressure seen over that one-millimeter increment.
 *              The goal of Scale_for_Graphing step is to rescale the pressure values to a pixel width for the graph. 
 *              When drawing the graph, we will overlay the graph information onto a bitmap image of the graph axis.
 * Parameter:   depth and pressure values
 * Return:      None, pressure pixel values are stored in global buffers
 -----------------------------------------------------------------------------*/

static void Discretize_Data_and_Scale_for_Graphing()
{
	uint16_t index=0;
	double temp_total = 0;
	uint32_t i=0;
	
	for(i=0;i<depthRough_length;i++)
	{
		index = abs((int16_t) (AlgorithmData->finalDepth[i] ));
		if (index <= EndDepth)
		{

			if (AlgorithmData->pressureMeans_count[index] == 0)
			{
				AlgorithmData->pressureMeans[index] = AlgorithmData->pressureNew[i];
				AlgorithmData->pressureMeans_count[index] = 1;
			}
			else
			{

				temp_total = (AlgorithmData->pressureMeans[index] * AlgorithmData->pressureMeans_count[index]) + AlgorithmData->pressureNew[i];

				AlgorithmData->pressureMeans[index] = (temp_total / (float) (AlgorithmData->pressureMeans_count[index] + 1));
				AlgorithmData->pressureMeans_count[index]++;

			}

		}
	
	}
	
	PressureMeans1Min = AlgorithmData->pressureMeans[0];

	for (i = 1; i < EndDepth; i++)
	{
		if (AlgorithmData->pressureMeans_count[i] == 0)
		{
			AlgorithmData->pressureMeans[i]=AlgorithmData->pressureMeans[i-1];
		}
		
		if(AlgorithmData->pressureMeans[i]<PressureMeans1Min)
		{
			PressureMeans1Min = AlgorithmData->pressureMeans[i];
		}
	} 
	
	if (PressureMeans1Min < 0)
	{
		
		for (i = 0; i < EndDepth; i++)
		{
			AlgorithmData->pressureMeans[i] = AlgorithmData->pressureMeans[i] - PressureMeans1Min; //PressureMeans1 = PressureMeans1+abs(PressureMeans1Min);
			
		}

	}
	for (i = EndDepth; i < DEPTH_RAW_COUNT; i++)
	{
		AlgorithmData->pressureMeans[i]=0;
	} 
	#if ENABLE_PRINTF_PRESSURE_MEANS
	printf("AlgorithmData->pressureMeans\r\n");
	for (i = 0; i < DEPTH_RAW_COUNT; i++)
	{
		printf("%0.8f\r\n",AlgorithmData->pressureMeans[i]);
	}
	#endif
}
/*-----------------------------------------------------------------------------* 
 * Function:    Print_Pressure_Blocky_Pixels
 * Brief:       Debug print of Blocky representation
 * Input: 		nPressure_BlockyPixels
 * Output:      Display on console
 -----------------------------------------------------------------------------*/
static void  Print_Pressure_Blocky_Pixels(void)
{
	uint16_t i=0;


	#if ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_1
	printf("Pressure_BlockyPixe1\r\n");
	for (i = GRAPH_RAW_PIXEL_COUNT; i < (2*GRAPH_RAW_PIXEL_COUNT); i++)
	{
		printf("%d\r\n",Pixels_buffer[i]);
	}
	#endif
	

	#if ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_2
	printf("Pressure_BlockyPixe2\r\n");
	for (i = (2*GRAPH_RAW_PIXEL_COUNT); i < (3*GRAPH_RAW_PIXEL_COUNT); i++)
	{
		printf("%d\r\n",Pixels_buffer[i]);
	}
	#endif
	

	#if ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_3
	printf("Pressure_BlockyPixel 3\r\n");
	for (i = (3*GRAPH_RAW_PIXEL_COUNT); i < (4*GRAPH_RAW_PIXEL_COUNT); i++)
	{
		printf("%d\r\n",Pixels_buffer[i]);
	}
	#endif
	
	#if ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_4
	printf("\r\nPressure_BlockyPixel 4\r\n");
	for (i = (4*GRAPH_RAW_PIXEL_COUNT); i < (5*GRAPH_RAW_PIXEL_COUNT); i++)
	{
		printf("%d\r\n",Pixels_buffer[i]);
	}
	#endif
}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_Pressure_Blocky_Pixels
 * Brief:       Create Blocky representation from Pixelated means data
 * Parameter:   Address of buffer which graph data to be plotted ,xfraction
 * Output:      pressureBlockyPixels column
 -----------------------------------------------------------------------------*/

static void Create_Pressure_Blocky_Pixels(uint16_t *BlockyPixels_buff_addr , float xfraction)
{
	float temp_pixel= 0.0;
	//uint16_t changeCountBlockyPixels = 0;	
	uint16_t i=0;
	
	memcpy((void *)AlgorithmData->pressureBlockyPixels,(void *)AlgorithmData->PressurePixels1,GRAPH_RAW_PIXEL_COUNT*sizeof(float));

	for(i=0; i< GRAPH_RAW_PIXEL_COUNT-1 ; i++)
	{
		
		if(((uint16_t)(AlgorithmData->pressureBlockyPixels[i])) != 0)
		{
			temp_pixel =  1- (AlgorithmData->pressureBlockyPixels[i+1]/AlgorithmData->pressureBlockyPixels[i]);

			//find abs
			if(temp_pixel < 0.0)
			{
				temp_pixel*= -1;
			}

			if (temp_pixel < xfraction)
			{

				AlgorithmData->pressureBlockyPixels[i+1]= AlgorithmData->pressureBlockyPixels[i];
			}
		/* 	else  
			{	
				changeCountBlockyPixels  = changeCountBlockyPixels + 1;
			} */
		}
		
		BlockyPixels_buff_addr[i]=(uint16_t)(AlgorithmData->pressureBlockyPixels[i]+0.5);
		

		
	}
//For last data
		i = GRAPH_RAW_PIXEL_COUNT-1;
		BlockyPixels_buff_addr[i]=(uint16_t)(AlgorithmData->pressureBlockyPixels[i]+0.5);

}

/*-----------------------------------------------------------------------------* 
 * Function:    CreatePixelatedMeans 
 * Brief:      In order to get the 1500 points printed to the graph’s 300 (Graph_height) rows of pixels, 
				please graph the round of the MEAN of every 5 	expandedPressureLog value., 
				resulting in a vector as long as Graph_height. This is considered the Pixelated means data:
 * Input:   	expandedPressureLog
 * Output:      PressurePixels1
 -----------------------------------------------------------------------------*/
void CreatePixelatedMeans(void)
{
	uint16_t i=0, j=0;
	double temp_mean = 0;


	for (i = 0; i < GRAPH_RAW_PIXEL_COUNT; i++)
	{
		temp_mean = 0;

		for (j = 0; j < RAW_AVERAGE_COUNT; j++)
		{

			temp_mean += AlgorithmData->expandedPressureLog[(i * RAW_AVERAGE_COUNT) + j];
		}
		AlgorithmData->PressurePixels1[i] = temp_mean/RAW_AVERAGE_COUNT;
		Pixels_buffer[i] = (uint16_t) (AlgorithmData->PressurePixels1[i] +0.5 );
		
	}
	
	#if ENABLE_PRINTF_PRESSURE_PIXELS
	printf("PressurePixels1\r\n");
	for (i = 0; i < GRAPH_RAW_PIXEL_COUNT; i++)
	{
		printf("%0.8f\r\n",AlgorithmData->PressurePixels1[i]);
	}
	#endif
}


/*-----------------------------------------------------------------------------* 
 * Function:    full_resolution_creation_force
 * Brief:       The goal of this step is to extract valid data from force column
 * Input:   	Pressure column
 * Return:      pressureNew
 -----------------------------------------------------------------------------*/
void full_resolution_creation_force(void)
{
	uint32_t index=0,pressureNew_length=0;
	
	for (index=Trigger_Index;pressureNew_length<depthRough_length;index++)
	{
		AlgorithmData->pressureNew[pressureNew_length++]=AlgorithmData->Pressure[index];
	}

	#if ENABLE_PRINTF_FINAL_DEPTH_PRESSURENEW
		printf("\n\r AlgorithmData->finalDepth_device \tAlgorithmData->pressureNew");
	for(index =0; index < depthRough_length ; index++)
	{	
		printf("\r\n %0.8f \t %0.8f",AlgorithmData->finalDepth[index],AlgorithmData->pressureNew[index]);
		
		//AlgorithmData->processedData[process_index++]	= AlgorithmData->pressureNew[index];
		//AlgorithmData->processedData[process_index++]	= AlgorithmData->finalDepth[index];	
	}
	#endif

}

/*-----------------------------------------------------------------------------* 
 * Function:    Expand_Pressure_Data
 * Brief:       rescale pressure from 8-bit value (between 0 and 255) for decompression for processed data file
 * Input:   	compressedPressure 
 * Output:		Expanded Pressure Means (1500 column) 
 -----------------------------------------------------------------------------*/
void Expand_Pressure_Data(void)
{
	uint16_t i=0;
	
	for (i = 0; i<DEPTH_RAW_COUNT;i++)
	{
		AlgorithmData->expandedPressure[i] = (0.00008 * (pow(AlgorithmData->compressedPressure[i],3)));
	}
	#if ENABLE_PRINTF_EXPANDED_PRESSURE
	printf("AlgorithmData->expandedPressure\r\n");
	for (i = 0; i < DEPTH_RAW_COUNT; i++)
	{
		printf("%f\r\n",AlgorithmData->expandedPressure[i]);
	}
	#endif
	
//	for (i = 0; i < EndDepth; i++)
	for (i = 0; i < DEPTH_RAW_COUNT; i++)
	{
	
		AlgorithmData->expandedPressureLog[i] = (A_Pr* pow(B_Pr,(-C_Pr*AlgorithmData->expandedPressure[i])))+(D_Pr*AlgorithmData->expandedPressure[i])+E_Pr;
	
	}
/* 	for (i = EndDepth; i < DEPTH_RAW_COUNT; i++)
	{
		AlgorithmData->expandedPressureLog[i] =0;
	} */
}
/*-----------------------------------------------------------------------------* 
 * Function:    Compress_Pressure_Data
 * Brief:       scale pressure into 8-bit value (between 0 and 255) for compression for processed data file
 * Input:   	Pressure Means (1500 column)
 * Output:		compressedPressure  
 -----------------------------------------------------------------------------*/
void Compress_Pressure_Data(void)
{

	uint16_t tempData=0;
	uint16_t i=0;
	
	for (i = 0; i<EndDepth;i++)
	{
		
		tempData = (uint16_t)((5*powf(10,(2/3.0))*powf(AlgorithmData->pressureMeans[i],(1/3.0)))+0.5);
		if (tempData > 255)
		{
			AlgorithmData->compressedPressure[i] = 255;
		}else
		{
			AlgorithmData->compressedPressure[i] = (uint8_t)tempData;
		
		}  
	}
	for (i = EndDepth; i<DEPTH_RAW_COUNT;i++)
	{
		AlgorithmData->compressedPressure[i] = 0;
	}
	
	#if ENABLE_PRINTF_COMPRESSED_PRESSURE
	printf("AlgorithmData->compressedPressure\r\n");
	for (i = 0; i < DEPTH_RAW_COUNT; i++)
	{
		printf("%d\r\n",AlgorithmData->compressedPressure[i]);
	}
	#endif
}



/*-----------------------------------------------------------------------------* 
 * Function:    Finding_End_Of_The_Test
 * Brief:       It will find the end of test 
 * Input:   	V_IRDMSSmooth
 * Output:		EndDepth  
 * Return:      None 
 -----------------------------------------------------------------------------*/
static void Finding_End_Of_The_Test(void)
{
	uint8_t p = 0,n=0; 
	uint32_t k = 0,delayRejectIndex = 0,s=0;
	uint32_t EndIndexDefault = 0;
	uint32_t i=0,j=0;

	float maxDepth=0.0,minDepth=0.0;
	
	if(depthRough_length <= pauseTime)
	{
	 EndIndex =0;
	 printf("\ndepthRough_length is less than pauseTime\n");
	 return;
	}

	#if ENABLE_PRINTF_FINDING_END_OF_THE_TEST 

		printf("\r\n Finding_End_Of_The_Test ");
		printf("\r\n depthRough_length:\t%d ",depthRough_length);
		printf("\r\n Trigger_Index:\t%d ",Trigger_Index);
		printf("\r\n pauseIRDMSignoreV:\t%f ",pauseIRDMSignoreV);
		printf("\r\n pauseTime:\t%d ",pauseTime);
		printf("\r\n depthRough_length:\t%d ",depthRough_length);
		printf("\r\n depthRangeMin:\t%f ",depthRangeMin);

	#endif

	while( p == 0)
	{
		if (k == (vIrdmsComp_length-1))
		{
				p = 1;
				delayRejectIndex = k - Trigger_Index;
		}
		else if (AlgorithmData->irdmsSmooth[k] < pauseIRDMSignoreV)
		{
			k = k + 1; 
		}
		else
		{
			p = 1;
			delayRejectIndex = k - Trigger_Index;
		}  
	}

	EndIndexDefault = depthRough_length-1;
	EndIndex = EndIndexDefault;
	memset(( float *)AlgorithmData->depthRange,0x00,(MAX_PROCESSING_DATA_COUNT*sizeof(float)));
	
	for( i=0;i<depthRough_length-pauseTime;i++)
	{
		maxDepth = AlgorithmData->finalDepth[i];
		minDepth = AlgorithmData->finalDepth[i];
		for(j=i;j<=(i+pauseTime);j++)
		{
			if(maxDepth<AlgorithmData->finalDepth[j])
			{
				maxDepth = AlgorithmData->finalDepth[j];
			}
			if(minDepth>AlgorithmData->finalDepth[j])
			{
				minDepth = AlgorithmData->finalDepth[j];
			}
		}
		AlgorithmData->depthRange[i] = maxDepth-minDepth;
	}
	
 	#if ENABLE_PRINTF_DEPTHRANGE 
	printf("\r\n AlgorithmData->depthRange");
	for( i=0;i<(depthRough_length-pauseTime);i++)
	{
		printf("\r\n %0.8f",AlgorithmData->depthRange[i]);
	}
	#endif 

	
	if (delayRejectIndex < 1)
	{
		delayRejectIndex = 0;
	}
	s = delayRejectIndex;

	n=0;

	while( n == 0)
	{
	
		if (AlgorithmData->depthRange[s] < depthRangeMin)
		{
	 
			EndIndex = s;

 			n=1;
			
		}else
		{
			s = s+1;
		}
		if (s==(EndIndexDefault-10)) //buffer of 10 to make sure we get there and end the loop in all cases
		{
			n=1;
		}  
		
	} 
	if(AlgorithmData->finalDepth[EndIndex-1]<0)
	{
		EndDepth = 0-((AlgorithmData->finalDepth[EndIndex-1]));
	}
	else
	{
		EndDepth = ((AlgorithmData->finalDepth[EndIndex-1]));
		
	}
	
	if(EndDepth>DEPTH_RAW_COUNT-1)
	{
		EndDepth=DEPTH_RAW_COUNT-2;
		
	}
	
#if ENABLE_PRINTF_FINDING_END_OF_THE_TEST 
	printf("\r\n EndDepth:%d\r\n",EndDepth);
	printf("\r\n EndIndex:%d\r\n",EndIndex);
#endif
}

/*-----------------------------------------------------------------------------* 
 * Function:    find_CS_Parameters
 * Brief:       Calculate Parameters and Trigger_CS based on a constant speed assumption
 * Input:   	V_IRDMSSmooth
 * Output:		EndDepth  
 -----------------------------------------------------------------------------*/
void find_CS_Parameters(void)
{
	triggerOffset = Lprobe - Dtrigger_A;
	
	countsCSV = indexCSV - Trigger_ROS ;
	countsCSdepth = indexCSdepth - Trigger_ROS;

	CSdepth = AlgorithmData->depthRoughFull[indexCSdepth];
	CSslope = (CSdepth+triggerOffset)/(float)countsCSdepth;

	extrapCounts =(int32_t) ((-triggerOffset)/CSslope)+0.5;
	//Trigger_CS = Trigger_ROS - extrapCounts;
	countsCStotal = extrapCounts + countsCSdepth; 
	// Use rise trigger instead of any CS assumptions
	Trigger_CS = Trigger_Rise;
	Trigger_Index = Trigger_Rise;
#if ENABLE_PRINTF_FIND_CS_PARAMETERS
	printf("\r\n Trigger_ROS 	:\t %d",Trigger_ROS);
	printf("\r\n Trigger_Rise 	:\t %d",Trigger_Rise);
	//Trigger_Shifted	Trigger_Index	EndIndex
	printf("\r\n triggerOffset  :\t%d",triggerOffset);
	printf("\r\n IRDMSCSdepth   :\t%f",IRDMSCSdepth);
	printf("\r\n indexCSdepth   :\t%d",indexCSdepth);
	printf("\r\n indexCSV       :\t%d",indexCSV);
	printf("\r\n countsCSV      :\t%d",countsCSV);
	printf("\r\n countsCSdepth  :\t%d",countsCSdepth);
	printf("\r\n CSdepth        :\t%f",CSdepth);
	printf("\r\n CSslope        :\t%f",CSslope);
	printf("\r\n extrapCounts   :\t%d",extrapCounts);
	printf("\r\n Trigger_CS     :\t%d",Trigger_CS);
	printf("\r\n countsCStotal  :\t%d\r\n",countsCStotal);
#endif
}

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Pressure
 * Brief:       This function will calculate Pressure column from raw data column .
 * Input:       V pressure column 
 *				Acq_Data_Count  :Acquisition data count
 *          	PressureAverageSize
 * Output:    	forceModeled.
 -----------------------------------------------------------------------------*/
static void Calculate_Pressure(void)
{
	uint32_t i=0; 
	float temp_float_data=0;//, temp_vpressure_smooth_minimum = 0xffff;
	//double temp_Total = 0;
	
	//Model V_pressure :-This step uses “Pranjal’s model”, a modeling tool developed by a consultant that adjusts Pressure based on experimental calibration.
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Before adjust @ ");
	print_rtc_time_date();
#endif
	adjust(AlgorithmData->V_pressure, Acq_Data_Count, AlgorithmData->pranjals_model_result);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n adjust @ ");
	print_rtc_time_date();
#endif
	#if ENABLE_PRINTF_PRANJALS_ALGORITHM
	printf("\r\n\r\nAlgorithmData->V_pressure,Pranjal's Output");
	for(i=0;i<Acq_Data_Count;i++)
	{
		printf("\r\n%f,%f",AlgorithmData->V_pressure[i],AlgorithmData->pranjals_model_result[i]);
	}
	#endif
	
	#if ENABLE_PRINTF_SMOOTHING_PRESSURE_RESULT
	
	printf("\r\n Smoothing Pranjals_Model_Result :");
	printf("\r\n************************************");

	#endif

//	/*Running Average of first Pressure size*/
//	for( i=0 ; i< (PressureAverageSize+1) ; i++ )
//	{
//		temp_Total += AlgorithmData->pranjals_model_result[i];
//	}
//	temp_float_data = (float)(temp_Total / (float)(PressureAverageSize+1));
//	AlgorithmData->forceModeled[0] = temp_float_data;
//
//	/* Calculating Running Average */ 
//	for( ;i<Acq_Data_Count;i++)
//	{  	
//		/* Calculating pressure smooth min */
////	if (temp_float_data < temp_vpressure_smooth_minimum)
////		{
////			temp_vpressure_smooth_minimum = temp_float_data;
////
////		}
//
//		temp_Total -= AlgorithmData->pranjals_model_result[i - (PressureAverageSize+1)];
//		temp_Total += AlgorithmData->pranjals_model_result[i];
//		temp_float_data = (float)(temp_Total / (float)(PressureAverageSize+1));
//		AlgorithmData->forceModeled[(i+1) - (PressureAverageSize+1)] = temp_float_data ;
//
//	}
//
//	/*Filling zero for last Pressure size elements in the force sensor data*/
//	for(i=(Acq_Data_Count-(PressureAverageSize+1)) ; i < Acq_Data_Count ;i++ )
//	{
//
//		AlgorithmData->forceModeled[i] = 0;
//
//	}
	/* Calculating Vpressure Smooth min */
//	Vpressure_smooth_min = temp_vpressure_smooth_minimum;    
		
	for(i= 0; i < Acq_Data_Count ;i++ )
	{
	
		 AlgorithmData->tempBuffer1_float[i] = AlgorithmData->pranjals_model_result[i];
	}
	fastsmoothany(AlgorithmData->tempBuffer1_float,Acq_Data_Count,PressureAverageSize, AlgorithmData->forceModeled);

	#if ENABLE_PRINTF_SMOOTHING_PRESSURE_RESULT	
	printf("\n\r forceModeled");
	for( i=0 ; i< Acq_Data_Count ; i++ )
	{
		printf("\n\r%0.8f",AlgorithmData->forceModeled[i]);			
	}
	//printf("\n\rVpressure_smooth_min=%0.8f",Vpressure_smooth_min);
	
	printf("\r\n\r\n");
	#endif

	#if ENABLE_PRINTF_PRESSURE_CALCULATION	
		printf("\n\r Pressure");
	#endif
	//Pressure = forceModeled./(SA*1000);
	temp_float_data = (SA*1000);  //Taking out of loop
	for( i=0 ; i< Acq_Data_Count ; i++ )
	{
		AlgorithmData->Pressure[i]=AlgorithmData->forceModeled[i]/temp_float_data;
		#if ENABLE_PRINTF_PRESSURE_CALCULATION	
		printf("\n\r%0.8f",AlgorithmData->Pressure[i]);	
		#endif
	}
	
	#if ENABLE_PRINTF_PRESSURE_CALCULATION	
	printf("\n\r\r\n");	
	#endif
}

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Pressure_without_pranjal
 * Brief:       This function will calculate Pressure column from raw data column .
 * Input:       V pressure column 
 *				Acq_Data_Count  :Acquisition data count
 *          	PressureAverageSize
 * Output:    	forceModeled.

 -----------------------------------------------------------------------------*/
static void Calculate_Pressure_without_pranjal(void)
{
	uint32_t i=0; 
	float temp_float_data=0;//, temp_vpressure_smooth_minimum = 0xffff;	
	//float K_force5 = 0.17;
	//float 
	Kforce_3 = Kforce_5*3/5.0;
	temp_float_data = (1000* Kforce_3 *SA);  //Taking out of loop
	for( i=0 ; i< Acq_Data_Count ; i++ )
	{
		AlgorithmData->Pressure[i]=(AlgorithmData->V_pressureSmooth[i]-Vpressure_smooth_min)/temp_float_data;
		#if ENABLE_PRINTF_PRESSURE_CALCULATION	
		printf("\n\r%0.8f",AlgorithmData->Pressure[i]);	
		#endif
	}
	
	#if ENABLE_PRINTF_PRESSURE_CALCULATION	
	printf("\n\r\r\n");	
	#endif
}


/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Failed_Test_Investigation_Parameters
 * Brief:      Calculate CStestAvgSpeed ,SHtestAvgSpeed , CSSHspeedDiff etc for checking test filed conditions
 * Input:       finalDepth ,
 * Output:    	CStestAvgSpeed , SHtestAvgSpeed , CSSHspeedDiff
 -----------------------------------------------------------------------------*/
void Calculate_Failed_Test_Investigation_Parameters(void)
{
	uint16_t SamplingRate= (Settings_Data.Sampling_Rate*1000);
	float testCounts = EndIndex;
	float SHtestCounts=0;
	float SHtestHeight=0;
	float CStestDur=0;
	float SHtestDur=0;
	float testHeight=0 ;
	float testDur = 0;
	
 	testDur = testCounts/(float)SamplingRate;
	testHeight = AlgorithmData->finalDepth[EndIndex]-AlgorithmData->finalDepth[0];
	
	testAvgSpeed = testHeight/testDur; //mm/s
	//abs
	if(testAvgSpeed<0)
	{
		testAvgSpeed= 0-testAvgSpeed;
	}
 
	CStestDur = (countsCStotal)/(float)SamplingRate;
	CStestAvgSpeed = CSdepth/(float)CStestDur; //mm/s
	//abs
	if(CStestAvgSpeed<0)
	{
		CStestAvgSpeed= 0-CStestAvgSpeed;
	}

 
 	SHtestCounts = testCounts - countsCStotal; //SH means secondHalf
	SHtestDur = SHtestCounts/(float)SamplingRate;
	SHtestHeight = testHeight-CSdepth;
	SHtestAvgSpeed = (SHtestHeight/(float)SHtestDur); //%mm/s
	//abs
	if(SHtestAvgSpeed<0)
	{
		SHtestAvgSpeed = 0-SHtestAvgSpeed;
	}

	CSSHspeedDiff = (1-(SHtestAvgSpeed/(float)CStestAvgSpeed));
	//abs
	if(CSSHspeedDiff<0)
	{
		CSSHspeedDiff = 0-CSSHspeedDiff;
	}
	
	#if ENABLE_PRINTF_FAILED_TEST_INVEST_PARAMS
	printf(" EndIndex :%d\r\n",EndIndex);
 	printf(" testDur :%f\r\n",testDur);
	printf(" testHeight :%f\r\n",testHeight);
	printf("\r\n testAvgSpeed :%f\r\n",testAvgSpeed); 
	
	printf(" countsCStotal :%d\r\n",countsCStotal);
	printf(" CSdepth :%f\r\n",CSdepth);
	printf(" CStestDur :%f\r\n",CStestDur);
	printf(" CStestAvgSpeed :%f\r\n",CStestAvgSpeed);
	
	printf(" countsCStotal :%d\r\n",countsCStotal);
	printf(" CSdepth :%f\r\n",CSdepth);
	
	printf(" SHtestCounts :%f\r\n",SHtestCounts);
	printf(" SHtestDur :%f\r\n",SHtestDur);
	printf(" SHtestHeight :%f\r\n",SHtestHeight);
	printf(" SHtestAvgSpeed :%f\r\n",SHtestAvgSpeed); 
	printf(" CSSHspeedDiff :%f\r\n",CSSHspeedDiff); 
	
	#endif 

} 

/*-----------------------------------------------------------------------------* 
 * Function:    Process_Data
 * Brief:       This function will process raw data to processed data
 * Parameter:   name of raw data file,name of processed data file and name pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint8_t Process_Data(void)
{

	uint32_t i=0;
/* 	uint8_t temp_result=0;
	uint64_t total_Bytes_written = 0;
	uint32_t index = 0;
	float V_Irdms_Mod=0;
	float Vpressure_Difference=0,depth_rough_temp=0,Final_pressure=0, D_meas=0, D_forward=0;
 */

//	#ifdef PC_DEBUG_TEST
//	AlgorithmData =&intermediate_data;
	
//	Settings_Data.Sampling_Rate = 5;
//	Settings_Data.QRDbreakV=0.5;
//	Settings_Data.Test_Time=5;
//	Settings_Data.Calibration_Time=0.5;
//	Settings_Data.QRDpercentLow=0.08;
//	Settings_Data.QRDpercentHigh= 0.3;
////	#endif
//	if(Take_SD_Raw()!=0)
//		return 1;
#if ALGO_TEST
	Acq_Data_Count =12500;
#endif
//	printf("\n Settings_Data.Sampling_Rate = %f\n",Settings_Data.Sampling_Rate);
//	printf("\n Settings_Data.QRDbreakV = %f\n",Settings_Data.QRDbreakV);
//	printf("\n Settings_Data.Test_Time = %d\n",Settings_Data.Test_Time);
//	printf("\n Settings_Data.Calibration_Time = %f\n",Settings_Data.Calibration_Time);
//	printf("\n Settings_Data.QRDpercentLow = %f\n",Settings_Data.QRDpercentLow);
//	printf("\n Settings_Data.QRDpercentHigh = %f\n",Settings_Data.QRDpercentHigh);
//	
//	
//	printf("\nData count = %d\n",Acq_Data_Count);
	if(Acq_Data_Count == 0)
	{
		printf("\n Acq_Data_Count 0!!!");
		return 1;
	}

	Initialize_Global_Data();
	//Create a float column of V_pressure,V_IRDMS from raw data
	for(i=0;i<Acq_Data_Count;i++)
	{
		AlgorithmData->V_pressure[i] = AlgorithmData->force_buff[i]*RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		AlgorithmData->V_IRDMS_Raw[i] = AlgorithmData->distanceBuff[i]*RAW_DATA_TO_VOLTAGE_MULTIPLIER;
	}	
	//K_factor = (1000 * Kforce_3 * SA);


	
	
	#if ENABLE_PRINTF_RAW_VOLTAGE_DATA
		printf("\r\n\r\nAcq_Data_Count=%d",Acq_Data_Count);
		printf("\r\n\r\nV_Pressure_Device\tV_IRDMS_Device\tV_QRD_Device");
		for(i=0;i<Acq_Data_Count;i++)
		{
			printf("\r\n%f\t%f\t%f",AlgorithmData->V_pressure[i],AlgorithmData->V_IRDMS_Raw[i],AlgorithmData->qrdBuff[i]*RAW_DATA_TO_VOLTAGE_MULTIPLIER);
		}

		
	#endif
	
	Calculate_Sampling_Rate_Dependent_Parameters();
	#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Calculate_Sampling_Rate_Dependent_Parameters @ ");
	print_rtc_time_date();
	#endif
	Normalize_Initial_Pressure_Voltages();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS	
	printf("\n Normalize_Initial_Pressure_Voltages @ ");
	print_rtc_time_date();
#endif
	Smooth_Pressure_Sensor();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Smooth_Pressure_Sensor @ ");
	print_rtc_time_date();
#endif
	Calculate_Pressure();
//	Calculate_Pressure_without_pranjal();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Calculate_Pressure @ ");
	print_rtc_time_date();
#endif
	Smooth_Qrd();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Smooth_Qrd @ ");
	print_rtc_time_date();
#endif
	Find_the_trigger();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Find_the_trigger @ ");
	print_rtc_time_date();
#endif
	Smooth_IRDMS();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Smooth_IRDMS @ ");
	print_rtc_time_date();
#endif
	Find_15cm_output_marker();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Find_15cm_output_marker @ ");
	print_rtc_time_date();
#endif
	Calculate_IRDMS_Distance_and_Depth_from_Voltage();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Calculate_IRDMS_Distance_and_Depth_from_Voltage @ ");
	print_rtc_time_date();
#endif
	find_CS_Parameters();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n find_CS_Parameters @ ");
	print_rtc_time_date();
#endif
	Look_for_Trigger_from_Pressure_Sensor();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Look_for_Trigger_from_Pressure_Sensor @ ");
	print_rtc_time_date();
#endif
	RedefineDepthRough();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n RedefineDepthRough @ ");
	print_rtc_time_date();
#endif
	Lock_at_Trigger_ROS();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Lock_at_Trigger_ROS @ ");
	print_rtc_time_date();
#endif
	adjust_DepthRough_at_IRDMS_flipIndex();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n adjust_DepthRough_at_IRDMS_flipIndex @ ");
	print_rtc_time_date();
#endif
	Finding_End_Of_The_Test();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Finding_End_Of_The_Test @ ");
	print_rtc_time_date();
#endif
	Calculate_Failed_Test_Investigation_Parameters();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Calculate_Failed_Test_Investigation_Parameters @ ");
	print_rtc_time_date();
#endif	
	if ((SHtestAvgSpeed < speedMinLimit) || (CStestAvgSpeed < speedMinLimit) || (testAvgSpeed < speedMinLimit))
	{
		
		printf("\r\n Too Slow \r\n");
		processing_status=1; //Failed
		Writefiles_after_processing();
		return TOO_SLOW_ERROR_CONDITION;
	}

	if ((SHtestAvgSpeed > speedMaxLimit) || (CStestAvgSpeed > speedMaxLimit) || (testAvgSpeed > speedMaxLimit))
	{
		processing_status=1; //failed
		Writefiles_after_processing();
		printf("\r\n Too Fast \r\n");
		return TOO_FAST_ERROR_CONDITION;

	}
	
	/*********************************************************/ 
	
	full_resolution_creation_force();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n full_resolution_creation_force @ ");
	print_rtc_time_date();
#endif
	Discretize_Data_and_Scale_for_Graphing();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Discretize_Data_and_Scale_for_Graphing @ ");
	print_rtc_time_date();
#endif
	Compress_Pressure_Data();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Compress_Pressure_Data @ ");
	print_rtc_time_date();
#endif
	Expand_Pressure_Data();	
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Expand_Pressure_Data @ ");
	print_rtc_time_date();	
#endif
	CreatePixelatedMeans();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n CreatePixelatedMeans @ ");
	print_rtc_time_date();
#endif
	//Pixels_buffer will contain 5 type graph data 
	//(Pixelated Means,Pressure Blocky Pixels A,Pressure Blocky Pixels B,Pressure Blocky Pixels C,Pressure Blocky Pixels D)
	Create_Pressure_Blocky_Pixels(Pixels_buffer+(GRAPH_RAW_PIXEL_COUNT), xfractionA);	
	Create_Pressure_Blocky_Pixels(Pixels_buffer+(2*GRAPH_RAW_PIXEL_COUNT), xfractionB);	
	Create_Pressure_Blocky_Pixels(Pixels_buffer+(3*GRAPH_RAW_PIXEL_COUNT), xfractionC);
	Create_Pressure_Blocky_Pixels(Pixels_buffer+(4*GRAPH_RAW_PIXEL_COUNT), xfractionD);
	
	//Save endDepth in processed Data file
	Pixels_buffer[5*GRAPH_RAW_PIXEL_COUNT] = EndDepth;
			
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Create_Pressure_Blocky_Pixels @ ");
	print_rtc_time_date();
#endif
	Print_Pressure_Blocky_Pixels();
	processing_status=0;//success
	Writefiles_after_processing();
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
printf("\n Writefiles_after_processing @ ");
print_rtc_time_date();
#endif
	printf("Processing  Completed\r\n ");


	return 0;
}





