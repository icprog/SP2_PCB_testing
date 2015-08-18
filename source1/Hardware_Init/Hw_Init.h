/*
 * Hw_Init.h
 *
 *  Created on: Jan 15, 2014
 *      Author: user
 */

#ifndef HW_INIT_H_
#define HW_INIT_H_

//#include <string.h>
//#include <mqx.h>
//#include <bsp.h>
//#include <psp.h>
//#include <spi.h>
//#include "bitmap.h"
//#include "spi_lcd.h"
//#include "config.h"
//#include "UI_Display.h"
//#include "Button_Layout.h"
//#include "Hw_Timer.h"
//#include "adc_k60.h"

#include "common_headers.h"
extern unsigned char Usb_Connected;
/*function prototype*/
extern void QRD_Disable(void);
extern void QRD_Enable(void);
extern void Init_HardWare(void);
extern void Resume_Task(_task_id Task_ID);
extern _task_id Create_Task(uint_8 Task_Index);
extern void Software_reboot(void);
void DDR_power_pin_init(void);
#endif /* HW_INIT_H_ */

