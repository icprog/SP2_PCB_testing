/*
 * data_acquisition.h
 *
 *  Created on: Jan 14, 2014
 *      Author: GADGEON PC
 */

#ifndef DATA_ACQUISITION_H_

#define DATA_ACQUISITION_H_

#define DATA_ACQUISTITION_TASK	5

extern volatile uchar Test_Complete_flag, Acq_Complete_flag;
extern uint_8 start_data_collection(void);
void stop_data_acquision(void);
extern void print_collected_data_buffer(void);



#define BEFORE_TEST         0
#define AFTER_TEST          1

#endif /* DATA_ACQUISITION_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
