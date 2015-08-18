#include "calib.h"

//real CALIB_PTS[NCAL] = {0.0, 1.0, 2.0, 3.0};
//real DEVICE_PTS[NCAL] = {0.0, 1.0, 2.0, 3.0};
//
//
//real IRDMS_CALIB_PTS[NCAL_IRDMS] = {0.0, 1.0, 2.0, 3.0, 4.0};
//real IRDMS_DEVICE_PTS[NCAL_IRDMS] = {0.0, 1.0, 2.0, 3.0, 4.0};

double CALIB_PTS[NCAL] = {0.0, 1.0};
double DEVICE_PTS[NCAL] = {0.0, 1.0};
real IRDMS_CALIB_PTS[NCAL_IRDMS] = {0.0, 1.0, 2.0, 3.0, 4.0};
real IRDMS_DEVICE_PTS[NCAL_IRDMS] = {0.0, 1.0, 2.0, 3.0, 4.0};

real compensate_static_pt(real x, real* calib_pts, real* device_pts, int ncal) {
  int i = 0;

  while ( (x > device_pts[i]) && (i+1 < ncal - 1)) { 
    i ++;
  }
  real m = (calib_pts[i+1] - calib_pts[i])/(device_pts[i+1] - device_pts[i]);
  real x0 = device_pts[i];
  real y0 = calib_pts[i];
  real y = (x - x0) * m + y0;
  return y;
}

void compensate_static(const real* input, const size_t npts, real* output, real* calib_pts, real* device_pts, int ncal) {
  for(int i = 0; i < npts; i++) {
    output[i] = compensate_static_pt(input[i], calib_pts, device_pts, ncal);    
  }
}
