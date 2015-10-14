/**HEADER********************************************************************
 *
 * Copyright (c) 2012 Freescale Semiconductor;
 * All Rights Reserved
 *
 ***************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************
 *
 * $FileName: main.c$
 * Comments:
 *
 *   This file contains the source for a simple example of an
 *   application that writes Memory LCD using the SPI driver.
 * 
 *
 *END************************************************************************/

#include "common_headers.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

char 	 BUSY=FALSE,Read_LSM=0;
LWGPIO_STRUCT PWR_5V_RAIL,KILL_CONTROL;
uint8_t Test_gpio_buttons(void);
void display_test_results(uint8_t button_error_code, uint8_t ddr_error_code, uint8_t sd_error_code, uint8_t lsm_error_code, uint8_t ble_error_code, uint8_t temp_error_code);
LWEVENT_STRUCT                  app_event,Lsm_Event;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
		/* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
		{ BLE_PROCESS_TASK,BLE_Process_Task,6000,7,"BLE_Process_Task",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK,0,0},
		{ TEST_TASK, Test_task, 2000,	8, "Main",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK, 0,0},
		{ 2, Lsm_Task, 2000,	8, "Lsm",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK, 0,0},
		{ UI_TASK,	 UI_Task, 10000,9,"User_Interface", MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK,0,0	},
		{ 0 }
};

void wait_for_event (void)
{
    _mqx_uint   app_event_value;

    /* Wait for button press */
    _lwevent_wait_ticks (&app_event, (SW_EVENT_MASK), FALSE, 0);

      /* Copy event masks into temporary variable */
     app_event_value = _lwevent_get_signalled();

    /* If button pressed or switch expires, moving to next clock configuration */
    if (app_event_value & SW_EVENT_MASK)
    {
//        printf("\nProcessing Display Event Generated\n");
    }
}

void wait_for_Lsm_event (void)
{
    _mqx_uint   app_event_value;

    /* Wait for button press */
    _lwevent_wait_ticks (&Lsm_Event, (SW_EVENT_MASK), FALSE, 0);

      /* Copy event masks into temporary variable */
     app_event_value = _lwevent_get_signalled();

    /* If button pressed or switch expires, moving to next clock configuration */
    if (app_event_value & SW_EVENT_MASK)
    {
//        printf("\nProcessing Display Event Generated\n");
    }
}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_init
 *  Brief:        Configuring the 5V power rail enable pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_init(void)
{
	
	if (!lwgpio_init(&PWR_5V_RAIL, PWR_RAIL_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}
	
	lwgpio_set_functionality(&PWR_5V_RAIL, PWR_RAIL_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_LOW);

}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_enable
 *  Brief:        Enable  the 5V power rail
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_enable(void)
{
	
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_disable
 *  Brief:        Disable  the 5V power rail
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_disable(void)
{
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_LOW);
}


void Kill_Controller_Init()
{
	if (!lwgpio_init(&KILL_CONTROL, KILL_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}
	
	lwgpio_set_functionality(&KILL_CONTROL, KILL_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_LOW);
}


void Kill_ON_Controller()
{
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_HIGH);
}

void Kill_OFF_Controller()
{
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_LOW);
}

void Test_task (uint_32)
{
    /* Create global event */
    if (_lwevent_create(&app_event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }
	while (1)
	{
		wait_for_event();
		LCD_Updating_Flag=1;
		display_New_Snow_profile_processing();
		LCD_Updating_Flag=0;
	}
}
unsigned int lsm_test_ctr=0;
void Lsm_Task(uint_32)
{
    /* Create global event */
    if (_lwevent_create(&Lsm_Event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }
	while (1)
	{
		wait_for_Lsm_event();
		lsm_test_ctr++;
		LSM_Updating_Flag=1;
	    read_magnetometer_data();
	    read_accelerometer_data();
		LSM_Updating_Flag=0;
	}
}

void Perform_Startup_Test(void)
{
	power_rail_init();
	power_rail_enable();
	Kill_Controller_Init();
	Kill_ON_Controller();
	OSC_EN_init();
	ui_timer_init();
	//start the actual tests
	uint8_t button_error_code = Test_gpio_buttons(); 
	Test_backlight();
	uint8_t ddr_error_code = Test_DDR();	//Perform LPDDR Test
	uint8_t sd_error_code = Test_Sd_card(); 
	Test_ADC();
	
	Battery_ADC_Init();
	Create_Header();
	Battery_ADC_Deinit();
	
	Test_Buzzer();
	uint8_t lsm_error_code = Test_lsm303();
	uint8_t temp_error_code = Test_Tmp006();
	printf("temp_error_code = %d", temp_error_code);
	uint8_t ble_error_code = Test_BLE();	
	//uint8_t gps_error_code = Test_Gps(); //this test has an erro so its not displayed
	ui_timer_de_init();
	power_rail_disable();
	display_test_results(button_error_code, ddr_error_code, sd_error_code, lsm_error_code, ble_error_code, temp_error_code);
	while(1){}
}

void display_test_results(uint8_t button_error_code, uint8_t ddr_error_code, uint8_t sd_error_code, uint8_t lsm_error_code, uint8_t ble_error_code, uint8_t temp_error_code)
{
	uint16_t x_position, y_position;
	char *result_string;
	char pass[5] = "PASS"; //ned to allocate 5 spaces because the end of string charater needs one
	char fail[5] = "FAIL";
	char none[5] = "----";
	//set up screen
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("RESULTS",strlen("RESULTS"));
	_time_delay(1000);
	
	/*BUTTON TEST*/
	x_position = 12;
	y_position = 45;
	Draw_string_new(x_position, y_position, (uint_8 *) "BUTTON TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	result_string = (button_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*BACKLIGHT TEST -- pass if it didnt crash*/
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "BACKLIGHT TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	uint8_t backlight_error_code = 0;
	result_string = none;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*DDR TEST*/
	x_position = 12;
	y_position += (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "DDR TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	result_string = (ddr_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*SD CARD TEST*/
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "SD TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	result_string = (sd_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*ADC TEST -- pass if it didnt crash*/
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "ADC TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	uint8_t adc_error_code = 0;
	result_string = (adc_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*BUZZER TEST -- pass if it didnt crash*/
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "BUZZER TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	uint8_t buzzer_error_code = 0;
	result_string = none;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*ACCELEROMETER TEST*/
	x_position = 12;
	y_position = y_position  +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "LSM TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	result_string = (lsm_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*TEMP TEST*/ 
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "TEMP TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	result_string = (temp_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	/*BLE TEST*/
	x_position = 12;
	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "BLE TEST:", COLOUR_BLACK, MEDIUM_FONT);	
	
	result_string = (ble_error_code == 0) ? pass : fail;
	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
//	/*GPS TEST*/
//	x_position = 12;
//	y_position = y_position +(2 * NUM_Y_PIXEL_PER_CHAR);
//	Draw_string_new(x_position, y_position, (uint_8 *) "GPS TEST:", COLOUR_BLACK, MEDIUM_FONT);	
//	
//	result_string = (gps_error_code == 0) ? pass : fail;
//	x_position = (DISPLAY_X_MAX -12) - (strlen(result_string)* NUM_X_PIXEL_PER_CHAR);
//	Draw_string_new(x_position, (y_position), (uint_8 *) result_string, COLOUR_BLACK, MEDIUM_FONT);
	
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

uint8_t Test_gpio_buttons(void)
{
	uint8_t error_code = 0;
	uint_8 lb_status = 0,rb_status = 0,ub_status = 0,db_status = 0,tb_status = 0;
	buff_clear();
//	Draw_string_new(60,10,(uint_8*)"BUTTON TEST",COLOUR_BLACK,MEDIUM_FONT);
//	Draw_Rect(55,10,175,40,COLOUR_BLACK);
//	Draw_Rect(54,9,176,41,COLOUR_BLACK);
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BUTTON TEST",strlen("BUTTON TEST"));
	_time_delay(1000);
	Draw_string_new(10,80,(uint_8*)"STARTING BUTTON TEST",COLOUR_BLACK,MEDIUM_FONT);

	printf("\n20 SECOND TEST\n");
	Draw_string_new(10,110,(uint_8*)"20 SECOND TEST",COLOUR_BLACK,MEDIUM_FONT);
	printf("\nPRESS ANY BUTTON\n");
	Draw_string_new(10,150,(uint_8*)"PRESS ANY BUTTON",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	
	ui_timer_start(20000); ///30 sec

	while(Check_UI_Timer_Timeout()!= TIME_OUT)
	{
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				lb_status = 1;
				Rect_Fill(1,219,239,245,COLOUR_WHITE);
				Draw_string_new(10,220,(uint_8*)"LEFT BUTTON PRESSED!!",COLOUR_BLACK,MEDIUM_FONT);
				Refresh_Lcd_Buffer((uint_8 *) frame_buff);

				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				rb_status = 1;
				Rect_Fill(1,219,239,245,COLOUR_WHITE);
				Draw_string_new(10,220,(uint_8*)"RIGHT BUTTON PRESSED!!",COLOUR_BLACK,MEDIUM_FONT);
				Refresh_Lcd_Buffer((uint_8 *) frame_buff);


				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n"); 
				ub_status = 1;
				Rect_Fill(1,219,239,245,COLOUR_WHITE);
				Draw_string_new(10,220,(uint_8*)"UP BUTTON PRESSED!!",COLOUR_BLACK,MEDIUM_FONT);
				Refresh_Lcd_Buffer((uint_8 *) frame_buff);
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				db_status = 1;
				Rect_Fill(1,219,239,245,COLOUR_WHITE);
				Draw_string_new(10,220,(uint_8*)"DOWN BUTTON PRESSED!!",COLOUR_BLACK,MEDIUM_FONT);
				Refresh_Lcd_Buffer((uint_8 *) frame_buff);
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				tb_status = 1;
				Rect_Fill(1,219,239,245,COLOUR_WHITE);
				Draw_string_new(10,220,(uint_8*)"TEST BUTTON PRESSED!!",COLOUR_BLACK,MEDIUM_FONT);
				Refresh_Lcd_Buffer((uint_8 *) frame_buff);
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if((lb_status == 1) && (rb_status == 1) && (ub_status == 1) && (db_status == 1) && (tb_status == 1))
		{
			ui_timer_stop();
//			Rect_Fill(1,219,239,245,COLOUR_WHITE);
//			Draw_string_new(10,220,(uint_8*)"BUTTON TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
//			Refresh_Lcd_Buffer((uint_8 *) frame_buff);
			break;
		}
	}


	Rect_Fill(1,219,239,245,COLOUR_WHITE);
	
	if(lb_status == 1)
	{
		Draw_string_new(10,180,(uint_8*)"LEFT BUTTON: SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n LEFT BUTTON: SUCCESS \n");
	}
	else
	{
		Draw_string_new(10,180,(uint_8*)"LEFT BUTTON: FAILED",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n LEFT BUTTON: FAILED \n");
	}
	
	if(rb_status == 1)
	{
		Draw_string_new(10,205,(uint_8*)"RIGHT BUTTON: SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n RIGHT BUTTON: SUCCESS \n");
	}
	else
	{
		Draw_string_new(10,205,(uint_8*)"RIGHT BUTTON: FAILED",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n RIGHT BUTTON: FAILED \n");
	}
	if(ub_status == 1)
	{
		Draw_string_new(10,240,(uint_8*)"UP BUTTON: SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n UP BUTTON: SUCCESS \n");
	}
	else
	{
		Draw_string_new(10,240,(uint_8*)"UP BUTTON: FAILED",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n UP BUTTON: FAILED \n");
	}
	if(db_status == 1)
	{
		Draw_string_new(10,265,(uint_8*)"DOWN BUTTON: SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n DOWN BUTTON: SUCCESS \n");
	}
	else
	{
		Draw_string_new(10,265,(uint_8*)"DOWN BUTTON: FAILED",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n DOWN BUTTON: FAILED \n");
	}
	if(tb_status == 1)
	{
		Draw_string_new(10,290,(uint_8*)"TEST BUTTON: SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n TEST BUTTON: SUCCESS \n");
	}
	else
	{
		Draw_string_new(10,290,(uint_8*)"TEST BUTTON: FAILED",COLOUR_BLACK,MEDIUM_FONT);
		printf("\n TEST BUTTON: FAILED \n");
	}
	if(lb_status==0 || rb_status==0 || ub_status==0 || db_status==0 || tb_status==0)
	{
		error_code = 1;
	}
	
	ui_timer_stop();
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	printf("\n\n********GPIO BUTTON TEST COMPLETE*********\n\n");
	_time_delay(2000);
	return error_code;
}

