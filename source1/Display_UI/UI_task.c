/******************************************************************************
 *
 *   Filename: $UI_task.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of processing user feedback
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "common_headers.h"
#include "matrix_operations.h"

TEST_COUNT_FLASH Test_Count_Flash;

unsigned char LCD_Updating_Flag,LSM_Updating_Flag=0;
uint_16 img_date_word=0, img_time_word=0;
uint_16 calib_date_word=0, calib_time_word=0;
MFS_DATE_TIME_PARAM Image_File_Attr;
MFS_DATE_TIME_PARAM Calib_File_Attr;
IMAGE_PARA SP1_IMG,CALIB_IMG;
char ble_daily_sync_file_name[20]={"\0"};
static void update_ui_screen(void);
static uint_8 temp_ble_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
uint_32 GPS_Wakeup_Time = 0;
STATE_OF_SCREEN State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF, 
		Next_State = UI_MAINMENU_LIST,Test_Menu_Prev_State=UI_MAINMENU_LIST,Prev_USB_State; 
STATE_OF_SCREEN Prev_State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
extern uchar usb_test_flag;
uint8_t beep_ctr = 0, type_of_error = TOO_SLOW_ERROR_CONDITION;
static int Ble_status = 0;
uint_8 temp_seconds_count=0, details_previous_state = LAST_STATE_IS_FILE_LIST;

static uint_8  Calib_result = INCOMPLETE;

int_16 Angle_result = 0;
uint_16 Heading_result = 0;
uint_16 mag_temp_heading_buff[300];
int_16 Count_angle = 0;
int_16 Count_heading = 0;
char 	aspect[4];
uint_16 magnetic_heading = 0xffff;
int_16  tempAngle = 0xffff;
uint8_t Calib_Flash_Buf[120];
//uint_8 test_menu_previous_state= TEST_MENU_BACK_BUTTON_MAIN_MENU;

void Time_Delay_Sleep(unsigned int delay_numbr)
{
	uint32_t ctr=0;
	while(delay_numbr)
	{
		for(ctr=0;ctr<0x2000;ctr++)
		{

		}
		delay_numbr--;
	}
}


/*-----------------------------------------------------------------------------* 
 * Function:    User_intrface_task
 * Brief:       It is the main task handling all the device activities
 * Parameter:   Task id
 * Return:      None
 -----------------------------------------------------------------------------*/
void UI_Task(uint_32 )
{
	MQX_FILE_PTR img_fd_ptr; 
	Image_File_Attr.DATE_PTR = &img_date_word;
	Image_File_Attr.TIME_PTR = &img_time_word;
	Calib_File_Attr.DATE_PTR = &calib_date_word;
	Calib_File_Attr.TIME_PTR = &calib_time_word;
	SP1_IMG.CRC_Status = 0;SP1_IMG.Update_Status=0;
	CALIB_IMG.CRC_Status = 0;CALIB_IMG.Update_Status=0;	

	Core_clock_selection = ACQUISITION_MODE;
	memset(Calib_Flash_Buf,0xFF,100);
	/*Initializing all sensor status flags to zero*/
	Init_Sensor_status();
	ADC_Init();
	ADC_deinit();

	printf("Initialising LCD...\n");
	spi_init(); /*Init SPI configuration*/
	lcd_clear(); /* Clearing LCD */
	buff_clear();
	printf("Initialising SD CARD ...\n");
	if(	Sd_Card_init()==1)
	{
		printf("SD CARD initialisation Failed...\n");
		//Handle SD card Error condition
		display_sd_error_screen();
		while (1)
		{

		}
	}

	printf("SD CARD initialisation Completed...\n");
	USB_Init();
	USB_DPin_Low();

	/* Reading Date & Time Parameters of image.bin file*/
	img_fd_ptr = fopen("a:image.bin", "r");
	if (img_fd_ptr == NULL)
	{
		printf("No image file found\n");
		img_date_word = 0;
		img_time_word = 0;
		//		return 0;
	}
	else
	{
		ioctl(img_fd_ptr, IO_IOCTL_GET_DATE_TIME,(uint_32_ptr) &Image_File_Attr);
		fclose(img_fd_ptr);	
	}

	/* Reading Date & Time Parameters of calib.bin file*/
	img_fd_ptr = fopen("a:calib.bin", "r");
	if (img_fd_ptr == NULL)
	{
		printf("No image file found\n");
		calib_date_word=0, calib_time_word=0;
		//		return 0;
	}
	else
	{
		ioctl(img_fd_ptr, IO_IOCTL_GET_DATE_TIME,(uint_32_ptr) &Calib_File_Attr);
		fclose(img_fd_ptr);	
	}


	Check_Serial();


	Read_calib_data("a:IRDMS_calib_data.txt",&IRDMS_Calib_Table);
	Read_calib_data("a:Pressure_calib_data.txt",&Pressure_Calib_Table);
	Read_calib_data("a:ROS_calib_data.txt",&ROS1_Calib_Table);
	Read_calib_data("a:ROS_calib_data.txt",&ROS2_Calib_Table);
	Read_calib_data("a:Accelerometer_calib_data.txt",&Accelerometer_Calib_Table);
	Read_calib_data("a:Magnetometer_calib_data.txt",&Magnetometer_Calib_Table);

	printf("\n IRDMS Calib Data\n");
	print_calib_data(&IRDMS_Calib_Table,5);
	printf("\n Pressure Calib Data\n");
	print_calib_data(&Pressure_Calib_Table,5);	
	printf("\n ROS1 Calib Data\n");
	print_calib_data(&ROS1_Calib_Table,2);
	printf("\n ROS2 Calib Data\n");
	print_calib_data(&ROS2_Calib_Table,2);
	printf("\n Accelerometer Calib Data\n");
	print_calib_data(&Accelerometer_Calib_Table,4);	
	printf("\n Magnetometer Calib Data\n");
	print_calib_data(&Magnetometer_Calib_Table,4);


	//	Update_rtc(Time_from_settings,Date_from_settings); 
	Init_HardWare();
	memset(Calib_status,0x00,7);
	memset(mag_temp_heading_buff,0x00,200);
	Update_Calib_Buff();
	State_of_Screen = UI_CALIBRATION_MENU;
	display_Mainmenu();

	while(1)
	{
		ui_screen_update();
	}
}

ui_screen_update(void)
{
	switch (State_of_Screen)
	{

	case UI_CALIBRATION_MENU:
		//		printf("\nUI_CALIBRATION_MENU\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_CALIBRATION_MENU;
				State_of_Screen = UI_CALIBRATION_MENU;
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				State_of_Screen = Mainmenu_Table[Main_menu_selection].UI_state;

				if ((State_of_Screen == UI_CALIBRATION_IRDMS) || (State_of_Screen == UI_CALIBRATION_PRESSURE) 
						|| (State_of_Screen == UI_CALIBRATION_ROS))
				{							
					ADC_Init();
					ui_timer_start(CAL_REFRESH_TIME);
				}
				else if (State_of_Screen == UI_ACCELEROMETER_CALIBRATION_SCREEN_DOWN)
				{
					accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_DOWN;		
				}
				else if (State_of_Screen == UI_CALIBRATION_ACCELEROMETER)
				{
					lsm303_i2c0_init();
					Start_LSM();
					Read_Acc_Calib_Dat();
					ui_timer_start(CAL_REFRESH_TIME);	
				}
				else if(State_of_Screen == UI_CALIBRATION_GPS)
				{
					strncpy(Data_header.Time_stamp.Time,"NA",16);
					strncpy(Data_header.Time_stamp.Date,"NA",16);
					strncpy(Data_header.Gps_data.Latitude,"NA",16);
					strncpy(Data_header.Gps_data.Longitude,"NA",16);
					resetCC4000API();
					/*GPIO for GPS*/
					gps_gpio_init();
					gps_regulator_init(); 
					gps_power_on();
					lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
					gpsCC4000OStartFixes(); 
					GPS_uart_init();
					RTC_TIME_STRUCT Gps_Time;
					_rtc_get_time (&Gps_Time);
					// setting new time
					Gps_Time.seconds = 0;	
					_rtc_set_time(&Gps_Time);
					ui_timer_start(GPS_REFRESH_TIME);
				}
				else if(State_of_Screen == UI_COMPASS_CALIBRATION)
				{
					compass_calibration_screen = COMPASS_CALIBRATION_START_SCREEN;
					Buzzer_Init();
					Buzzer_Short_Beep(1);
					Magnetometer_Calib_process = 1;
					//						power_rail_enable();
					lsm303_i2c0_init();
					/*
					     These values lead to an assumed magnetometer bias of 0.
					     Use the Calibrate example program to determine appropriate values
					     for your particular unit. The Heading example demonstrates how to
					     adjust these values in your own sketch.
					 */
//					m_min_x = +32767;
//					m_min_y = +32767;
//					m_min_z = +32767;
//
//					m_max_x = -32767;
//					m_max_y = -32767;
//					m_max_z = -32767;
					reset_calibration_values();
					Start_LSM();
					ui_timer_start(10000);
					compass_calibration_screen = COMPASS_CALIBRATION_ROLL_SCREEN;
					//					Next_State = UI_COMPASS_CALIBRATION;
					//					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					display_compass_calibration_screen();
					//Call calibration start function here.			

					break;
				}

				//						State_of_Screen = UI_CALIBRATION_MENU;
				Mainmenu_Table[Main_menu_selection].menu_Function();
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_CALIBRATION_MENU;
				State_of_Screen = UI_CALIBRATION_MENU;
				Mainmenu_Key_up();
				display_Mainmenu();

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_CALIBRATION_MENU;
				State_of_Screen = UI_CALIBRATION_MENU;
				Mainmenu_Key_down();
				display_Mainmenu();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			if(Usb_Connected == 1)
			{
				Usb_Connected = 0;
				if(((1 << GPIO_PIN17) & GPIOA_PDIR) != 0)
				{
					// usb is not now connected
				}
				else
				{
					Uninstall_FS_for_MSD();
					State_of_Screen = UI_MASS_STORAGE_ENABLE;
					USB_DPin_High();
					display_mass_storage_enable();
				}
			}
		}
		break;


	case UI_MASS_STORAGE_ENABLE:
		//    	printf("UI_MASS_STORAGE_ENABLE\n");
		if(((1 << GPIO_PIN17) & GPIOA_PDIR) != 0)
		{
			USB_DPin_Low();
			Time_Delay_Sleep(2000);
			Usb_Connected = 0;
			
			Re_install_FS_for_MSD();
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			char pathname[261];
			ioctl(filesystem_handle, IO_IOCTL_GET_CURRENT_DIR,(uint_32_ptr) pathname);
			ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
			if(Check_Firmware_Update()==1)
			{
				Display_Validating_Firmware();
				//				ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
				if(Find_CRC()==1)
				{
					ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
					State_of_Screen = UI_FIRMWARE_UPDATE;
					Display_Firmware_Update_Available();
				}
				else
				{
					ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
					Read_calib_data("a:IRDMS_calib_data.txt",&IRDMS_Calib_Table);
					Read_calib_data("a:Pressure_calib_data.txt",&Pressure_Calib_Table);
					Read_calib_data("a:ROS_calib_data.txt",&ROS1_Calib_Table);
					Read_calib_data("a:ROS_calib_data.txt",&ROS2_Calib_Table);
					Read_calib_data("a:Accelerometer_calib_data.txt",&Accelerometer_Calib_Table);
					Read_calib_data("a:Magnetometer_calib_data.txt",&Magnetometer_Calib_Table);
					State_of_Screen = UI_FIRMWARE_CORRUPT;
					Display_Firmware_Corrupted();
				}
			}
			else
			{
				ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
				Read_calib_data("a:IRDMS_calib_data.txt",&IRDMS_Calib_Table);
				Read_calib_data("a:Pressure_calib_data.txt",&Pressure_Calib_Table);
				Read_calib_data("a:ROS_calib_data.txt",&ROS1_Calib_Table);
				Read_calib_data("a:ROS_calib_data.txt",&ROS2_Calib_Table);
				Read_calib_data("a:Accelerometer_calib_data.txt",&Accelerometer_Calib_Table);
				Read_calib_data("a:Magnetometer_calib_data.txt",&Magnetometer_Calib_Table);
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
			}
		}
		break;


	case UI_FIRMWARE_UPDATE:
		//		printf("\nUI_FIRMWARE_UPDATE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				Write_SDcard_DFU();
//				Write_Flash_DFU();
				Sd_Card_deinit();
				Software_reboot();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;


	case UI_FIRMWARE_CORRUPT:
		//		printf("\nUI_FIRMWARE_UPDATE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				//				Display_Firmware_Corrupted();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//				Display_Firmware_Corrupted();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//				Display_Firmware_Corrupted();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				//				Display_Firmware_Corrupted();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	case UI_CALIBRATION_IRDMS:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				ADC_deinit();			
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				Calib_result = calibrate_IRDMS();
				if(Calib_result == COMPLETED)
				{
					Calib_status[IRDMS_CALIB] = COMPLETED;
					for(int i =0; i < NUM_OF_IRDMS_CONDITION; i++)
					{
						IRDMS_Calib_Table[i].Calib_status = INCOMPLETE;
					}
					ADC_deinit();	
					State_of_Screen = UI_CALIBRATION_MENU;
					display_Mainmenu();
					Write_IRDMS_Data();
					break;
				}
				else if(Calib_result == INCOMPLETE)
				{
					if(Calib_status[IRDMS_CALIB] == COMPLETED)
					{
						Stop_PDB_Timer();
						Write_IRDMS_Data();						
						Start_PDB_Timer();
					}
					display_IRDMS_Calibration();	
				}
				else if(Calib_result == OUT_OF_RANGE)
				{
					ADC_deinit();
					Next_State = UI_CALIBRATION_IRDMS;
					State_of_Screen = UI_CALIBRATION_ERROR_SCREEN;					
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_IRDMS;
				IRDMS_Condition_Key_up();
				display_IRDMS_Calibration();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_IRDMS;
				IRDMS_Condition_Key_down();
				display_IRDMS_Calibration();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_IRDMS))
		{          
			display_IRDMS_Calibration();
			ui_timer_start(CAL_REFRESH_TIME);
		}		
		break;	

	case UI_CALIBRATION_PRESSURE:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				ADC_deinit();			
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				Calib_result = calibrate_Pressure_Sensor();
				if(Calib_result == COMPLETED)
				{
					Calib_status[PRESSURE_CALIB] = COMPLETED;
					for(int i =0; i < NUM_OF_PRESSURE_CONDITION; i++)
					{
						Pressure_Calib_Table[i].Calib_status = INCOMPLETE;
					}
					ADC_deinit();	
					State_of_Screen = UI_CALIBRATION_MENU;
					display_Mainmenu();
					Write_Pressure_Data();
					break;
				}
				else if(Calib_result == INCOMPLETE)
				{				
					if(Calib_status[PRESSURE_CALIB] == COMPLETED)
					{
						Stop_PDB_Timer();
						Write_Pressure_Data();						
						Start_PDB_Timer();
					}
					display_Pressure_sensor_Calibration();	
				}
				else if(Calib_result == OUT_OF_RANGE)
				{
					ADC_deinit();
					Next_State = UI_CALIBRATION_PRESSURE;
					State_of_Screen = UI_CALIBRATION_ERROR_SCREEN;					
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_PRESSURE;
				Pressure_Condition_Key_up();
				display_Pressure_sensor_Calibration();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_PRESSURE;
				Pressure_Condition_Key_down();
				display_Pressure_sensor_Calibration();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_PRESSURE))
		{          
			display_Pressure_sensor_Calibration();
			ui_timer_start(CAL_REFRESH_TIME);
		}		
		break;	


	case UI_ACCELEROMETER_CALIBRATION_SCREEN_DOWN:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");		
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");	
				Acc_calib_Buff_Clear();
				ui_timer_init();
				Buzzer_Init();
				lsm303_i2c0_init();
				Start_LSM();
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_SCREEN_DOWN);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_UP;
				State_of_Screen = UI_ACCELEROMETER_CALIBRATION_SCREEN_UP;
				display_Collect_Accelerometer_Calibration_Data();
				break;

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;		

	case UI_ACCELEROMETER_CALIBRATION_SCREEN_UP:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				Lsm303_deinit();
				Stop_LSM();					
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");		
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_SCREEN_UP);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_TIP_POINT_DOWN;
				State_of_Screen = UI_ACCELEROMETER_CALIBRATION_TIP_POINT_DOWN;
				display_Collect_Accelerometer_Calibration_Data();
				break;

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;		

	case UI_ACCELEROMETER_CALIBRATION_TIP_POINT_DOWN:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");		
				Lsm303_deinit();
				Stop_LSM();		
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");		
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_TIP_POINT_DOWN);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_TIP_POINT_UP;
				State_of_Screen = UI_ACCELEROMETER_CALIBRATION_TIP_POINT_UP;
				display_Collect_Accelerometer_Calibration_Data();
				break;

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;		

	case UI_ACCELEROMETER_CALIBRATION_TIP_POINT_UP:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");		
				Lsm303_deinit();
				Stop_LSM();		
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");		
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_TIP_POINT_UP);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_FACE_IN;
				State_of_Screen = UI_ACCELEROMETER_CALIBRATION_SCREEN_FACE_IN;
				display_Collect_Accelerometer_Calibration_Data();
				break;

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;		

	case UI_ACCELEROMETER_CALIBRATION_SCREEN_FACE_IN:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");		
				Lsm303_deinit();
				Stop_LSM();		
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");		
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_SCREEN_FACE_IN);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_FACE_OUT;
				State_of_Screen = UI_ACCELEROMETER_CALIBRATION_SCREEN_FACE_OUT;
				display_Collect_Accelerometer_Calibration_Data();
				break;

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;		

	case UI_ACCELEROMETER_CALIBRATION_SCREEN_FACE_OUT:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");	
				Lsm303_deinit();
				Stop_LSM();		
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");		
				_time_delay(2500);
				Buzzer_Short_Beep(0);
				collect_accelerometer_data(ACC_CALIBRATION_ACC_SCREEN_FACE_OUT);
				Buzzer_Short_Beep(1);
				accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_DOWN;
				Calib_Acc_Avg();
				
				//calibrate the accelerometer with the values collected
				Write_Calib_Acc_Dat(); //write the w matrix
				acc_transform_wrapper();
				Calib_status[IRDMS_CALIB] = COMPLETED;
				
//				Lsm303_deinit();
//				Stop_LSM();		
//				State_of_Screen = UI_CALIBRATION_MENU;
//				display_Mainmenu();
				
				State_of_Screen = UI_CALIBRATION_ACCELEROMETER;
				lsm303_i2c0_init();
				Start_LSM();
				Read_Acc_Calib_Dat();
				ui_timer_start(CAL_REFRESH_TIME);
				

			default:
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;		


	case UI_CALIBRATION_ACCELEROMETER:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				Lsm303_deinit();
				Stop_LSM();					
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				Calib_result = calibrate_Accelerometer2();
				if(Calib_result == COMPLETED)
				{
					Calib_status[ACCELEROMETER_CALIB] = COMPLETED;
					for(int i =0; i < NUM_OF_ACC_CONDITION; i++)
					{
						Accelerometer_Calib_Table[i].Calib_status = INCOMPLETE;
					}

					Lsm303_deinit();
					Stop_LSM();					
					State_of_Screen = UI_CALIBRATION_MENU;
					display_Mainmenu();
					Write_ACC_Data();
					break;
				}
				else if(Calib_result == INCOMPLETE)
				{
					if(Calib_status[ACCELEROMETER_CALIB] == COMPLETED)
					{
						Stop_PDB_Timer();
						Write_ACC_Data();						
						Start_PDB_Timer();
					}
					display_Accelerometer_Calibration();	
				}
				else if(Calib_result == OUT_OF_RANGE)
				{
					Lsm303_deinit();
					Stop_LSM();	
					Next_State = UI_CALIBRATION_ACCELEROMETER;
					State_of_Screen = UI_CALIBRATION_ERROR_SCREEN;					
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_ACCELEROMETER;
				Accelerometer_Condition_Key_up();
				display_Accelerometer_Calibration();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_ACCELEROMETER;
				Accelerometer_Condition_Key_down();
				display_Accelerometer_Calibration();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_ACCELEROMETER))
		{          
			display_Accelerometer_Calibration();
			ui_timer_start(CAL_REFRESH_TIME);
		}		
		else if(State_of_Screen == UI_CALIBRATION_ACCELEROMETER)
		{

			float roll, pitch;
//			transform_raw_acc();
			transform_raw_acc_manual();
			get_euler_angles(&roll, &pitch);
			printf("roll= %f pitch = %f \n", roll, pitch);
			tempAngle = (int_16)pitch;
			Angle_result = Angle_result + tempAngle;
			Count_angle++;

		}
		break;		
		
	case UI_CALIBRATION_MAGNETOMETER:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				Lsm303_deinit();
				Stop_LSM();					
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				Calib_result = calibrate_Magnetometer();
				if(Calib_result == COMPLETED)
				{
					Calib_status[MAGNETOMETER_CALIB] = COMPLETED;

					for(int i =0; i < NUM_OF_MAG_CONDITION; i++)
					{
						Magnetometer_Calib_Table[i].Calib_status = INCOMPLETE;
					}

					Lsm303_deinit();
					Stop_LSM();					
					State_of_Screen = UI_CALIBRATION_MENU;
					display_Mainmenu();
					Write_MAG_Data();
					break;
				}
				else if(Calib_result == INCOMPLETE)
				{					
					if(Calib_status[MAGNETOMETER_CALIB] == COMPLETED)
					{
						Stop_PDB_Timer();
						Write_MAG_Data();						
						Start_PDB_Timer();
					}
					display_Magnetometer_Calibration();	
				}
				else if(Calib_result == OUT_OF_RANGE)
				{
					Lsm303_deinit();
					Stop_LSM();	
					Next_State = UI_CALIBRATION_MAGNETOMETER;
					State_of_Screen = UI_CALIBRATION_ERROR_SCREEN;					
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_MAGNETOMETER;
				Magnetometer_Condition_Key_up();
				display_Magnetometer_Calibration();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_MAGNETOMETER;
				Magnetometer_Condition_Key_down();
				display_Magnetometer_Calibration();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_MAGNETOMETER))
		{          
			display_Magnetometer_Calibration();
			ui_timer_start(CAL_REFRESH_TIME);
		}	
		else if(State_of_Screen == UI_CALIBRATION_MAGNETOMETER)
		{
			if(get_slope_measurement(&tempAngle, aspect, &magnetic_heading) == 0)
			{
				mag_temp_heading_buff[Count_heading++] = magnetic_heading;
			}
		}
		break;

	case UI_CALIBRATION_ROS:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				ADC_deinit();			
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				Calib_result = calibrate_ROS_Sensors();
				if(Calib_result == COMPLETED)
				{
					Calib_status[ROS_CALIB] = COMPLETED;
					for(int i =0; i < NUM_OF_ROS_CONDITION; i++)
					{
						ROS1_Calib_Table[i].Calib_status = INCOMPLETE;
					}
					ADC_deinit();	
					State_of_Screen = UI_CALIBRATION_MENU;
					display_Mainmenu();
					Write_ROS_Data();
					break;
				}
				else if(Calib_result == INCOMPLETE)
				{					
					if(Calib_status[ROS_CALIB] == COMPLETED)
					{
						Stop_PDB_Timer();
						Write_ROS_Data();						
						Start_PDB_Timer();
					}
					display_ROS_Calibration();	
				}
				else if(Calib_result == OUT_OF_RANGE)
				{
					ADC_deinit();
					Next_State = UI_CALIBRATION_ROS;
					State_of_Screen = UI_CALIBRATION_ERROR_SCREEN;					
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_ROS;
				ROS1_Condition_Key_up();
				display_ROS_Calibration();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_CALIBRATION_ROS;
				ROS1_Condition_Key_down();
				display_ROS_Calibration();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_ROS))
		{          
			display_ROS_Calibration();
			ui_timer_start(CAL_REFRESH_TIME);
		}		
		break;
		
	case UI_CALIBRATION_GPS:
		//		printf("\nUI_FIRMWARE_UPDATE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
				gps_power_off();
				GPS_Current_State = GPS_POWER_OFF;
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		if ((Check_UI_Timer_Timeout() == TIME_OUT) && (State_of_Screen == UI_CALIBRATION_GPS))
		{          
			ui_timer_start(GPS_REFRESH_TIME);
			display_GPS_Calibration();
			if(gps_data_read()== 0)
			{
				RTC_TIME_STRUCT Gps_Time;
				_rtc_get_time (&Gps_Time);
				if(Gps_Time.seconds>60)
				{
					Refresh_Lcd_Buffer((uint_8 *) Calibration_SENSOR_ERROR);
				}
				else
				{
					// save data
					Write_GPS_Data(Gps_Time.seconds);
				}
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
				gps_power_off();
				GPS_Current_State = GPS_POWER_OFF;
				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();

			}
		}
		break;
		
	case UI_ACCELEROMETER_CALIB_ERROR_SCREEN:
		if (Button_Press_Status == LEFT_BUTTON_PRESSED)
		{
			State_of_Screen = UI_CALIBRATION_MENU;
			display_Mainmenu();
		}
		Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			
		break;

	case UI_CALIBRATION_ERROR_SCREEN:

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");	

				State_of_Screen = Next_State;
				switch(State_of_Screen)
				{
				case UI_CALIBRATION_IRDMS:
					ADC_Init();
					display_IRDMS_Calibration();
					ui_timer_start(CAL_REFRESH_TIME);						
					break;

				case UI_CALIBRATION_PRESSURE:
					ADC_Init();
					display_Pressure_sensor_Calibration();
					ui_timer_start(CAL_REFRESH_TIME);	
					break;

				case UI_CALIBRATION_ACCELEROMETER:
					lsm303_i2c0_init();
					Start_LSM();	
					display_Accelerometer_Calibration();
					ui_timer_start(CAL_REFRESH_TIME);							
					break;

				case UI_CALIBRATION_MAGNETOMETER:							
					lsm303_i2c0_init();
					Start_LSM();	
					display_Magnetometer_Calibration();
					ui_timer_start(CAL_REFRESH_TIME);	
					break;

				case UI_CALIBRATION_ROS:
					ADC_Init();
					display_ROS_Calibration();
					ui_timer_start(CAL_REFRESH_TIME);

					break;
					
				case UI_CALIBRATION_GPS:
					strncpy(Data_header.Time_stamp.Time,"NA",16);
					strncpy(Data_header.Time_stamp.Date,"NA",16);
					strncpy(Data_header.Gps_data.Latitude,"NA",16);
					strncpy(Data_header.Gps_data.Longitude,"NA",16);
					resetCC4000API();
					/*GPIO for GPS*/
					gps_gpio_init();
					gps_regulator_init(); 
					gps_power_on();
					lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
					gpsCC4000OStartFixes(); 
					GPS_uart_init();
					RTC_TIME_STRUCT Gps_Time;
					_rtc_get_time (&Gps_Time);
					// setting new time
					Gps_Time.seconds = 0;	
					_rtc_set_time(&Gps_Time);
					ui_timer_start(GPS_REFRESH_TIME);
					break;					
				}
				break;

				default:
					printf("Invalid Button Pressed \n");
					break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	case UI_COMPASS_CALIBRATION:
		//		printf("UI_COMPASS_CALIBRATION\n");
		/*
           BUTTON		NEW STATE														SOFTWARE ACTION
           LEFT		On first input: (1) Main Menu Otherwise: select previous input	
           RIGHT		On correct password: (9-2) Settings		Otherwise: Select next input	
           UP		Increment selected input	
           DOWN	Decrement selected input	
           TEST	(4) Test Menu (default)

           Additional Notes:
           Inputs: min = 0; max = 0; step = 1; default = 0;
		 */
		if( compass_calibration_screen != COMPASS_CALIBRATION_START_SCREEN)
		{
			calibrate_magnetometer();
		}
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//				if((compass_calibration_screen == COMPASS_CALIBRATION_ROLL_SCREEN) || (compass_calibration_screen == COMPASS_CALIBRATION_SPIN_SCREEN)
				//						|| (compass_calibration_screen == COMPASS_CALIBRATION_FLIP_SCREEN))
				//				{
				ui_timer_stop();
				Stop_LSM();
				Lsm303_deinit();
				//					power_rail_disable();

				State_of_Screen = UI_CALIBRATION_MENU;
				display_Mainmenu();
				//				}
				//				else if(compass_calibration_screen == COMPASS_CALIBRATION_START_SCREEN)
				//				{
				//					Magnetometer_Calib_process=0;
				//					State_of_Screen = UI_CALIBRATION_MENU;
				//					display_Mainmenu();
				//				}
				break;

			case RIGHT_BUTTON_PRESSED:
				//				if(compass_calibration_screen == COMPASS_CALIBRATION_START_SCREEN)
				//				{
				//					Buzzer_Init();
				//					Buzzer_Short_Beep(1);
				//					Magnetometer_Calib_process = 1;
				//					power_rail_enable();
				//					lsm303_i2c0_init();
				//					/*
				//				     These values lead to an assumed magnetometer bias of 0.
				//				     Use the Calibrate example program to determine appropriate values
				//				     for your particular unit. The Heading example demonstrates how to
				//				     adjust these values in your own sketch.
				//					 */
				//					m_min_x = +32767;
				//					m_min_y = +32767;
				//					m_min_z = +32767;
				//
				//					m_max_x = -32767;
				//					m_max_y = -32767;
				//					m_max_z = -32767;
				//					Start_LSM();
				//					ui_timer_start(5000);
				//					compass_calibration_screen = COMPASS_CALIBRATION_ROLL_SCREEN;
				//					//					Next_State = UI_COMPASS_CALIBRATION;
				//					//					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//					display_compass_calibration_screen();
				//					//Call calibration start function here.			
				//				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if((Check_UI_Timer_Timeout() == TIME_OUT) && (compass_calibration_screen != COMPASS_CALIBRATION_START_SCREEN))
		{

			Buzzer_Short_Beep(1);
			compass_calibration_screen = (++compass_calibration_screen)%4;
			if(compass_calibration_screen == 0)
			{

				update_magnetomer_calib_data();
//				printf( "FINAL\r\n min: {%+6d, %+6d, %+6d}    max: {%+6d, %+6d, %+6d}\r\n", m_min_x, m_min_x, m_min_z,m_max_x, m_max_y, m_max_z);

				Magnetometer_Calib_process=0;
				ui_timer_stop();

				ui_timer_start(CAL_REFRESH_TIME);

				State_of_Screen = UI_CALIBRATION_MAGNETOMETER;
				display_Magnetometer_Calibration();
				Buzzer_Off();
				break;
			}

			//			Next_State = UI_COMPASS_CALIBRATION;  
			//			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_compass_calibration_screen();
			ui_timer_start(10000);
		}
		break;

	case MATRIX_TEST:
		matrix_test();
		
	
//		w.mat[0][0] = -8700;
//		w.mat[0][1] = -192;
//		w.mat[0][2] = -28079;
//		w.mat[0][3] = 1;
//		w.mat[1][0] = 8103;
//		w.mat[1][1] = -1953;
//		w.mat[1][2] = 30355;
//		w.mat[1][3] = 1;
//		w.mat[2][0] = 27282;
//		w.mat[2][1] = -1000;
//		w.mat[2][2] = -8648;
//		w.mat[2][3] = 1;
//		w.mat[3][0] = -27502;
//		w.mat[3][1] = 1663;
//		w.mat[3][2] = 7415;
//		w.mat[3][3] = 1;
//		w.mat[4][0] = -846;
//		w.mat[4][1] = 27803;
//		w.mat[4][2] = 2635;
//		w.mat[4][3] = 1;
//		w.mat[5][0] = 390;
//		w.mat[5][1] = -27699;
//		w.mat[5][2] = -4107;
//		w.mat[5][3] = 1;
		
		State_of_Screen = UI_CALIBRATION_MENU;
		
		break;

	default:
		printf("\nInvalid screen\n");
		break;

	}

}


/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
