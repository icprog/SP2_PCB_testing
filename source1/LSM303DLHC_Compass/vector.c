/******************************************************************************
 *
 *     Filename:       $vector.c
 *     Created on:     $Date: Mar 24, 2014    (5:22:55 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file includes function releted to vectors
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "vector.h"
#include <math.h>

/*Function prototype*/
static void vector_cross(const vector *a, const vector *b, vector *out);
static float vector_dot(const vector *a, const vector *b);

/*-----------------------------------------------------------------------------
 *  Function:     vector_cross
 *  Brief:        Calculate the vector cross product to 2 vectors
 *  Parameter:    Address of input vectors and address of output vector
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void vector_cross(const vector *a, const vector *b, vector *out)
{
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

/*-----------------------------------------------------------------------------
 *  Function:     vector_dot
 *  Brief:        Calculate the dot  product to 2 vectors
 *  Parameter:    Address of input vectors 
 *  Return:       dot product
 -----------------------------------------------------------------------------*/
static float vector_dot(const vector *a, const vector *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

/*-----------------------------------------------------------------------------
 *  Function:     vector_normalize
 *  Brief:        Normalise a given vector
 *  Parameter:    Address of input vectors to normalize
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Vector_normalize(vector *a)
{
    float mag = sqrt(vector_dot(a, a));
    a->x /= mag;
    a->y /= mag;
    a->z /= mag;
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
