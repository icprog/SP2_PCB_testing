#ifndef _MODEL_H_
#define _MODEL_H_
#include "adjust.h"
#define PARAM_SZ 200
#define FILTER_SZ 4
extern const real params_inc[PARAM_SZ];
extern const real params_dec[PARAM_SZ];
extern const int params_inc_ind[PARAM_SZ];
extern const int params_dec_ind[PARAM_SZ];
extern const BiquadFilter filters[FILTER_SZ];
#endif /* _MODEL_H_ */
