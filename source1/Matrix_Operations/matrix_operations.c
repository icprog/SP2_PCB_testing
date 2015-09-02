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
void transpose(struct matrix_struct *input, struct matrix_struct *output)
{
	int m = input->m;
	int n = input->n;
	output->m = n;
	output->n = m;
	
	for(uint8_t i = 0; i < m; i++)
	{
		for(uint8_t j = 0; j < n; j++)
		{
			output->mat[j][i] = input->mat[i][j];
		}
	}
}


void inverse(struct matrix_struct *input, struct matrix_struct *output)
{
		if(input->m != input->n)
		{
			printf("matrix must be square. Exiting.");
			return;
		}
}
void m_mult(struct matrix_struct *a, struct matrix_struct *b, struct matrix_struct *product)
{
	if(a->n != b->m)
	{
		printf("matrix dimensions do not agree. Exiting.");
		return;
	}
	product->m = a->m;
	product->n = a->n;
	float sum = 0.0;
	for(int i = 0; i < a->m; i++)
	{
		for(int j = 0; j < b->n; j++)
		{
			for(int k = 0; k<b->m ; k++)
			{
				sum += a->mat[i][k]*b->mat[k][j];
			}
			product->mat[i][j] = sum;
			sum = 0.0;
		}
	}
}


/*
 * Function:	Matrix Test
 * Description: Test file for our matrix operations
 */
void matrix_test(void)
{
	printf("matrix transpose test! \n");
	
	
	
	int m = 6;
	int n = 4;
	struct matrix_struct w;
	w.m = m;
	w.n = n;
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			w.mat[i][j] = i*n + j;
		}
	}
	printf("input matrix: \n");
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			printf("%f ", w.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	struct matrix_struct w_t;
	transpose(&w, &w_t);
	printf("transposed matrix: \n");
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			printf("%f ", w_t.mat[i][j]);
		}
		printf("\n");
	}
	
	printf("\n\n multiplication test");
	struct matrix_struct wwt;
	m_mult(&w, &w_t, &wwt);
	
	printf("those two multiplied: \n");
	for(int i = 0; i < wwt.n; i++)
	{
		for(int j = 0; j < wwt.m; j++)
		{
			printf("%f ", wwt.mat[i][j]);
		}
		printf("\n");
	}
	

	
}
