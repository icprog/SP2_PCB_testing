/******************************************************************************
 *
 *   Filename: $matlab_functions.c
 *   Created on: $Date: Oct 10, 2014 
 *   Revision: $1
 *   Author: $Shihab
 *
 *   Description: $This file contains c implementation of some Matlab functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include "matlab_functions.h"
#include <string.h>
#include <stdio.h>

 /*-----------------------------------------------------------------------------* 
 * Function:    zeros_f
 * Brief:       Fill float array with all zeros
 * Input:   	Start address float array ,size of the column
 * Output:		Float array with all zeros
 * Return:      None 
 -----------------------------------------------------------------------------*/
 void zeros_f(float *data_buffer,const uint32_t size)
 {
	memset(( float *)data_buffer,0x00,(size*sizeof(float)));
 }
 

  /*-----------------------------------------------------------------------------* 
 * Function:    linspace
 * Brief:       Generate linearly spaced vector
 * Input:   	Start address float array ,Start element,End element, Number of elements RawLength
 * Output:		returns RawLength linearly spaced points.
 * Return:      None 
 -----------------------------------------------------------------------------*/
 void linspace(float *data_buffer,const float start_data,const float end_data,const uint32_t RawLength)
 {
	float diff_to_add=0;
	uint32_t i=0;
	if(RawLength<2)
	{
		printf("\r\n Invalid RawLength");
		return;
	}
	diff_to_add = (end_data-start_data)/(float)(RawLength-1);
	
	data_buffer[0]  = start_data;
	//printf("\r\n %f",data_buffer[0]);
	for(i=1;i<RawLength;i++)
	{
		data_buffer[i] = data_buffer[i-1]+ diff_to_add;
		//printf("\r\n %f",data_buffer[i]);
	}
 
 }