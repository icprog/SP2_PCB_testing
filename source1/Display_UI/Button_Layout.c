/******************************************************************************
 *
 *   Filename: $data_acquisition.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of user input buttons
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "common_headers.h"

volatile uint_8 Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
static void int_service_routine(void *);
static void Individual_Button_Config(LWGPIO_STRUCT_PTR button_name, LWGPIO_PIN_ID pin_id,
		uint_32 button_mux_irq);
unsigned char Button_Interrupt_Enabled = TRUE;
LWGPIO_STRUCT Left_Button;
LWGPIO_STRUCT Right_Button;
LWGPIO_STRUCT Up_Button;
LWGPIO_STRUCT Down_Button;
LWGPIO_STRUCT Test_Button;

/*-----------------------------------------------------------------------------* 
 * Function:    disable_all_gpio_interrupts
 * Brief:       Disable all 5 button interrupts
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Disable_All_Button_ISR()
{
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Left_Button, FALSE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Right_Button, FALSE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Up_Button, FALSE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Down_Button, FALSE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Test_Button, FALSE );
	Button_Interrupt_Enabled = FALSE;
}
/*-----------------------------------------------------------------------------* 
 * Function:    enable_all_gpio_interrupts
 * Brief:       Enable all 5 button interrupts
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Enable_All_Button_ISR()
{
	lwgpio_int_clear_flag(&Left_Button);
	lwgpio_int_clear_flag(&Right_Button);
	lwgpio_int_clear_flag(&Up_Button);
	lwgpio_int_clear_flag(&Down_Button);
	lwgpio_int_clear_flag(&Test_Button);

	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Left_Button, TRUE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Right_Button, TRUE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Up_Button, TRUE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Down_Button, TRUE );
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Test_Button, TRUE );

	Button_Interrupt_Enabled = TRUE;
}

/*-----------------------------------------------------------------------------* 
 * Function:    int_service_routine
 * Brief:       Interrupt service routine for user input buttons
 * Parameter:   pin id
 * Return:      None
 -----------------------------------------------------------------------------*/
static void int_service_routine(void *)
{
//	Sleep_timer_start(SLEEP_TIME * 60000);
	
	/*BLE Host processor wake up pin isr*/
	if(lwgpio_int_get_flag(&BLE_host_interrupt) == TRUE )
	{	
		lwgpio_int_clear_flag(&BLE_host_interrupt);	
		lwgpio_int_enable(&BLE_host_interrupt, FALSE );	
		BLE_host_wakeup_flag = YES;
		_lwevent_set(&BLE_process_event, SW_EVENT_MASK);
		

	}

	if (Check_Button_Timer_Timeout() == TIME_OUT)
	{
		if (lwgpio_int_get_flag(&Left_Button) == TRUE )
		{
			Button_Press_Status = LEFT_BUTTON_PRESSED;
			lwgpio_int_clear_flag(&Left_Button);

		}

		if (lwgpio_int_get_flag(&Right_Button) == TRUE )
		{
			Button_Press_Status = RIGHT_BUTTON_PRESSED;
			lwgpio_int_clear_flag(&Right_Button);

		}

		if (lwgpio_int_get_flag(&Up_Button) == TRUE )
		{
			Button_Press_Status = UP_BUTTON_PRESSED;
			lwgpio_int_clear_flag(&Up_Button);

		}

		if (lwgpio_int_get_flag(&Down_Button) == TRUE )
		{
			Button_Press_Status = DOWN_BUTTON_PRESSED;
			lwgpio_int_clear_flag(&Down_Button);

		}

		if (lwgpio_int_get_flag(&Test_Button) == TRUE )
		{
			Button_Press_Status = TEST_BUTTON_PRESSED;
			lwgpio_int_clear_flag(&Test_Button);

		}
		
		if (run_mode_flag == 0)
		{
			//        Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			run_mode_flag = 1;
		}

		Button_bounce_timer_start(BUTTON_DE_BOUNCE_TIME); // time in millisec
	}
	else
	{
		lwgpio_int_clear_flag(&Left_Button);
		lwgpio_int_clear_flag(&Right_Button);
		lwgpio_int_clear_flag(&Up_Button);
		lwgpio_int_clear_flag(&Down_Button);
		lwgpio_int_clear_flag(&Test_Button);
	}
	
//	if (lwgpio_int_get_flag(&FIX_AVL) == TRUE )
//	{
//		if(GPS_OFF_timer_count == 0x00)
//		{
//			/* disable interrupt on GPIO peripheral module*/
////			lwgpio_int_enable(&FIX_AVL, FALSE );
//			GPS_ON_timer_start(180000);	// 3 min
//			lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_LOW); 
//			gps_disable();
////			gps_power_off();
//			GPS_Current_State = GPS_OFF;
//		}
//		else
//		{
//			GPS_Current_State = GPS_LOCKING;
//			GPS_Lock_timer_start(3000);	// 3 sec
//		}
//		lwgpio_int_clear_flag(&FIX_AVL);
//	}
}

/*-----------------------------------------------------------------------------* 
 * Function:    Individual_Button_Config
 * Brief:       Initalize input buttons as gpio input
 * Parameter:   button structure pointer,pin id,pin_mux
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Individual_Button_Config(LWGPIO_STRUCT_PTR button_name, LWGPIO_PIN_ID pin_id,
		uint_32 button_mux_irq)
{
	/*******************************************************************************
     Opening the pin for input, initialize interrupt on it and set interrupt handler.
	 *******************************************************************************/
	/* opening pins for input */
	if (!lwgpio_init(button_name, pin_id, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing button GPIO as input failed.\n");
		_task_block();
	}

	/* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(button_name, button_mux_irq);
	lwgpio_set_attribute(button_name, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(button_name, LWGPIO_INT_MODE_FALLING))
	{
		printf("Initializing button GPIO for interrupt failed.\n");
		_task_block();
	}
	lwgpio_int_clear_flag (button_name);
	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(button_name), int_service_routine, (void *) button_name);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(button_name), 3, 0, TRUE);
	    lwgpio_int_clear_flag (button_name);
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(button_name, TRUE );

}

/*-----------------------------------------------------------------------------* 
 * Function:    Button_Init
 * Brief:       Initalize input buttons as gpio input
 * Parameter:   none
 * Return:      None
 -----------------------------------------------------------------------------*/
void Button_Init(void)
{
	//Configuring Left Button 
	Individual_Button_Config(&Left_Button, LEFT_BUTTON, LEFT_BUTTON_MUX_IRQ);
	//Configuring Right Button 
	Individual_Button_Config(&Right_Button, RIGHT_BUTTON, RIGHT_BUTTON_MUX_IRQ);
	//Configuring Up Button 
	Individual_Button_Config(&Up_Button, UP_BUTTON, UP_BUTTON_MUX_IRQ);
	//Configuring Down Button 
	Individual_Button_Config(&Down_Button, DOWN_BUTTON, DOWN_BUTTON_MUX_IRQ);
	//Configuring Test Button 
	Individual_Button_Config(&Test_Button, TEST_BUTTON, TEST_BUTTON_MUX_IRQ);

}

//void GPS_Input_Pin_Init()
//{
//	/* disable interrupt on GPIO peripheral module*/
////	lwgpio_int_enable(&FIX_AVL, FALSE );
//	
//	/*******************************************************************************
//     Opening the pin for input, initialize interrupt on it and set interrupt handler.
//	 *******************************************************************************/
//	/* opening pins for input */
//	if (!lwgpio_init(&FIX_AVL, BSP_FIX_AVL, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
//	{
//		printf("Initializing button GPIO as input failed.\n");
//		_task_block();
//	}
//
//	/* Some platforms require to setup MUX to IRQ functionality,
//     for the rest just set MUXto GPIO functionality */
//	lwgpio_set_functionality(&FIX_AVL, BSP_GPIO_MUX_GPIO);
//	lwgpio_set_attribute(&FIX_AVL, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
//
//	/* enable gpio functionality for given pin, react on falling edge */
//	if (!lwgpio_int_init(&FIX_AVL, LWGPIO_INT_MODE_FALLING))
//	{
//		printf("Initializing button GPIO for interrupt failed.\n");
//		_task_block();
//	}
//	lwgpio_int_clear_flag (&FIX_AVL);
//	/* install gpio interrupt service routine */
//	_int_install_isr(lwgpio_int_get_vector(&FIX_AVL), int_service_routine, (void *) &FIX_AVL);
//	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
//	_bsp_int_init(lwgpio_int_get_vector(&FIX_AVL), 3, 0, TRUE);
//	    lwgpio_int_clear_flag (&FIX_AVL);
//	/* enable interrupt on GPIO peripheral module*/
//	lwgpio_int_enable(&FIX_AVL, TRUE );
//	
//	
////	Individual_Button_Config(&FIX_AVL, BSP_FIX_AVL, BSP_GPIO_MUX_GPIO);
//}

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/

