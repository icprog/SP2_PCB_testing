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

void transpose(struct matrix_struct *input, struct matrix_struct *output);
void inverse(struct matrix_struct *input, struct matrix_struct *output);
void m_mult(struct matrix_struct *a, struct matrix_struct *b, struct matrix_struct *product);
void matrix_test(void);
 
#endif //__MATRIX_OPERATIONS_H__
