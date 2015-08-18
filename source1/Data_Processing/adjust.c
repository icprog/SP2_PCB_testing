#include "common_headers.h"
#include "model.h"
#include "adjust.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
//#include <stdio.h>
#include "calib.h"

#include "algorithm.h"

void convolve_(const real* f1, const size_t n1, const real* f2, const int* order, const int n2, real* result) {
  for(int i = 0; i < n2; i++) {
    result[i] = 0;
    result[n1 - i] = 0;
  }
  for (int i = n2; i < n1 - n2; i++) {
    result[i] = 0.0;
    //    printf("%d = \n", i);
    for (size_t j = 0 ; j < n2; j++) { 
      result[i] += f1[i + order[j] - n2/2 - 1] * f2[n2 - order[j]];
      //      printf("+ %lf * %lf ", f1[i + j - n2/2], f2[n2 - j - 1]);
    }
    //    printf("\n\n%lf\n\n\n", result[i]);
  }
}
    
void convolve(const real* f1, const size_t n1, const real* f2, const int* order, const size_t n2, real* result) {
  if (n2 > n1) {
    printf("ERROR\n");
    //    convolve_(f2, n2, f1, order, n1, result);
  } else {
    convolve_(f1, n1, f2, order, n2, result);
  }
}


void filter(const real* input, const size_t npts, const real* filtera, const real* filterb, const size_t filtersz, real* output) {
  for (int i = 0; i < npts; i++) {
    output[i] = 0;
    for (int j = 0; (j < filtersz) && (j <= i); j++) {
      output[i] += filterb[j] * input[i-j];
      if (j > 0) {
	output[i] -= filtera[j] * output[i - j];
      }
    }
    output[i] /= filtera[0];
    //    printf("%f -> %f\n", input[i], output[i]);
  }
}

void applyBiquadFilter(const real* input, const size_t npts, const BiquadFilter* filter, real* output) {
  double b0 = filter->b0;
  double b1 = filter->b1;
  double b2 = filter->b2;
  double a1 = filter->a1;
  double a2 = filter->a2;
  for(int i = 2; i < npts; i++) {
    output[i] =  b0 * input[i] + b1 * input[i-1] + b2 * input[i - 2] - a1 * output[i-1] - a2 * output[i-2];
  }
}

void sosFilter(const real* input, const size_t npts, const BiquadFilter* filters, const size_t nfilters, real* output) {
  //real* buf = (real*)malloc(npts * sizeof(real));  
	
  real* buf = AlgorithmData->sosFilter; //Removed dynamic memory allocation
  memcpy(buf, input, npts * sizeof(real));
  for (int i = 0; i < nfilters; i++) {
    applyBiquadFilter(buf, npts, &(filters[i]), output);
    memcpy(buf, output, npts * sizeof(real));
  }
}

static void reverse(real* input, const size_t npts) {
  for (int i = 0; i < npts/2; i++) {
    real tmp = input[npts - 1 - i];
    input[npts - 1 - i] = input[i];
    input[i] = tmp;
  }
}

void filtfilt(const real* input, const size_t npts, const BiquadFilter* filters, const size_t filtersz, real* output)  {
 // real* buf = (real*)malloc(npts * sizeof(real));
 // assert(buf);
  real* buf = AlgorithmData->filtfilt_double_buf; //Removed dynamic memory allocation
  
  sosFilter(input, npts, filters, filtersz, buf);
  reverse(buf, npts);
  sosFilter(buf, npts, filters, filtersz, output);
  reverse(output, npts);
//  free(buf);
}

void adjust(const real* voltage, const size_t npts, real* output) {

/*   real* buffer = (real*)malloc(npts * sizeof(real));
  real* inc = (real*)malloc(npts * sizeof(real));
  real* dec = (real*)malloc(npts * sizeof(real));
  assert(buffer); */
  real* buffer = AlgorithmData->adjust_buffer;  //Removed dynamic memory allocation
  real* inc = AlgorithmData->adjust_inc;
  real* dec = AlgorithmData->adjust_dec;
  
  // low-pass filter the voltage
  filtfilt(voltage, npts, filters, FILTER_SZ, buffer);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n filtfilt @ ");
	print_rtc_time_date();
#endif

  real zero = 0.0;
  int nave = Settings_Data.Sampling_Rate * Settings_Data.Calibration_Time *1000;;
  for (int i = 0; i < nave; i++) {
    zero += voltage[i];
  }
  zero /= nave;
  for (int i = 0; i < npts; i++) {
    buffer[i] -= zero;
  }
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n buffer init @ ");
	print_rtc_time_date();
#endif
  // calculate the force based on params_inc
  convolve(buffer, npts, params_inc, params_inc_ind, PARAM_SZ, inc);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n convolve inc @ ");
	print_rtc_time_date();
#endif
  // calculate the force based on params_dec
  convolve(buffer, npts, params_dec, params_dec_ind, PARAM_SZ, dec);
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n convolve dec @ ");
	print_rtc_time_date();
#endif
	for (int i = 1; i < npts; i++) 
	{
		// choose the right force based on whether derivative is positive
		// or negative
		//    real wi = (inc[i] - inc[i-1]) * 20 + (dec[i] - dec[i-1]) * 20;;
		//    real wd = (inc[i - 1] - inc[i]) * 20 + (dec[i - 1] - dec[i]) * 20;
		double wii = inc[i] - inc[i-1];
		double wid = -wii;
		double wdi = dec[i] - dec[i-1];
		double wdd = -wdi;
	
		wii = wii > 0;
		wid = wid > 0;
		wdi = wdi > 0;
		wdd = wdd > 0;
	
	
		double wi = (wii + wdi)/2;
		double wd = (wid + wdd)/2;    
	
		output[i] = wi * inc[i] + wd * dec[i];  
		output[i] = 3 * pow(10, output[i]) - pow(10, 0.45);
		if(output[i] > 20) {
			output[i] = 20;
		}
		if (output[i] < -20) {
			output[i] = -20;
		}
	
	}
#if ENABLE_PRINTF_FUNCTION_EXECUTION_TIME_ANALYSIS
	printf("\n Last for loop @ ");
	print_rtc_time_date();
#endif
/*   free(buffer);
  free(inc);
  free(dec); */
}

