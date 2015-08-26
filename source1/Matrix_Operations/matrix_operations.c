/*
 * matrix_operations.c
 *
 *  Created on: Aug 25, 2015
 *      Author: Joe Trovato
 */
#include "matrix_operations.h"

void transpose(float * input, float * output, uint8_t m, uint8_t n)
{
	
}
void inverse(float * input, float * output, uint8_t m, uint8_t n)
{
	
}
void m_mult(float * a, float * b, uint8_t m1, uint8_t n1, uint8_t m2, uint8_t n2)
{
	if(n1 != m2)
	{
		printf("matrix dimensions do not agree. Exiting.");
		return;
	}
	
}

