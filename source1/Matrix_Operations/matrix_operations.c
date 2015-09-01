/*
 * matrix_operations.c
 *
 *  Created on: Aug 25, 2015
 *      Author: Joe Trovato
 */
#include "matrix_operations.h"

/*
 * This function requires input to be a 2d array (matrix) of size [m x n] and that output is a 2d array of size [n x m]
 */
void transpose(struct matrix_struct *input, struct matrix_struct *output, int m, int n)
{
	printf("input matrix: \n");
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			printf("%f ", input->mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for(uint8_t i = 0; i < m; i++)
	{
		for(uint8_t j = 0; j < n; j++)
		{
			output->mat[j][i] = input->mat[i][j];
		}
	}
	printf("transposed matrix: \n");
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			printf("%f ", output->mat[i][j]);
		}
		printf("\n");
	}
}


void inverse(float * input, float * output, uint8_t m, uint8_t n)
{
	
}
//void m_mult(float a, float * b, float * output, uint8_t m1, uint8_t n1, uint8_t m2, uint8_t n2)
//{
//	if(n1 != m2)
//	{
//		printf("matrix dimensions do not agree. Exiting.");
//		return;
//	}
//	
//}


/*
 * Function:	Matrix Test
 * Description: Test file for our matrix operations
 */
void matrix_test(void)
{
	printf("matrix transpose test! \n");
	
	
	
	int m = 6;
	int n = 4;
	struct matrix_struct w2;
	w2.m = m;
	w2.n = n;
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			w2.mat[i][j] = i*n + j;
		}
	}

	struct matrix_struct w_t2;
	w_t2.m = n;
	w_t2.n = m;
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			w_t2.mat[i][j] = 0;
		}
	}
	transpose(&w2, &w_t2, m, n);
	printf("transposed matrix outside: \n");
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			printf("%f ", w_t2.mat[i][j]);
		}
		printf("\n");
	}

	
}
