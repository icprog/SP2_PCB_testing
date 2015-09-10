/*
 * calib.h
 *
 *  Created on: Dec 8, 2014
 *      Author: Rahul
 */

#ifndef CALIB_H_
#define CALIB_H_

#include "matrix_operations.h"

#define NUM_OF_IRDMS_CONDITION		(2)
#define NUM_OF_PRESSURE_CONDITION	(5)
#define NUM_OF_ROS_CONDITION	(2)
#define NUM_OF_ACC_CONDITION	(4)
#define NUM_OF_MAG_CONDITION	(4)

#define DEG_TO_RAD           (0.0174532925)
#define RAD_TO_DEG           (57.2957795)

extern float cal_pitch;
extern float rad_shift;

void calculate_acc_transform(struct matrix_struct * w, struct matrix_struct * Y, struct matrix_struct *X );
void acc_transform_wrapper(void);
void acc_transform_wrapper2(int pos_num);
void transform_raw_acc(void);
void transform_raw_acc_manual(void);

#endif /* CALIB_H_ */
