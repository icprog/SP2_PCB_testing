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
#include "common_headers.h"

struct matrix_struct
{
	int m, n;
	float mat[10][10]; //maximum size of matrix allowed
};

void transpose(struct matrix_struct *input, struct matrix_struct *output, int m, int n);
void inverse(float * input, float * output, uint8_t m, uint8_t n);
//void m_mult(float * a, float * b, float * output, uint8_t m1, uint8_t n1, uint8_t m2, uint8_t n2);

void matrix_test(void);
 
#endif //__MATRIX_OPERATIONS_H__
