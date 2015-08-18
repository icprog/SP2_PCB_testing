#ifndef _ADJUST_H_
#define _ADJUST_H_

#include <stdarg.h>
#include <stddef.h>

typedef float real;

struct BiquadFilter_struct {
  real b0, b1, b2, a1, a2;
};

typedef struct BiquadFilter_struct BiquadFilter;

/**
 * void convolve(real* f1, size_t n1, real* f2, size_t n2, real* result);
 * 
 * Computes the convolution of f1 and f2, with sizes n1 and n2, and
 * puts the result in result, which must be have size at least max(n1,
 * n2).
 *
 *  
 * It is NOT valid for result and f1 or result and f2 to overlap!
 *
 * Values near the boundaries may be inaccurate because only part of
 * one of the functions gets considered.
 *
 **********************************************************************
 **/
void convolve(const real* f1, const size_t n1, const real* f2, const int* order, const size_t n2, real* result);

/**
 * void adjust(real* voltage, size_t npts, real* output);
 *
 * Converts voltage to force, applying non-linearity compensation.
 * 
 * output must be at least as long as npts, but may overlap voltage
 *
 **/
void adjust(const real* voltage, const size_t npts, real* output);

/**
 * void filtfilt(const real* input, const size_t npts, const real*
 * filtera, const real* filterb, const size_t filtersz, real* output);
 * 
 * Behaves like MATLAB filtfilt; takes the transfer-function
 * representation of an IIR filter and returns the filtered values in
 * output.
 * 
 **/
void filtfilt(const real* input, const size_t npts, const BiquadFilter* filters, const size_t filtersz, real* output) ;
#endif /* _ADJUST_H_ */
