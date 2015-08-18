/******************************************************************************
 *
 *   Filename: $algorithm.h
 *   Created on: $Date: Sept 17, 2014 
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains header data for data processing algorithms
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#ifndef __ALGORITHM_H__

#define __ALGORITHM_H__

#define BUFFER_BLOCK_SIZE	       (600)
#define DEPTH_RAW_COUNT            (1500)
#define GRAPH_RAW_PIXEL_COUNT      (300)
#define GRAPH_COLUMN_PIXEL_COUNT   (200)
#define RAW_AVERAGE_COUNT          (DEPTH_RAW_COUNT/GRAPH_RAW_PIXEL_COUNT)
#define RAW_DATA_TO_VOLTAGE_MULTIPLIER (0.00005035)
#define SA_VALUE		(0.00002324275)
#define V_IRDMS_OFFSET_V 			(0.393648)



#define AIRDMS						(3740000000)	
#define BIRDMS 						(-1022.0)
#define CIRDMS						(-2.965)
#define DIRDMS						(0.08696)
/* 

#define A_P4 						(194.1)
#define B_P4 						(0.1304)
#define C_P4 						(-0.002314)
#define D_P4 						(197.7) */

#define V_PRESSURE_0 	(0.788303)
#define V_PRESSURE_MIN 	(0.003)
#define K_PRESSURE		(0.117)

#define TOO_SLOW_ERROR_CONDITION 2
#define TOO_FAST_ERROR_CONDITION 3
#define NO_ERROR_STATE			0

#define MAX_PROCESSING_DATA_COUNT (150000)//(10*15*1000)
#define ENABLE_PRINTF_SAMPLING_RATE_DEPENDENT_PARAMETERS 0
#define ENABLE_PRINTF_RAW_VOLTAGE_DATA 0
#define ENABLE_PRINTF_NORMALIZE_INITIAL_PRESSURE_VOLTAGES 0
#define ENABLE_PRINTF_SMOOTHING_PRESSURE 0
#define ENABLE_PRINTF_PRANJALS_ALGORITHM 0
#define ENABLE_PRINTF_SMOOTHING_PRESSURE_RESULT 0
#define ENABLE_PRINTF_PRESSURE_CALCULATION 0
#define ENABLE_PRINTF_SMOOTH_QRD 0
#define ENABLE_PRINTF_FIND_THE_TRIGGER 0
#define ENABLE_PRINTF_IRDMS_COMP 0
#define ENABLE_PRINTF_IRDMS_SMOOTH1 0
#define ENABLE_PRINTF_IRDMS_SMOOTH2 0
#define ENABLE_PRINTF_IRDMS_SMOOTH 0
#define ENABLE_PRINTF_VIRDMS_MOD 0
#define ENABLE_PRINTF_DEPTH_ROUGH_FULL 0
#define ENABLE_PRINTF_DEPTH_ROUGH 0
#define ENABLE_PRINTF_FIND_CS_PARAMETERS 0
#define ENABLE_PRINTF_PRESSURECHANGE 0
#define ENABLE_PRINTF_DEPTHROUGHFULLOFFSET 0
#define ENABLE_PRINTF_DEPTHROUGHSHIFT 0
#define ENABLE_PRINTF_DEPTHROUGH_B 0
#define ENABLE_PRINTF_DEPTHROUGH_C 0
#define ENABLE_PRINTF_DEPTHROUGH_D 0
#define ENABLE_PRINTF_DEPTHROUGH_E 0
#define ENABLE_PRINTF_DEPTHROUGH_F 0
#define ENABLE_PRINTF_DEPTHROUGH_G 0
#define ENABLE_PRINTF_IRDMS_MOVING_MIN 0
#define ENABLE_PRINTF_IRDMS_RUNNING_AVG 0
#define PRESSURE_CALCULATION_LOG_ENABLE 0
#define ENABLE_PRINTF_DEPTH_CALC 0
#define ENABLE_PRINTF_FINAL_DEPTH_PRESSURENEW 0
#define ENABLE_PRINTF_PRESSURE_MEANS 0
#define ENABLE_PRINTF_PRESSUREMEANSLOG4 0
#define ENABLE_PRINTF_PRESSUREMEANSLOGO 0
#define ENABLE_PRINTF_PRESSUREMEANSLOGR 0
#define ENABLE_PRINTF_COMPRESSED_PRESSURE 0
#define ENABLE_PRINTF_EXPANDED_PRESSURE 0
#define ENABLE_PRINTF_PRESSURE_PIXELS 0
#define ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_1 0
#define ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_2 0
#define ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_3 0
#define ENABLE_PRINTF_PRESSURE_BLOCKY_PIXELS_4 0

#define PRESSURE_DEPTH_CALCULATION_LOG_ENABLE 0
#define ENABLE_PRINTF_LOG 0
#define ENABLE_PRINTF_CALIBEREATE_PRESSURE_SENSOR 0
#define DISTANCE_CALCULATION_LOG 0
#define VIRDMS_MODE_PRITNF_FLAG 0
#define ENABLE_PRINTF_DEPTHRANGE 0


#define ENABLE_PRINTF_FINDING_END_OF_THE_TEST 0
#define ENABLE_PRINTF_FAILED_TEST_INVEST_PARAMS 0
#define ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS 0

#define CHECKING_ALGORTIHM_ENABLED 0
#define QRD_MEDIAN_TEMP_ARRAY_SIZE (128)
//Structure for holding intermediate data during algorithm processing
typedef struct 
{
	
	uint8_t compressedPressure[DEPTH_RAW_COUNT];
	uint16_t force_buff[MAX_PROCESSING_DATA_COUNT]; 
	uint16_t distanceBuff[MAX_PROCESSING_DATA_COUNT];
	uint16_t qrdBuff[MAX_PROCESSING_DATA_COUNT];
	uint16_t pressureMeans_count[DEPTH_RAW_COUNT];
	uint16_t smoothQRD[MAX_PROCESSING_DATA_COUNT];
/* 	uint32_t tempBuffer1_uint32_t[MAX_PROCESSING_DATA_COUNT];
	uint32_t tempBuffer2_uint32_t[MAX_PROCESSING_DATA_COUNT]; */
	
	float tempBuffer1_float[MAX_PROCESSING_DATA_COUNT];
	float tempBuffer2_float[MAX_PROCESSING_DATA_COUNT];
	float *VIRDMS_Mod;
	float depthRoughFull[MAX_PROCESSING_DATA_COUNT];
	float DepthRoughFullOffset[MAX_PROCESSING_DATA_COUNT];
	float depthRough[MAX_PROCESSING_DATA_COUNT];
	float DepthRoughShift[MAX_PROCESSING_DATA_COUNT];
	float DepthRoughFullShifted[MAX_PROCESSING_DATA_COUNT];
	float DepthRoughPLock[MAX_PROCESSING_DATA_COUNT];
	float DepthRoughUnaltered[MAX_PROCESSING_DATA_COUNT];
	
	float PressureRaw[MAX_PROCESSING_DATA_COUNT];
	float V_pressureSmooth[MAX_PROCESSING_DATA_COUNT];
	float PressureChange[MAX_PROCESSING_DATA_COUNT];
	
	
	float forceModeled[MAX_PROCESSING_DATA_COUNT];
	float Pressure[MAX_PROCESSING_DATA_COUNT];
	
 	float pressureBlockyPixels[GRAPH_RAW_PIXEL_COUNT];
/*	float pressurePixels[GRAPH_RAW_PIXEL_COUNT]; */
	float 	vIrdmsComp[MAX_PROCESSING_DATA_COUNT];
	float 	irdmsSmooth[MAX_PROCESSING_DATA_COUNT];
	//float 	processedData[2*MAX_PROCESSING_DATA_COUNT];
	
	//float force[MAX_PROCESSING_DATA_COUNT];
	//float finalDepth[MAX_PROCESSING_DATA_COUNT];
	float *finalDepth;
	float pressureNew[MAX_PROCESSING_DATA_COUNT];
	float depthRange[MAX_PROCESSING_DATA_COUNT];
	
	float pranjals_model_result[MAX_PROCESSING_DATA_COUNT];
	float V_pressure[MAX_PROCESSING_DATA_COUNT];
	float V_IRDMS[MAX_PROCESSING_DATA_COUNT];
	float V_IRDMS_Raw[MAX_PROCESSING_DATA_COUNT];
	float  pressureMeans[DEPTH_RAW_COUNT];
/* 	float PressureMeansLog4[DEPTH_RAW_COUNT];
	float PressureMeansLogo[DEPTH_RAW_COUNT];
	float PressureMeansLogr[DEPTH_RAW_COUNT]; */
	float expandedPressure[DEPTH_RAW_COUNT];
	float expandedPressureLog[DEPTH_RAW_COUNT];
	
	float PressurePixels1[GRAPH_RAW_PIXEL_COUNT];
	
	volatile int16_t AX[MAX_PROCESSING_DATA_COUNT];
	volatile int16_t AY[MAX_PROCESSING_DATA_COUNT];
	volatile int16_t AZ[MAX_PROCESSING_DATA_COUNT];
	volatile int16_t MX[MAX_PROCESSING_DATA_COUNT];
	volatile int16_t MY[MAX_PROCESSING_DATA_COUNT];
	volatile int16_t MZ[MAX_PROCESSING_DATA_COUNT];
	
	float filtfilt_double_buf[MAX_PROCESSING_DATA_COUNT];
	float adjust_buffer[MAX_PROCESSING_DATA_COUNT];
	float adjust_inc[MAX_PROCESSING_DATA_COUNT];
	float adjust_dec[MAX_PROCESSING_DATA_COUNT];
	float sosFilter[MAX_PROCESSING_DATA_COUNT];
	
}Algorithm_Data;

extern Algorithm_Data *AlgorithmData;
extern volatile uint32_t acqDataCount;
extern uint16_t EndDepth;
uint8_t Process_Data(void);

#endif /* __ALGORITHM140904_H__ */

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
