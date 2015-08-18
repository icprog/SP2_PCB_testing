/******************************************************************************
 *
 *   Filename: $data_acquisition.h
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains header data  of user input buttons
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#ifndef BUTTON_LAYOUT_H_
#define BUTTON_LAYOUT_H_

#define LEFT_BUTTON				 (GPIO_PORT_C | GPIO_PIN6)
#define LEFT_BUTTON_MUX_IRQ 	 (LWGPIO_MUX_C6_GPIO)

#define RIGHT_BUTTON			 (GPIO_PORT_C | GPIO_PIN3)
#define RIGHT_BUTTON_MUX_IRQ	 (LWGPIO_MUX_C3_GPIO)

#define UP_BUTTON			 	(GPIO_PORT_C | GPIO_PIN5)
#define UP_BUTTON_MUX_IRQ	 	(LWGPIO_MUX_C5_GPIO)

#define DOWN_BUTTON				 (GPIO_PORT_C | GPIO_PIN4)
#define DOWN_BUTTON_MUX_IRQ 	 (LWGPIO_MUX_C4_GPIO)

#define TEST_BUTTON				 (GPIO_PORT_C | GPIO_PIN1)
#define TEST_BUTTON_MUX_IRQ 	 (LWGPIO_MUX_C1_GPIO)

#define BUTTON_PRESS_NOT_PENDING 0
#define LEFT_BUTTON_PRESSED 	1
#define RIGHT_BUTTON_PRESSED 	2
#define UP_BUTTON_PRESSED		3
#define DOWN_BUTTON_PRESSED		4
#define TEST_BUTTON_PRESSED		5

extern volatile uint_8 Button_Press_Status;
extern unsigned char Button_Interrupt_Enabled;
extern void Button_Init(void);
void GPS_Input_Pin_Init();
void Disable_All_Button_ISR(void);
void Enable_All_Button_ISR(void);

#endif /* BUTTON_LAYOUT_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
