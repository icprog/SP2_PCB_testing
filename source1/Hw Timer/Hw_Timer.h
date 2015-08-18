/*
 * Hw_Timer.h
 *
 *  Created on: Dec 18, 2013
 *      Author: user
 */

#ifndef HW_TIMER_H_
#define HW_TIMER_H_
#endif /* HW_TIMER_H_ */

#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include <fio.h>

#include <hwtimer.h>

#include "config.h"
#include "adc_k70.h"


/*hwtimer example defines*/
extern uint_32  Adc_Sampling_Freq;      //frequency set to hwtimer 1
extern uint_32  HWTIMER2_FREQUENCY;    //frequency set to hwtimer 1
extern uint_32  TEST_COUNT;		

void Init_Hw_Timer(void);
void De_Init_Hw_Timer(void);
void Start_Hw_Timer_1(void);
void Stop_Hw_Timer_1(void);


void Cancel_Hw_Timer(void);
void Block_Hw_Timer(void);
void UnBlock_Hw_Timer(void);
void set_adc_sample_Rate(uint_16);

extern unsigned int dummy_ctr;
extern char Compass_Read,BUSY,Read_LSM;
extern volatile unsigned char Buffer_Overflow_Flag,Test_Err;
extern HWTIMER hwtimer1; 

