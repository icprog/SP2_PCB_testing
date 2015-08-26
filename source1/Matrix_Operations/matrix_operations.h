/******************************************************************************
 *
 *   Filename: matrix_operations.h
 *   Created on: $Date: August 25 2015 
 *   Revision: $1
 *   Author: $Joe Trovato
 *
 *   Description: $This file contains c implementation of some basic matrix operations
 *
 *****************************************************************************/
#ifndef __MATRIX_OPERATIONS_H__
#define __MATRIX_OPERATIONS_H__

#include <stdint.h>

void transpose(float * input, float * output, uint8_t m, uint8_t n);
void inverse(float * input, float * output, uint8_t m, uint8_t n);
void m_mult(float * a, float * b, uint8_t m1, uint8_t n1, uint8_t m2, uint8_t n2);
 
#endif //__MATRIX_OPERATIONS_H__
