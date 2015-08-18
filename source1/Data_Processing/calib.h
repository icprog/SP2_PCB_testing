#ifndef _CALIB_H_
#define _CALIB_H_
#include "adjust.h"

//#define NCAL 4
//#define NCAL_IRDMS 5
//extern real CALIB_PTS[NCAL];
//extern real DEVICE_PTS[NCAL];
//extern real IRDMS_CALIB_PTS[NCAL_IRDMS] ;
//extern real IRDMS_DEVICE_PTS[NCAL_IRDMS] ;
//
//void compensate_static(const real* input, const size_t npts, real* output, real* calib_pts, real* device_pts, int ncal);
#define NCAL 4
#define NCAL_IRDMS 5

extern double CALIB_PTS[NCAL];
extern double DEVICE_PTS[NCAL];

extern real IRDMS_CALIB_PTS[NCAL_IRDMS] ;
extern real IRDMS_DEVICE_PTS[NCAL_IRDMS] ;

void compensate_static(const real* input, const size_t npts, real* output, real* calib_pts, real* device_pts, int ncal);

#endif /* _CALIB_H_ */
