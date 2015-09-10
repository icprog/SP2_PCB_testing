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

/*
 * find the inverse of a  amtrix using the guass jordan method
 */
void inverse(struct matrix_struct *input, struct matrix_struct *output)
{
	if(input->m != input->n)
	{
		printf("matrix must be square. Exiting. \n");
		return;
	}
	int m = input->m;
	output->m = m;
	output->n = m;
	
	//generate the identity and a copy of the input (as it will be changed via row operations)
	struct matrix_struct A;
	struct matrix_struct I;
	I.m = m; A.m = m;
	I.n = m; A.n = m;
	for(int i = 0; i < I.m; i++)
	{
		for(int j = 0; j < I.n; j++)
		{
			A.mat[i][j] = input->mat[i][j];
			if(i ==j)
			{
				I.mat[i][j] = 1;
			}
			else
			{
				I.mat[i][j] = 0;
			}
		}
	}
	
	//Gauss Jordan
	double temp;
	int i, j, k;
	for(i=0; i<m; i++)
	{
		temp = A.mat[i][i];
		for(j=0; j<m; j++)
		{
			A.mat[i][j]/=temp;
			I.mat[i][j]/=temp;
		}
		for(k=0; k < m; k++)
		{
			temp = A.mat[k][i];
			for(j=0;j<m;j++)
			{
				if(k==i)
				{
					break;
				}
				A.mat[k][j]-=A.mat[i][j]*temp;
				I.mat[k][j]-=I.mat[i][j]*temp;
			}
		}
	}
	//populate output matrix with the inverese, which was transfromed from the identity
	for(int i = 0; i < I.m; i++)
	{
		for(int j = 0; j < I.n; j++)
		{
			output->mat[i][j] = I.mat[i][j];
		}
	}
	
		
}
void m_mult(struct matrix_struct *a, struct matrix_struct *b, struct matrix_struct *product)
{
	if(a->n != b->m)
	{
		printf("matrix dimensions do not agree. Exiting.\n");
		return;
	}
	product->m = a->m;
	product->n = b->n;
	double sum = 0.0;
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
 * find the determinant of a square matrix
 */
float determinant(struct matrix_struct *input, int m)
{
	if(input->m != input->n)
	{
		printf("matrix must be square. Exiting. \n");
		return;
	}
	float det; //the value of the determinant
	
	if(m == 2)
	{
		//TODO 
		return det;
	}
	struct matrix_struct temp_mat;
	temp_mat.m = m;
	temp_mat.n = m;
	float temp_arr[m];
	
	for(int i = 1; i<=m; i++)
	{
		int r=0;
		int s=0;
		for(int j = 0; j < m; j++)
		{
			for(int k = 0; k<m; k++)
			{
				if(j!=0 && k!=i)
				{
					temp_mat.mat[r][s] = input->mat[j][k];
					s++;
					if(s>m-1)
					{
						r++;
						s=0;
					}
				}
			}
		}
		for(int l=1, pr=1; l<=(i+1); l++)
		{
			pr=(-1)*pr;
			temp_arr[i]=pr*determinant(&temp_mat, m-1);
		}
	}
	det = 0.0;
	for(int i=1; i<=m; i++)
	{
		det += input->mat[1][i]*temp_arr[i];
	}
	return det;
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
	
	printf("\n\n multiplication test \n");
	struct matrix_struct wwt;
	m_mult(&w_t, &w, &wwt);
	
	printf("those two multiplied: \n");
	for(int i = 0; i < wwt.m; i++)
	{
		for(int j = 0; j < wwt.n; j++)
		{
			printf("%f ", wwt.mat[i][j]);
		}
		printf("\n");
	}
	
	struct matrix_struct A;
	A.m = 3;
	A.n = 3;
	A.mat[0][0] = 1;
	A.mat[0][1] = 2;
	A.mat[0][2] = 3;
	A.mat[1][0] = 0;
	A.mat[1][1] = 4;
	A.mat[1][2] = 5;
	A.mat[2][0] = 1;
	A.mat[2][1] = 0;
	A.mat[2][2] = 6;
	
	printf("\n\n multiplication test 2 \n");
	struct matrix_struct A2;
	m_mult(&A, &A, &A2);
	printf("A: \n");
	for(int i = 0; i < A.m; i++)
	{
		for(int j = 0; j < A.n; j++)
		{
			printf("%f ", A.mat[i][j]);
		}
		printf("\n");
	}
	
	printf("A^2: \n");
	for(int i = 0; i < A2.m; i++)
	{
		for(int j = 0; j < A2.n; j++)
		{
			printf("%f ", A2.mat[i][j]);
		}
		printf("\n");
	}
	
	
	
	
	printf("inverse test! \n\n");

	
//	for(int i = 0; i < A.m; i++)
//	{
//		for(int j = 0; j < A.n; j++)
//		{
//			A.mat[i][j] = i+j;
////			if(i ==j)
////			{
////				A.mat[i][j] = 1;
////			}
////			else
////			{
////				A.mat[i][j] = 0;
////			}
//		}
//	}
	struct matrix_struct invA;
	inverse(&A, &invA);
	printf("input matrix: \n");
	for(int i = 0; i < A.m; i++)
	{
		for(int j = 0; j < A.n; j++)
		{
			printf("%f ", A.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	printf("inverse matrix: \n");
	for(int i = 0; i < invA.m; i++)
	{
		for(int j = 0; j < invA.n; j++)
		{
			printf("%f ", invA.mat[i][j]);
		}
		printf("\n");
	}
	
}
