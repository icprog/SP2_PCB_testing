/*
 * Power_mode.h
 *
 *  Created on: Feb 10, 2014
 *      Author: Gadgeon
 */

#ifndef POWER_MODE_H_
#define POWER_MODE_H_

/*Global variables*/
extern volatile char run_mode_flag, low_power_mode_flag;

/*function prototype*/
extern void VLPR_mode(void);
extern void run_mode(void);
extern void run_mode_for_BLE(void);
extern void low_power_mode_on(void);
extern void low_power_mode_off(void);
void OSC_EN_init(void);

#endif /* POWER_MODE_H_ */
