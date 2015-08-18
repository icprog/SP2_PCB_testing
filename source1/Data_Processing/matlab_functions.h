/******************************************************************************
 *
 *   Filename: $matlab_functions.h
 *   Created on: $Date: Oct 10, 2014 
 *   Revision: $1
 *   Author: $Shihab
 *
 *   Description: $This file contains c implementation of some Matlab functions
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#ifndef __MATLAB_FUNCTIONS_H__

#define __MATLAB_FUNCTIONS_H__


#include <stdint.h>

void zeros_f(float *data_buffer,const uint32_t size);
 
void linspace(float *data_buffer,const float start_data,const float end_data,const uint32_t RawLength);
 
#endif //__MATLAB_FUNCTIONS_H__