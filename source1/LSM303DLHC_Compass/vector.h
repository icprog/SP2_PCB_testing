#ifndef __vector_h__
#define __vector_h__

/*Global variable*/
typedef struct vector
{
    float x, y, z;
} vector;

/*function prototype*/
extern void Vector_normalize(vector *a);

#endif
