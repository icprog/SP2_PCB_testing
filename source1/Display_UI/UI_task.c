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

TEST_COUNT_FLASH Test_Count_Flash;

unsigned char LCD_Updating_Flag,LSM_Updating_Flag=0;
uint_16 img_date_word=0, img_time_word=0;
MFS_DATE_TIME_PARAM Image_File_Attr;
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


void Get_Raw_Data(void)
{
	if(Read_LSM==1)
	{
		Read_LSM=0;
		Start_LSM();						
	}

	if(Lsm_Data_Ready)
	{
		Get_Lsm_Data();
	}
	//	gps_data_read();
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

	
	Core_clock_selection = ACQUISITION_MODE;

	/*Initializing all sensor status flags to zero*/
	Init_Sensor_status();
	ADC_Init();
	ADC_deinit();
	

	printf("Initialising LCD...\n");
	spi_init(); /*Init SPI configuration*/
	lcd_clear(); /* Clearing LCD */
	buff_clear();

	Settings_Data.battery_threshold = POWERUP_BATTERY_THRESHOLD;
	Battery_checkon_powerup();
	Settings_Data.battery_threshold = DEFAULT_BATTERY_THRESHOLD;
	
	//	Refresh_Lcd_Buffer((uint_8 *) logo_image);

	// Initialising sd card
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
	
	/* Opening settings file from SD card*/
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
	
	Sd_Card_deinit();
	USB_DPin_Low();
	lcd_display_deinit();
	Run_in_idle_mode();

	//	Update_rtc(Time_from_settings,Date_from_settings); 
	Init_HardWare();
	update_ble_firmware();
	Switch_acquisition_mode();
	Perform_Startup_Test(); //this is the whole test of the unit
	Switch_to_idle_mode();
	
	display_Mainmenu();
	while(1)
	{
		update_ui_screen();
	}
}


void Enter_K70_LPM(uint_32 time)
{
	// disable power rail
	power_rail_disable();
	//	lwgpio_set_value(&SD_EN, LWGPIO_VALUE_LOW);

	VLPR_mode();
	ui_timer_start(time);
#if AUTOMATE_TEST
	Button_Press_Status = TEST_BUTTON_PRESSED;
#endif
	while(1)
	{
		if(State_of_Screen == UI_BLUETOOTH_MAIN)
		{
			if(Prev_State_of_Screen == UI_IDLE_CHECK_KILL)
			{
				spi_init(); 
				lcd_clear();
			}
			GPS_Wakeup_Time = 0;
			break;
		}

		if(Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			GPS_Wakeup_Time = 0;
			break;
		}
		if(Check_UI_Timer_Timeout() == TIME_OUT)
		{
			GPS_Wakeup_Time = 0;
			break;
		}
		if(GPS_Current_State == GPS_LOCKED)
		{
			GPS_Wakeup_Time = UI_timer_count;
			break;
		}
		if(Usb_Connected == 1)
		{
			if(((1 << GPIO_PIN17) & GPIOA_PDIR) != 0)
			{
				Usb_Connected = 0;
			}
			else
			{
				GPS_Wakeup_Time = 0;
				break;
			}
		}
	}
}

/*-----------------------------------------------------------------------------* 
 * Function:    update_ui_screen
 * Brief:       This will do appropriate operation on each user feedback
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void update_ui_screen(void)
{
	static uint_32 test_counts=0;

	switch (State_of_Screen)
	{
	case UI_IDLE_CHECK_DISPLAY_OFF:

		if(set_date_time_status == SET_DATE_TIME_FROM_SETTINGS)
		{
#if DYNAMIC_UPDATE_TIME
			if((Next_State == UI_DATE_AND_TIME) || (Next_State == UI_UPDATE_DATE))
			{
				State_of_Screen = Next_State;
				break;
			}
#endif
		}
		printf("\nUI_IDLE_CHECK_DISPLAY_OFF\n");
		Enter_K70_LPM(IDLE_DISPLAY_OFF_TIMEOUT); //30 sec
		if(Check_UI_Timer_Timeout() == TIME_OUT)
		{
			run_mode();
			lcd_display_deinit();
			GPS_Wakeup_Time = 0;
			State_of_Screen = UI_IDLE_CHECK_KILL;
		}
		else if(Usb_Connected == 1)
		{
			Usb_Connected = 0;
			State_of_Screen = UI_MASS_STORAGE_ENABLE;
			run_mode();
			Battery_ADC_Init();
			Create_Header();
			Battery_ADC_Deinit();
			
//			Sd_Card_deinit();
			
			BLE_Advertising(OFF);
			
			/* Switch to core to high frequency */
			Switch_acquisition_mode();
//			Sd_Card_init();
			USB_DPin_High();
			display_mass_storage_enable();
		}
		else if(Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			State_of_Screen = Next_State;
			run_mode();
			Battery_ADC_Init();
			Create_Header();
			Battery_ADC_Deinit();
			//			lwgpio_set_value(&SD_EN, LWGPIO_VALUE_HIGH);
		}
		else if(GPS_Current_State == GPS_LOCKED)
		{
			run_mode();
			resetCC4000API();
			GPS_uart_init();
			printf("GPS Locked found\nTrying to get data from GPS and Update RTC\n");
			Time_Delay_Sleep(2000);
			if(gps_data_read()== 0)
			{
				printf("DATE: %s\n",Data_header.Time_stamp.Date);
				printf("TIME: %s\n",Data_header.Time_stamp.Time);
				printf("LATI: %s\n",Data_header.Gps_data.Latitude);
				printf("LONG: %s\n",Data_header.Gps_data.Longitude);
				Update_rtc((char *)Data_header.Time_stamp.Time,Data_header.Time_stamp.Date);
				if(Settings_Data.Dst_status == DST_ON)
				{
					Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_Value);   
				}
				else
				{
					Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_Value);
				}

				GPS_ON_timer_start(GPS_ON_TIMEOUT);	// 3 min
				GPS_Current_State = GPS_POWER_OFF;
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
				break;
			}
			else
			{
				printf("No GPS data found\n");
				GPS_ON_timer_start(GPS_ON_TIMEOUT);	// 3 min
				GPS_Current_State = GPS_POWER_OFF;
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
			}
		}
		else
		{
			// error
		}
		break;



	case UI_IDLE_CHECK_KILL:
		printf("\nUI_IDLE_CHECK_KILL\n");
		if(GPS_Wakeup_Time)
		{
			Enter_K70_LPM(GPS_Wakeup_Time); //300 sec = 5 Min
		}
		else
		{
			Enter_K70_LPM(IDLE_KILL_TIMEOUT); //300 sec = 5 Min
		}

		if(Check_UI_Timer_Timeout() == TIME_OUT)
		{
			Kill_OFF_Controller();
		}
		else if(Usb_Connected == 1)
		{
			Usb_Connected = 0;
			State_of_Screen = UI_MASS_STORAGE_ENABLE;
			run_mode();
			printf("Initialising LCD...\n");
			spi_init(); /*Init SPI configuration*/
			lcd_clear(); /* Clearing LCD */
			buff_clear();
			Battery_ADC_Init();
			Create_Header();
			Battery_ADC_Deinit();
			
//			Sd_Card_deinit();
			
			BLE_Advertising(OFF);
			
			/* Switch to core to high frequency */
			Switch_acquisition_mode();
//			Sd_Card_init();
			USB_DPin_High();
			display_mass_storage_enable();
		}
		else if(Button_Press_Status != BUTTON_PRESS_NOT_PENDING)//(run_mode_flag==0)
		{
			run_mode();
			printf("Initialising LCD...\n");
			spi_init(); /*Init SPI configuration*/
			lcd_clear(); /* Clearing LCD */
			buff_clear();
			State_of_Screen = Next_State;
			Battery_ADC_Init();
			Create_Header();
			Battery_ADC_Deinit();
			//			lwgpio_set_value(&SD_EN, LWGPIO_VALUE_HIGH);
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if(GPS_Current_State == GPS_LOCKED)
		{
			run_mode();
			resetCC4000API();  
			GPS_uart_init();
			printf("GPS Locked found\nTrying to get data from GPS and Update RTC\n");
			Time_Delay_Sleep(2000);
			if(gps_data_read()== 0)
			{
				printf("DATE: %s\n",Data_header.Time_stamp.Date);
				printf("TIME: %s\n",Data_header.Time_stamp.Time);
				printf("LATI: %s\n",Data_header.Gps_data.Latitude);
				printf("LONG: %s\n",Data_header.Gps_data.Longitude);
				Update_rtc((char *)Data_header.Time_stamp.Time,Data_header.Time_stamp.Date);
				if(Settings_Data.Dst_status == DST_ON)
				{
					Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_Value);   
				}
				else
				{
					Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_Value);
				}

				GPS_ON_timer_start(GPS_ON_TIMEOUT);	// 3 min
				GPS_Current_State = GPS_POWER_OFF;
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
				break;
			}
			else
			{
				printf("No GPS data found\n");
				GPS_ON_timer_start(GPS_ON_TIMEOUT);	// 3 min
				GPS_Current_State = GPS_POWER_OFF;
				lwgpio_int_enable(&FIX_AVL, FALSE );
				gpsCC4000OStopFixes(); 
				GPS_uart_deinit();
			}
		}
		else
		{
			// error
		}
		break;


	case UI_WAKING_UP_FROM_SLEEP:
		printf("UI_WAKING_UP_FROM_SLEEP\n");
		//3.	The logo will disappear after 5 seconds or after another button press
		Next_State = UI_MAINMENU_LIST;
		State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		//Display Snow Profile List
		//					display_Snow_Profiles_List();
		//	                display_Mainmenu();
		//					Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		break;

	case UI_MAINMENU_LIST:
		printf("\nUI_MAINMENU_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
             BUTTON	NEW STATE			SOFTWARE ACTION
             LEFT	--	
             RIGHT	(select menu item)	
             UP		(navigate menu)	
             DOWN	(navigate menu)	
             TEST	Test Menu (default)
			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
//				List_File_Attributes();		//Lists the files in sd card including folders... not subfolders.
				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				State_of_Screen = Mainmenu_Table[Main_menu_selection].UI_state;
				if (State_of_Screen == UI_SETTINGS)
				{
					Settings_menu_selection = 0;
				}
				else if(State_of_Screen == UI_NEW_TEST_LIST)
				{
					Test_Menu_Prev_State = UI_MAINMENU_LIST;
				}
				else if (State_of_Screen == UI_SNOW_PROFILE_LIST)
				{
					Snow_Profiles_List_selection = 0;
					Folder_name_count=0;
					Sd_Card_Folder_List();

				}
				else if(State_of_Screen == UI_FAVOURITES_LIST)
				{
					sort_favourite_list();
					Favourites_List_selection = 0;
					File_name_count=0;
				}

//				if (State_of_Screen == UI_SENSOR_OUTPUT)
//				{
//				
//					Next_State = UI_SENSOR_OUTPUT;
//					//						Start_ADC();
//					power_rail_enable();
//					ADC_Init();
//					lsm303_i2c0_init();
//					Start_LSM();
//					if((GPS_Current_State == GPS_LOCKING)||(GPS_Current_State == GPS_POWER_ON))
//					{
//						lwgpio_int_enable(&FIX_AVL, FALSE );
//					}
//
//					GPS_Prev_State = GPS_Current_State;
//					GPS_Current_State = GPS_STANDBY;
//					resetCC4000API();  
//					gpsCC4000OStartFixes(); 
//					GPS_uart_init();
//
//					//						Gps_on_for_sensor_output();
//					ui_timer_start(500);
//
//				}
				else
				{
					Next_State=State_of_Screen;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				}
				Mainmenu_Table[Main_menu_selection].menu_Function();

				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Mainmenu_Key_up();
				display_Mainmenu();

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Mainmenu_Key_down();
				display_Mainmenu();
				break;

			case TEST_BUTTON_PRESSED:

				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_MAINMENU_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_Mainmenu();
		}
		break;


	case UI_FAVOURITES_LIST:

		/*
		 * BUTTON	NEW STATE				SOFTWARE ACTION
         LEFT	Main Menu	
         RIGHT	Snow Profile Graph	
         UP		Navigate up	
         DOWN	Navigate down
         TEST	Test Menu (default)

		 * 
		 * 
		 */
		printf("\nUI_FAVOURITES_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_MAINMENU_LIST;
				Main_menu_selection = 0;
				Favourites_List_selection = 0;
				display_Mainmenu();
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				State_of_Screen=UI_FAVOURITES_LIST;
				if(Favourites_List_count==0)
				{
					break;
				}
				details_previous_state = LAST_STATE_IS_FAVOURITE;
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;


				break;

			case UP_BUTTON_PRESSED:
				Next_State = UI_FAVOURITES_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				printf("Up Button Pressed \n"); 
				Favourites_List_Key_up();
				display_favourites_menu();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_FAVOURITES_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Favourites_List_Key_down();
				display_favourites_menu();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_FAVOURITES_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_FAVOURITES_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			Mainmenu_Table[Main_menu_selection].menu_Function();
		}
		break;

	case UI_NEW_TEST_LIST:
		printf("\nUI_NEW_TEST_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
             BUTTON  NEW STATE 			SOFTWARE ACTION
             LEFT   	Snow Profiles List 
             RIGHT  (select menu item)  
             UP    (navigate menu)  
             DOWN  (navigate menu)  
             TEST  (select menu item)  


			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
//				if(test_menu_previous_state == TEST_MENU_BACK_BUTTON_MAIN_MENU)
//				{
//					Next_State = UI_MAINMENU_LIST;
//					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//					Main_menu_selection = 0;
//					display_Mainmenu();
//				}
//				else if(test_menu_previous_state == TEST_MENU_BACK_BUTTON_DETAILS_LIST)
//				{
//					Next_State = UI_SNOW_PROFILE_DETAILS;
//					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//					display_Snow_Profile_Details();
//					test_menu_previous_state = TEST_MENU_BACK_BUTTON_MAIN_MENU;
//				}
				
//				Next_State = Test_Menu_Prev_State;
				State_of_Screen = Test_Menu_Prev_State;
				
				break;

			case TEST_BUTTON_PRESSED:

				printf("Test Button Pressed \n");
				Test_disp_ctr = 0;
				/* Need to modify to  */                      
				//                    Refresh_Lcd_Buffer((uint_8 *) logo_image);
				State_of_Screen = UI_NEW_SNOW_PROFILE_CALIBRATING;

				display_New_Snow_profile_calibrating();

				//Create New Folder and File Names
				++Test_Count_Flash.Total_Test_Ct;
				Write_Flash(Test_Count_Flash.Total_Test_Ctr_Buff, 4);
				printf("\n TOTAL TEST CTR : %u", Test_Count_Flash.Total_Test_Ct);

				Get_timestamp_from_rtc();
				sprintf(Test_file_directory.test_folder_name,Data_header.Time_stamp.Date, NULL);
				sprintf(Test_file_directory.test_file_name,"%08u", Test_Count_Flash.Total_Test_Ct);                                
				printf("FOLDER NAME is: %s",Test_file_directory.test_folder_name); 
				printf("\n FILE NAME is: %s",Test_file_directory.test_file_name); 

				printf("\nTest Number = %u\n",Test_Count_Flash.Total_Test_Ct);

				GPS_Prev_State = GPS_Current_State;
				GPS_Current_State = GPS_STANDBY;
				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
					lwgpio_int_enable(&FIX_AVL, FALSE );
#if GPS_OFF_DURING_TEST
					gpsCC4000OStopFixes(); 
					//Turn gps off code
					gps_power_off();
#endif
				}
				BLE_Advertising(OFF);
				/* Switch to core to high frequency */
				Switch_acquisition_mode();
				//	/* Init HW timer for acquisition */
				Init_Hw_Timer();

				power_rail_enable();
				ddr_init();
				ADC_Init();
				//					lsm303_i2c0_init();
				Lsm303_Test_Init();
				//					Start_LSM();
				Tmp006_Init();
				//                      display_New_Snow_profile_calibrating();
				Test_disp_ctr = 0;

				battery_Voltage_Check();

				//Switch_to_idle_mode();
				display_New_Snow_profile_calibrating();
				//		            Refresh_Lcd_Buffer((uint_8 *) logo_image);

				printf("Start Data Acquisition \n%f", Settings_Data.Calibration_Time);

				/* Clear DDR buffers */
				Init_acq_buff();
#if ALGO_TEST
				Take_SD_Raw();
#endif
				start_data_acquision();
				//					Start_LSM();
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");

				//                        State_of_Screen = Testmenu_Table[Test_menu_selection].UI_state;

				State_of_Screen = UI_SLOP_MEASURMENTS;
				lsm303_i2c0_init();
				Start_LSM();
				ui_timer_start(500);
				display_Slope_Measurment();
				//Testmenu_Table[Test_menu_selection].menu_Function();
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//                        Testmenu_Key_up();
				//                        display_Testmenu();

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//                        Testmenu_Key_down();
				//                        display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_NEW_TEST_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_Testmenu();
			//			Mainmenu_Table[Main_menu_selection].menu_Function();
		}
		break;



	case UI_SNOW_PROFILE_LIST:
		printf("\nUI_SNOW_PROFILE_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
			 * BUTTON	NEW STATE			SOFTWARE ACTION
			 * LEFT		Main Menu	
			 * RIGHT	Snow Profile Details Load selected snow profile file
			 * UP		(navigate list)	
			 * DOWN		(navigate list)	
			 * TEST		 Test Menu (default)
			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//        			Sd_Card_deinit();
				Main_menu_selection = 0;
				Snow_Profiles_List_selection = 0;
				display_Mainmenu();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				if(Sd_card_folder_count >1)
				{
					Next_State = UI_SNOW_PROFILE_FILE_LIST;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					File_name_index = 0;
					Snow_Profiles_File_List_selection = 0;
					File_name_count = 0;
					Sd_Card_File_List(Folder_name_index);
					display_Snow_Profiles_File_List();
				}
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Snow_Profiles_List_Key_up();
				display_Snow_Profiles_List();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Snow_Profiles_List_Key_down();
				display_Snow_Profiles_List();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_SNOW_PROFILE_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			//Display Snow Profile List
			display_Snow_Profiles_List();
		}

		break;

	case UI_SNOW_PROFILE_FILE_LIST:
		printf("\nUI_SNOW_PROFILE_FILE_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
			 * BUTTON	NEW STATE			SOFTWARE ACTION
			 * LEFT		Main Menu	
			 * RIGHT	Snow Profile Details Load selected snow profile file
			 * UP		(navigate list)	
			 * DOWN		(navigate list)	
			 * TEST		 Test Menu (default)
			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//                    Snow_Profiles_List_selection=0;
				display_Snow_Profiles_List();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				if(Sd_card_file_count > 0)
				{
					State_of_Screen = UI_SNOW_PROFILE_DETAILS;
					display_Snow_Profile_Details();
					Next_State = UI_SNOW_PROFILE_DETAILS;
				}
				else
				{
					Next_State = UI_SNOW_PROFILE_FILE_LIST;
				}
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_FILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Snow_Profiles_File_List_Key_up();
				display_Snow_Profiles_File_List();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_FILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Snow_Profiles_File_List_Key_down();
				display_Snow_Profiles_File_List();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_FILE_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_SNOW_PROFILE_FILE_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_Snow_Profiles_File_List();
		}
		break;



	case UI_NEW_MULTIPLE_TEST_LIST:
		printf("\nUI_NEW_MULTIPLE_TEST_LIST\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
             BUTTON  NEW STATE 			SOFTWARE ACTION
             LEFT   	New multiple Test List
             RIGHT  (select menu item)  
             UP    (navigate menu)  
             DOWN  (navigate menu)  
             TEST  (select menu item)  


			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Main_menu_selection = 0;
				display_Mainmenu();

				break;                    

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");

				if(test_number == TEST_NUMBER_ONE)
				{
					State_of_Screen = UI_SLOP_MEASURMENTS;
					lsm303_i2c0_init();
					Start_LSM();
					ui_timer_start(500);
					display_Slope_Measurment();
				}
				else
				{
					Next_State = UI_NEW_MULTIPLE_TEST_LIST;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				}
				break;

			case TEST_BUTTON_PRESSED:

				Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
				printf("Test Button Pressed \n");

				display_MultipleTestMenu();
				Next_State = UI_NEW_MULTIPLE_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;


				//                        State_of_Screen = UI_NEW_SNOW_PROFILE_CALIBRATING;
				//                        display_New_Snow_profile_calibrating();
				//                        printf("Start Data Acquision \n%f", Settings_Data.Calibration_Time);
				//                        start_data_acquision();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Next_State = UI_NEW_MULTIPLE_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_NEW_MULTIPLE_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_NEW_MULTIPLE_TEST_LIST;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_MultipleTestMenu();
		}
		break;

	case UI_SNOW_PROFILE_DETAILS:
		printf("\nUI_SNOW_PROFILE_DETAILS\n");
		/*
		 * BUTTON	NEW STATE			SOFTWARE ACTION
         LEFT    Snow Profiles List
         RIGHT	Snow Profile Options
         UP		Load previous snow profile 
         DOWN	Load next snow profile
         TEST	Test Menu (default)

		 * 
		 * */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				if(details_previous_state == LAST_STATE_IS_FILE_LIST)
				{
					Next_State = UI_SNOW_PROFILE_FILE_LIST;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					//Display Snow Profile List
					display_Snow_Profiles_File_List();
					details_previous_state = LAST_STATE_IS_FILE_LIST;
				}
				else
				{
					Next_State = UI_FAVOURITES_LIST;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					sort_favourite_list();
					Favourites_List_selection = 0;
					File_name_count=0;
					display_favourites_menu();
					details_previous_state = LAST_STATE_IS_FILE_LIST;
				}
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Snow_profile_options_selection = SNOWPROFILEOPTIONS_DISPLACEMENT +4; //Select Delete as default
				display_Snow_Profiles_Options();
				Next_State = UI_SNOW_PROFILE_OPTIONS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Snow_Profiles_File_List_Key_up();
				Create_Snow_Profiles_File_List_Content();
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Snow_Profiles_File_List_Key_down();
				Create_Snow_Profiles_File_List_Content();
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_DETAILS;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			State_of_Screen = UI_SNOW_PROFILE_DETAILS;
			display_Snow_Profile_Details();
			Next_State = UI_SNOW_PROFILE_DETAILS;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		}
		break;

	case UI_SNOW_PROFILE_OPTIONS:
		printf("\nUI_SNOW_PROFILE_OPTIONS\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 * BUTTON        NEW STATE                                SOFTWARE ACTION
               LEFT        (Snow Profile Details
               RIGHT        (select menu item)        
               UP                (navigate menu)        
               DOWN        (navigate menu)        
               TEST        Test Menu (default)

			 * 
			 * 
			 */

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_DETAILS;
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				State_of_Screen= Snowprofileoptions_Table[Snow_profile_options_selection-SNOWPROFILEOPTIONS_DISPLACEMENT].UI_state;

				if(State_of_Screen == UI_SAVE_SLOPE)
				{

					lsm303_i2c0_init();
					Start_LSM();
					ui_timer_start(500);

				}  
				else if(State_of_Screen==UI_VIEW_CHARTS)
				{
					Settings_Data.default_gragh_view = (Settings_Data.default_gragh_view + 1)% 5;
					//					Sd_Card_File_List(Folder_name_index);
					State_of_Screen = UI_SNOW_PROFILE_DETAILS;
					display_Snow_Profile_Details();
					Next_State = UI_SNOW_PROFILE_DETAILS;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					//					Next_State = UI_VIEW_CHARTS;
					//					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					break;
				}
				else 
				{
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				}
				Snowprofileoptions_Table[Snow_profile_options_selection-SNOWPROFILEOPTIONS_DISPLACEMENT].menu_Function();
				Next_State = Snowprofileoptions_Table[Snow_profile_options_selection-SNOWPROFILEOPTIONS_DISPLACEMENT].UI_state;
				if(Next_State == UI_SET_FAVOURITE)
				{
					State_of_Screen = UI_SET_FAVOURITE;
				}
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Snow_Profiles_Options_Key_up();
				display_Snow_Profiles_Options();
				Next_State = UI_SNOW_PROFILE_OPTIONS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Snow_Profiles_Options_Key_down();
				display_Snow_Profiles_Options();
				Next_State = UI_SNOW_PROFILE_OPTIONS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				test_menu_previous_state = TEST_MENU_BACK_BUTTON_DETAILS_LIST;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_DETAILS;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
			display_Snow_Profiles_Options();
			Next_State = UI_SNOW_PROFILE_OPTIONS;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

		}
		break;

	case UI_SET_FAVOURITE:
		printf("UI_SET_FAVOURITE\n");
		display_Snow_Profile_Details();
		Link_Favourite_With_Test();
		Next_State = UI_SNOW_PROFILE_DETAILS;
		State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		break;

	case UI_SAVE_SLOPE:
		//    	printf("\nUI_SAVE_SLOPE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
              BUTTON	NEW STATE				SOFTWARE ACTION
              LEFT	Snow Profiles List		delete snow profile
              RIGHT	Snow Profile Options

              UP		--	
              DOWN	--	
              TEST	Test Menu (default)


			 * 
			 */

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Stop_LSM();
				Lsm303_deinit();

				Snow_profile_options_selection = SNOWPROFILEOPTIONS_DISPLACEMENT;
				display_Snow_Profiles_Options();
				Next_State= UI_SNOW_PROFILE_OPTIONS;     
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				//Link slope to profile code here

				Next_State= UI_SNOW_PROFILE_DETAILS;     
				State_of_Screen =UI_SNOW_PROFILE_DETAILS;

				if (Link_Snow_Profile_With_Test(Slope_Measurment_Result_Slope_Angle,
						Slope_Measurment_Result_Aspect) != 0)
				{
					Stop_LSM(); 
					Lsm303_deinit();
					display_record_not_found();
					return;
				}

				Stop_LSM(); 
				Lsm303_deinit();

				//                     Snow_profile_options_selection = SNOWPROFILEOPTIONS_DISPLACEMENT;
				display_Snow_Profile_Details();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done
				Next_State= UI_SAVE_SLOPE;     
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				Next_State= UI_SAVE_SLOPE;     
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");

				Stop_LSM(); 
				Lsm303_deinit();
				Next_State= UI_NEW_TEST_LIST;     
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				test_menu_previous_state = TEST_MENU_BACK_BUTTON_DETAILS_LIST;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_DETAILS;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if (Check_UI_Timer_Timeout() == TIME_OUT)        
		{
			display_save_slope();
			ui_timer_start(500);

		}
		break;

	case UI_GPS_LOCATION:
		printf("\nUI_GPS_LOCATION\n");
		Next_State = UI_SNOW_PROFILE_OPTIONS;  
		State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;                  		   		
		break;

	case UI_VIEW_CHARTS:
		printf("UI_VIEW_CHARTS\n");

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_FILE_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Display Snow Profile List
				display_Snow_Profiles_File_List();

				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				Sd_Card_File_List(Folder_name_index);
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Snow_profile_options_selection = SNOWPROFILEOPTIONS_DISPLACEMENT;
				display_Snow_Profiles_Options();
				Next_State = UI_SNOW_PROFILE_OPTIONS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Snow_Profiles_File_List_Key_up();
				Create_Snow_Profiles_File_List_Content();
				State_of_Screen = UI_SNOW_PROFILE_DETAILS;
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				//Nothing to be done
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Snow_Profiles_File_List_Key_down();
				Create_Snow_Profiles_File_List_Content();
				State_of_Screen = UI_SNOW_PROFILE_DETAILS;
				display_Snow_Profile_Details();
				Next_State = UI_SNOW_PROFILE_DETAILS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_DETAILS;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_VIEW_CHARTS;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_Snow_Profile_Details();
		}
		break;

	case UI_DELETE_PROFILE: 
		printf("UI_DELETE_PROFILE\n");
		Next_State = UI_SNOW_PROFILE_DELETE;  
		State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		display_Snow_Profiles_Delete();
		break;


	case UI_SNOW_PROFILE_DELETE:
		printf("UI_SNOW_PROFILE_DELETE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
             BUTTON	NEW STATE				SOFTWARE ACTION
             LEFT	Snow Profiles List		delete snow profile
             RIGHT	Snow Profile Options
             UP		--	
             DOWN	--	
             TEST	Test Menu (default)
			 * 
			 */

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//DELETE delete snow profile
				delete_snow_profile_selected_from_favourite();
				Delete_File(Folder_name_index, File_name_index);
				Update_Fav_List_File_Index_After_File_Deletion();
				Create_ble_manifest_after_startup_and_delete();
				Sd_Card_File_List(Folder_name_index);
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				State_of_Screen = UI_SNOW_PROFILE_OPTIONS;
				Snow_profile_options_selection = SNOWPROFILEOPTIONS_DISPLACEMENT;
				display_Snow_Profiles_Options();
				Next_State = UI_SNOW_PROFILE_OPTIONS;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_DELETE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_SNOW_PROFILE_DELETE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				test_menu_previous_state = TEST_MENU_BACK_BUTTON_DETAILS_LIST;
				Test_Menu_Prev_State = UI_SNOW_PROFILE_DETAILS;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_SNOW_PROFILE_DELETE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		}
		break;

	case UI_NEW_SNOW_PROFILE:
		printf("\nUI_NEW_SNOW_PROFILE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
             BUTTON	NEW STATE	SOFTWARE ACTION
             LEFT	Test Menu
             RIGHT	--	
             UP		--	
             DOWN	--	
             TEST	New Snow Profile start collecting data
			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_NEW_SNOW_PROFILE;
				display_Testmenu();

				break;

			case RIGHT_BUTTON_PRESSED:
				Next_State = UI_NEW_SNOW_PROFILE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				printf("Right Button Pressed \n");
				//Nothing to be done
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done
				Next_State = UI_NEW_SNOW_PROFILE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_NEW_SNOW_PROFILE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:

				printf("Test Button Pressed \n");
				State_of_Screen = UI_NEW_SNOW_PROFILE_CALIBRATING;
				Test_disp_ctr = 0;
				/* Need to modify to  */                       
				display_New_Snow_profile_calibrating();
				//                    Refresh_Lcd_Buffer((uint_8 *) logo_image);

				GPS_Prev_State = GPS_Current_State;
				GPS_Current_State = GPS_STANDBY;

				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
					lwgpio_int_enable(&FIX_AVL, FALSE );
#if GPS_OFF_DURING_TEST
					gpsCC4000OStopFixes(); 
					//Turn gps off code
					gps_power_off();
#endif
				}

				BLE_Advertising(OFF);
				/* Switch to core to high frequency */
				Switch_acquisition_mode();
				//	/* Init HW timer for acquisition */
				Init_Hw_Timer();

				power_rail_enable();
				ddr_init();
				ADC_Init();
				//					lsm303_i2c0_init();
				Lsm303_Test_Init();
				//					Start_LSM();
				Tmp006_Init();
				// Temp Init
				//		            tempCount = 0;
				//Switch_to_idle_mode();
				//                    printf("Updating\n");
				/* Need to modify to  */                       
				display_New_Snow_profile_calibrating();
				//		            Refresh_Lcd_Buffer((uint_8 *) logo_image);
				//                    printf("Updated\n");
				printf("Start Data Acquision \n%f", Settings_Data.Calibration_Time);

				/* Clear DDR buffers */
				Init_acq_buff();
#if ALGO_TEST
				Take_SD_Raw();
#endif
				
				start_data_acquision();
				//					Start_LSM();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_NEW_SNOW_PROFILE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		}
		break;

	case UI_SLOP_MEASURMENTS:
		//    	printf("\nUI_SLOP_MEASURMENTS\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *
             BUTTON		NEW STATE		SOFTWARE ACTION
             LEFT		Test Menu
             RIGHT		--	
             UP			--	
             DOWN		--	
             TEST		Test menu
             Additional Notes:
             Populate slope angle and aspect labels with sensor data
			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Lsm303_deinit();
				Stop_LSM();
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				Test_Menu_Prev_State = UI_SLOP_MEASURMENTS;
				display_Testmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				//Nothing to be done
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:

				printf("Test Button Pressed \n");
				Lsm303_deinit();
				Stop_LSM();
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				Test_Menu_Prev_State = UI_SLOP_MEASURMENTS;
				display_Testmenu();
				//                        printf("Test Button Pressed \n");
				//                        Stop_LSM();
				//                        ui_timer_stop();
				//                        State_of_Screen = UI_LINK_SLOP_WITH_PROFILE;
				//                        display_Link_Slope_with_Profile();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		//for Populating slope angle and aspect labels with sensor data
		else if (Check_UI_Timer_Timeout() == TIME_OUT)
		{
			display_Slope_Measurment();
			ui_timer_start(500);
		}
		break;

	case UI_NEW_SNOW_PROFILE_CALIBRATING:
		printf("\nUI_NEW_SNOW_PROFILE_CALIBRATING\n");
		/*
		 * 
         BUTTON	NEW STATE			SOFTWARE ACTION
         LEFT	Test Menu			stop collecting data
         RIGHT	New Snow Profile 3	stop collecting / start processing
         UP		--	
         DOWN	--	
         TEST	(same as RIGHT button)	(same as RIGHT button)
         Additional Notes:
         After 1 second timeout, Start collecting (same as RIGHT button)
		 * */

		//Get Raw Data from sensors
		if (Check_UI_Timer_Timeout() == TIME_OUT)
		{
			Buzzer_Short_Beep(0);
			State_of_Screen = UI_NEW_SNOW_PROFILE_COLLECTING;                  
			display_New_Snow_profile_collecting();
#if AUTOMATE_TEST
			ui_timer_start(200);
#else
			ui_timer_start(1000);
#endif
		}
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				stop_data_acquision();
				Tmp006_deinit();
				De_Init_Hw_Timer();
				ADC_deinit();
				Stop_LSM();
				Lsm303_deinit();
				Switch_to_idle_mode();
				BLE_Advertising(ON);

				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
#if GPS_OFF_DURING_TEST
					GPS_OFF_timer_start(180000);	// 3 min
					gps_power_on();
					gpsCC4000OStartFixes(); 
#endif
					GPS_Current_State = GPS_POWER_ON;
					/* enable interrupt on GPIO peripheral module*/
					lwgpio_int_enable(&FIX_AVL, TRUE );
					GPS_Lock_timer_start(5000);	// 5 sec
				}
				else
				{
					GPS_Current_State = GPS_Prev_State;
				}

				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();

				break;

			case RIGHT_BUTTON_PRESSED:

				//		printf("Right Button Pressed \nNot implemented this functionality now.\n");
				//		collecting_data_count=0;
				//		ui_timer_stop();
				//		ui_timer_start(5000);//5sec need to change after 
				//		State_of_Screen = UI_NEW_SNOW_PROFILE_PROCESSING;
				//		display_New_Snow_profile_processing();

				break;

			case UP_BUTTON_PRESSED:
				//	printf("Up Button Pressed \n");
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				//		printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				//	printf("Test Button Pressed \nNot implemented this functionality now.\n");
				//	ui_timer_stop();
				//	collecting_data_count=0;
				//	State_of_Screen = UI_NEW_TEST_LIST;
				//	display_Testmenu();

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		break;

	case UI_NEW_SNOW_PROFILE_COLLECTING:
		//    	printf("\nUI_NEW_SNOW_PROFILE_COLLECTING\n");
		/*
		 * 
         BUTTON	NEW STATE			SOFTWARE ACTION
         LEFT	Test Menu			stop collecting data
         RIGHT	New Snow Profile 3	top collecting / start processing
         UP		--	
         DOWN	--	
         TEST	(same as RIGHT button)	(same as RIGHT button)
         Additional Notes:
         After 10 second timeout, end test (same as RIGHT button)
		 * */

		//Get Raw Data from sensors
		//        Get_Raw_Data();
		if (Test_Complete_flag == 1)
		{
			beep_ctr = 0;
			Test_Complete_flag = 0;
			Tmp006_Read(1);	       //Reading temperature after test
			Tmp006_deinit();
			De_Init_Hw_Timer();
			ADC_deinit();
			Stop_LSM();
			Lsm303_deinit();
			//            Switch_to_idle_mode();
			printf("\n Stop Collecting Data @ ");
			print_rtc_time_date();

			printf("\nSD Card Written %d times\n", SD_Write_Cnt);
			SD_Write_Cnt = 0;

			print_collected_data_buffer();

//			Test_disp_ctr = 0;

			display_New_Snow_profile_processing();
			/* Switch to core to high frequency */

			//            Switch_acquisition_mode();
			Processing_Display=1;
#if AUTOMATE_TEST
			Processing_Display_timer_start(200);
#else
			Processing_Display_timer_start(1000); 
#endif
			//			Buzzer_Long_Beep();
			//			Time_Delay_Sleep(3000);
			//            Buzzer_Off();
			Snow_Profiles_List_count = Data_File_Count;

			type_of_error = Process_Data();
			if (type_of_error != NO_ERROR_STATE)
			{
				State_of_Screen = UI_DISPLAY_ERROR_SCREENS;
				Processing_Display=0;
				while(LCD_Updating_Flag==1)
				{

				}
				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
#if GPS_OFF_DURING_TEST
					GPS_OFF_timer_start(180000);	// 3 min
					gps_power_on();
					gpsCC4000OStartFixes(); 
#endif
					GPS_Current_State = GPS_POWER_ON;
					/* enable interrupt on GPIO peripheral module*/
					lwgpio_int_enable(&FIX_AVL, TRUE );
					GPS_Lock_timer_start(5000);	// 5 sec
				}
				else
				{
					GPS_Current_State = GPS_Prev_State;
				}
				printf("\n display_test_failure @ ");
				print_rtc_time_date();
				Buzzer_Off();
				Switch_to_idle_mode();
				BLE_Advertising(ON);
				Next_State = UI_DISPLAY_ERROR_SCREENS;

				State_of_Screen = UI_DISPLAY_ERROR_SCREENS;  
				display_error_screen_during_test();
				break;
			}
			else
			{
				Processing_Display=0;
				while(LCD_Updating_Flag==1)
				{

				}

				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
#if GPS_OFF_DURING_TEST
					GPS_OFF_timer_start(180000);	// 3 min
					gps_power_on();
					gpsCC4000OStartFixes(); 
#endif
					GPS_Current_State = GPS_POWER_ON;
					/* enable interrupt on GPIO peripheral module*/
					lwgpio_int_enable(&FIX_AVL, TRUE );
					GPS_Lock_timer_start(5000);	// 5 sec
				}
				else
				{
					GPS_Current_State = GPS_Prev_State;
				}
				printf("\n Graph Ploted @ ");
				print_rtc_time_date();
				//            Buzzer_Off();
				Switch_to_idle_mode();
				BLE_Advertising(ON);

#if AUTOMATE_TEST
				Next_State = UI_MAINMENU_LIST;
#else
				Next_State = UI_SNOW_PROFILE_DETAILS;
#endif
				State_of_Screen = UI_SNOW_PROFILE_DETAILS;  
				display_New_Snow_profile_complete();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Buzzer_Short_Beep(0);
				Time_Delay_Sleep(1000);
#if AUTOMATE_TEST
				display_Snow_Profile_Details();
#endif
			}


			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			break;
		}
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:				

				stop_data_acquision();
				Tmp006_deinit();
				De_Init_Hw_Timer();
				ADC_deinit();
				Stop_LSM();
				Lsm303_deinit();
				Switch_to_idle_mode();
				BLE_Advertising(ON);
				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
#if GPS_OFF_DURING_TEST
					GPS_OFF_timer_start(180000);	// 3 min
					gps_power_on();
					gpsCC4000OStartFixes(); 
#endif
					GPS_Current_State = GPS_POWER_ON;
					/* enable interrupt on GPIO peripheral module*/
					lwgpio_int_enable(&FIX_AVL, TRUE );
					GPS_Lock_timer_start(5000);	// 5 sec
				}
				else
				{
					GPS_Current_State = GPS_Prev_State;
				}
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();
				break;

			case RIGHT_BUTTON_PRESSED:
				//	printf("Up Button Pressed \n");
				//Nothing to be done
				break;

			case UP_BUTTON_PRESSED:
				//	printf("Up Button Pressed \n");
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				//		printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				//	printf("Test Button Pressed \nNot implemented this functionality now.\n");
				//	ui_timer_stop();
				//	collecting_data_count=0;
				//	State_of_Screen = UI_NEW_TEST_LIST;
				//	display_Testmenu();

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if (Check_UI_Timer_Timeout() == TIME_OUT)
		{             
			display_New_Snow_profile_collecting();
#if AUTOMATE_TEST
			ui_timer_start(200);
#else
			ui_timer_start(1000); 
#endif
			beep_ctr++;
			if(beep_ctr==3)
			{
				Buzzer_Short_Beep(1);
			}
			//            if(beep_ctr==4)
			//            {
			//            	Buzzer_Short_Beep(2);
			//            }
		}
		break;

	case UI_DISPLAY_ERROR_SCREENS:
		printf("\nUI_ERROR_SCREENS\n");
		/*
         BUTTON	NEW STATE	SOFTWARE ACTION
         LEFT	--	
         RIGHT	--	
         UP	--	
         DOWN	--	
         TEST	--	
         Additional Notes:
         Process data. After processing, load: (5-4) New Snow Profile 4
		 */

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				display_Mainmenu();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				State_of_Screen = UI_MAINMENU_LIST;
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Next_State = UI_NEW_TEST_LIST;
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Next_State = UI_NEW_TEST_LIST;
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else 
		{
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			Next_State = UI_DISPLAY_ERROR_SCREENS;
			display_error_screen_during_test();
		}
		break;


	case UI_NEW_SNOW_PROFILE_PROCESSING:
		printf("\nUI_NEW_SNOW_PROFILE_PROCESSING\n");
		/*
         BUTTON	NEW STATE	SOFTWARE ACTION
         LEFT	--	
         RIGHT	--	
         UP	--	
         DOWN	--	
         TEST	--	
         Additional Notes:
         Process data. After processing, load: (5-4) New Snow Profile 4
		 */

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//Nothing to be done

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				//Nothing to be done

				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				//Nothing to be done

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}

		else if (Check_UI_Timer_Timeout() == TIME_OUT)
		{

			State_of_Screen = UI_NEW_SNOW_PROFILE_COMPLETE;
			display_New_Snow_profile_complete();

		}
		break;

	case UI_NEW_SNOW_PROFILE_COMPLETE:
		printf("\nUI_NEW_SNOW_PROFILE_COMPLETE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*

             BUTTON	NEW STATE			SOFTWARE ACTION
             LEFT	New Snow Profile 
             RIGHT	Snow Profile Details	save new snow profile
             UP		--	
             DOWN	--	
             TEST	Test Menu (default)
			 * 
			 */

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//            		Next_State = UI_NEW_SNOW_PROFILE_DISCARD;  
				Next_State = UI_SNOW_PROFILE_DETAILS;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_Snow_Profile_Details();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				//save new snow profile
				Snow_Profiles_List_selection = Data_File_Count - 1;
				Next_State = UI_SNOW_PROFILE_DETAILS;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_Snow_Profile_Details();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_NEW_SNOW_PROFILE_COMPLETE;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_NEW_SNOW_PROFILE_COMPLETE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_New_Snow_profile_complete();
		}
		break;

	case UI_NEW_SNOW_PROFILE_DISCARD:
		printf("\nUI_NEW_SNOW_PROFILE_DISCARD\n");
		/*
         BUTTON	NEW STATE	SOFTWARE ACTION
         LEFT	(5-1) New Snow Profile 1
         discard new test data
         RIGHT	(5-4) New Snow Profile 4

         UP	--	
         DOWN	--	
         TEST	(4) Test Menu (default)

		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//discard new test data
				//    Discard_New_Test_Data();

				Next_State = UI_NEW_SNOW_PROFILE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_New_Snow_profile();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");

				Next_State = UI_NEW_SNOW_PROFILE_COMPLETE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_New_Snow_profile_complete();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Next_State = UI_NEW_SNOW_PROFILE_DISCARD;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_NEW_SNOW_PROFILE_DISCARD;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_NEW_SNOW_PROFILE_DISCARD;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_NEW_SNOW_PROFILE_DISCARD;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			display_New_Snow_profile_discard();
		}
		break;

	case UI_LINK_SLOP_WITH_PROFILE:
		printf("\nUI_LINK_SLOP_WITH_PROFILE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
             BUTTON	NEW STATE				SOFTWARE ACTION
             LEFT	(6-1) Slope Measurement

             RIGHT	(3-1) Snow Profile Details	link slope to selected profile
             UP		(navigate menu)	
             DOWN	(navigate menu)	
             TEST	(4) Test Menu (default)

			 */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_SLOP_MEASURMENTS;
				display_Slope_Measurment();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				//link slope to selected profile

				State_of_Screen = UI_SNOW_PROFILE_DETAILS;

				if (Link_Snow_Profile_With_Test(Slope_Measurment_Result_Slope_Angle,
						Slope_Measurment_Result_Aspect) != 0)
				{
					Stop_LSM(); 
					Lsm303_deinit();
					display_record_not_found();
					return;
				}

				display_Snow_Profile_Details();
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Snow_Profiles_List_Key_up();
				display_Link_Slope_with_Profile();

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Snow_Profiles_List_Key_down();
				display_Link_Slope_with_Profile();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				State_of_Screen = UI_NEW_TEST_LIST;
				Test_Menu_Prev_State = UI_LINK_SLOP_WITH_PROFILE;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	case UI_SETTINGS:
		/*
         BUTTON		NEW STATE					SOFTWARE ACTION
         LEFT	Main Menu	
         RIGHT	Select next input	
         UP		Increment selected input	
         DOWN	Decrement selected input	
         TEST	(4) Test Menu (default)


		 */
		printf("\nUI_SETTINGS\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//				Next_State = UI_SETTINGS_SELECTION;  
				Next_State = UI_MAINMENU_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Main_menu_selection = 0;
				display_Mainmenu();
				//				Settings_selection_menu_selection = 0;
				//				Settings_menu_selection = 0;
				//				display_Settings_Selection();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("RIght Button Pressed \n");

				
				State_of_Screen = Settingsmenu_Table[Settings_menu_selection].UI_state;
				if(State_of_Screen == UI_DATE_AND_TIME)
				{
					Date_Time_menu_selection = 0;
					if(set_date_time_status != SET_DATE_TIME_WHILE_BOOTING)
					{
						update_system_date_time_from_rtc();
					}
				}
				Settingsmenu_Table[Settings_menu_selection].menu_Function();
				Next_State = State_of_Screen;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_SETTINGS;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Settingsmenu_Key_up();       
				display_Settings_Main();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_SETTINGS;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Settingsmenu_Key_down();       
				display_Settings_Main();

				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				//                        Write_Data_file_information_To_Sdcard(); //Store settings to sd card
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SETTINGS;
				display_Testmenu();

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_SETTINGS;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_Settings_Main();
		}
		break;

	case UI_CONFIG_SETTINGS:
		printf("\nUI_CONFIG_SETTINGS\n");
		/*

         Additional Notes:
         Sampling rate: min = 1; max = 10; step = 1; default = 5;
         Opt trig thr: min = 0; max = 3; step = 1; default = 1.5;
         Test time: min = 1; max = 10; step = 1; default = 7;
         Force sen cal: min = 0; max = .350; step = .001; default = 0.195;
         Cal time: min = 0.1; max = 1; step = 0.1; default = 1.0;
         Compass samples: min = 1; max = 10; step = 1; default = 3;

		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//On first input: (1) Main Menu Otherwise: select previous input	
				if (Config_settings_selection == SELECTION_SAMPLING_RATE)
				{
					Next_State = UI_SETTINGS_SELECTION;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					Settings_selection_menu_selection=0;
					Write_Data_file_information_To_Sdcard(); //Store settings to sd card
					display_Settings_Selection();
				}
				else
				{
					Config_Settings_navigate_up();
					display_Config_Settings();
				}

				break;

			case RIGHT_BUTTON_PRESSED:
				printf("RIght Button Pressed \n");
				Config_Settings_navigate_down();
				display_Config_Settings();
				/*
                     if(Check_User_Authentication()==TRUE)
                     {
                     State_of_Screen = UI_SETTINGS;
                     display_Settings_Main();
                     }
                     else
                     {
                     Password_navigate_right();
                     display_Settings_Password();
                     }*/
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				switch (Config_settings_selection)
				{
				case SELECTION_SAMPLING_RATE:
					increment_Sample_Rate();
					break;
				case SELECTION_OPTICAL_TRIGGER_THRESHOLD:
					increment_Optical_Trigger_Threshold();
					break;
				case SELECTION_TEST_TIME:
					increment_Test_Time();
					break;
				case SELECTION_FORCE_SENSOR_CALIBERATION:
					increment_Force_Sensor_Calibration();
					break;
				case SELECTION_CALIBERATION_TIME:
					increment_Calibration_Time();
					break;
				case SELECTION_COMPASS_SAMPLES:
					increment_Compass_Samples();
					break;
				case SELECTION_QRDPERCENT_LOW:
					increment_QRDpercentlow();
					break;
				case SELECTION_QRDPERCENT_HIGH:
					increment_QRDpercentHigh();
					break;
				case SELECTION_BATTERY_THRESHOLD:
					increment_battery_threshold();
					break;
				case SELECTION_XFRACTION_VALUE:
					increment_xfraction_value();
					break;

				}
				display_Config_Settings();
				break;

				case DOWN_BUTTON_PRESSED:

					printf("Down Button Pressed \n");
					switch (Config_settings_selection)
					{
					case SELECTION_SAMPLING_RATE:
						decrement_Sample_Rate();
						break;
					case SELECTION_OPTICAL_TRIGGER_THRESHOLD:
						decrement_Optical_Trigger_Threshold();
						break;
					case SELECTION_TEST_TIME:
						decrement_Test_Time();
						break;
					case SELECTION_FORCE_SENSOR_CALIBERATION:
						decrement_Force_Sensor_Calibration();
						break;
					case SELECTION_CALIBERATION_TIME:
						decrement_Calibration_Time();
						break;
					case SELECTION_COMPASS_SAMPLES:
						decrement_Compass_Samples();
						break;
					case SELECTION_QRDPERCENT_LOW:
						decrement_QRDpercentlow();
						break;

					case SELECTION_QRDPERCENT_HIGH:
						decrement_QRDpercentHigh();
						break;

					case SELECTION_BATTERY_THRESHOLD:
						decrement_battery_threshold();
						break;

					case SELECTION_XFRACTION_VALUE:
						decrement_xfraction_value();
						break;

					}
					display_Config_Settings();

					break;

					case TEST_BUTTON_PRESSED:
						printf("Test Button Pressed \n");
						//                        Write_Data_file_information_To_Sdcard(); //Store settings to sd card
						Next_State = UI_NEW_TEST_LIST;  
						State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
						display_Testmenu();

						break;

					default:
						printf("Invalid Button Pressed \n");
						break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;


		}
		else
		{
			Next_State = UI_CONFIG_SETTINGS;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_Config_Settings();
		}
		break;

	case UI_DATE_AND_TIME:
//		printf("\nUI_SET_DATE_AND_TIME\n");
		/*
			Sets the date and time from settings screen.
		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				if(set_date_time_status == SET_DATE_TIME_FROM_SETTINGS)
				{
					Settings_menu_selection = 0;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					Next_State = UI_SETTINGS;
					display_Settings_Main();
				}
				else if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
				{
					if((set_date_while_booting_count == DATE_SET) && (set_time_while_booting_count == TIME_SET))
					{
						State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
						Next_State = UI_MAINMENU_LIST;
						display_Mainmenu();
						set_date_time_status = SET_DATE_TIME_FROM_SETTINGS;
						set_date_while_booting_count = DATE_NOT_SET;
						set_date_while_booting_count = TIME_NOT_SET;
					}
				}

				break;

			case RIGHT_BUTTON_PRESSED:
				printf("\nRight Button Pressed\n");

				State_of_Screen = Date_Time_menu_Table[Date_Time_menu_selection].UI_state;
				if(State_of_Screen == UI_UPDATE_DATE)
				{
					Date_Time_update_selection = 0;
					update_system_date_from_rtc();
				}
				else if(State_of_Screen == UI_UPDATE_TIME)
				{
					Date_Time_update_selection = 3;
					update_system_time_from_rtc();
				}
				else if(State_of_Screen == UI_UPDATE_ZONE)
					Time_Zone_List_selection = Settings_Data.Prev_Zone_List_selection;

				Date_Time_menu_Table[Date_Time_menu_selection].menu_Function();
				Next_State = State_of_Screen;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");

				Date_Time_menu_Key_up();
				Next_State = UI_DATE_AND_TIME;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_Date_Time_Menu();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Date_Time_menu_Key_down();
				Next_State = UI_DATE_AND_TIME;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				display_Date_Time_Menu(); 
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				if(set_date_time_status == SET_DATE_TIME_FROM_SETTINGS)
				{
					Next_State = UI_NEW_TEST_LIST;  
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
					Test_Menu_Prev_State = UI_DATE_AND_TIME;
					display_Testmenu();
				}
				else if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
				{
					if((set_date_while_booting_count == DATE_SET) && (set_time_while_booting_count == TIME_SET))
					{
						State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
						Next_State = UI_MAINMENU_LIST;
						display_Mainmenu();
						set_date_time_status = SET_DATE_TIME_FROM_SETTINGS;
						set_date_while_booting_count = DATE_NOT_SET;
						set_date_while_booting_count = TIME_NOT_SET;
					}
				}

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
#if DYNAMIC_UPDATE_TIME
			if(set_date_time_status == SET_DATE_TIME_FROM_SETTINGS)
			{
				update_system_date_time_from_rtc();
				if(temp_seconds_count != Settings_Seconds_Count)
				{	
					display_Date_Time_Menu();
					temp_seconds_count = Settings_Seconds_Count;
				}
			}
			else
				display_Date_Time_Menu();
#endif
			Next_State = UI_DATE_AND_TIME;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
		}
		break;

	case UI_UPDATE_DATE:
//		printf("\nUI_UPDATE_DATE\n");
		/*
			Sets the date.
		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				//SAVE THE DATE CODE HERE
				printf("TIME AND DATE: %s  %s",Time_from_settings , Date_from_settings);
				Update_rtc(Time_from_settings,Date_from_settings);   
				set_date_while_booting_count = DATE_SET;
				display_Date_Time_Menu();
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Next_State = UI_DATE_AND_TIME;
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("\nRight Button Pressed\n");
				if (Date_Time_update_selection == 2)
					Date_Time_update_selection = 0;
				else
					++Date_Time_update_selection; 
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_DATE;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Increment_date_time_selection();
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_DATE;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Decrement_date_time_selection();
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_DATE;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
				{
					Next_State = UI_UPDATE_DATE;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					break;
				}
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				Test_Menu_Prev_State = UI_UPDATE_DATE;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
#if DYNAMIC_UPDATE_TIME
			if(set_date_time_status == SET_DATE_TIME_FROM_SETTINGS)
			{
				update_system_time_from_rtc();
				if(temp_seconds_count != Settings_Seconds_Count)
				{
					display_Update_Date_Time_Menu();
					temp_seconds_count = Settings_Seconds_Count;
				}
			}
			else
				display_Update_Date_Time_Menu();
#endif
			Next_State = UI_UPDATE_DATE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
		}
		break;

	case UI_UPDATE_TIME:
		printf("\nUI_UPDATE_TIME\n");
		/*
			Sets the date.
		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				printf("TIME AND DATE: %s  %s",Time_from_settings , Date_from_settings);
				Update_rtc(Time_from_settings,Date_from_settings);   
				set_time_while_booting_count = TIME_SET;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Next_State = UI_DATE_AND_TIME;
				display_Date_Time_Menu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("\nRight Button Pressed\n");
				if(Time_format == TWELVE_HR_FORMAT)
				{
					if (Date_Time_update_selection == 6)
						Date_Time_update_selection = 3;
					else
						++Date_Time_update_selection; 
				}
				else
				{
					if (Date_Time_update_selection == 5)
						Date_Time_update_selection = 3;
					else
						++Date_Time_update_selection; 
				}
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_TIME;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Increment_date_time_selection();
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_TIME;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Decrement_date_time_selection();
				display_Update_Date_Time_Menu(); 
				Next_State = UI_UPDATE_TIME;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
				{
					Next_State = UI_UPDATE_TIME;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					break;
				}
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				Test_Menu_Prev_State = UI_UPDATE_TIME;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
#if DYNAMIC_UPDATE_TIME
			//			update_system_date_from_rtc();
			//			if(temp_seconds_count != Settings_Seconds_Count)
			//			{
			display_Update_Date_Time_Menu();
			//				temp_seconds_count = Settings_Seconds_Count;
			//			}
#else
			display_Update_Date_Time_Menu();
#endif
			Next_State = UI_UPDATE_TIME;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
		}
		break;

	case UI_UPDATE_ZONE:
		printf("\nUI_UPDATE_ZONE\n");
		/*
			Sets the date.
		 */
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Next_State = UI_DATE_AND_TIME;
				display_Date_Time_Menu();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("\nRight Button Pressed\n");
				//CALL FUNCTION TO UPDATE UTC OR DST
				if(Settings_Data.Dst_status == DST_ON)
				{
					Set_Time_Zone(TimeZoneTable[Time_Zone_List_selection].UTC_DST_Offset_Value
							- TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_Value);
				}
				else
				{
					Set_Time_Zone(TimeZoneTable[Time_Zone_List_selection].UTC_Offset_Value
							- TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_Value);
				}
				Settings_Data.Prev_Zone_List_selection = Time_Zone_List_selection;
				Write_Data_file_information_To_Sdcard(); //Store settings to sd card
				//				memset(Default_Time_Zone, 0, 13);
				//				memcpy(Default_Time_Zone, TimeZoneTable[Time_Zone_List_selection].Time_Zone_Name, strlen(TimeZoneTable[Time_Zone_List_selection].Time_Zone_Name));                 
				display_Date_Time_Menu(); 
				Next_State = UI_DATE_AND_TIME;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");

				Time_Zone_List_Key_up();
				display_Time_Zone_List();
				Next_State = UI_UPDATE_ZONE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Time_Zone_List_Key_down();
				display_Time_Zone_List();
				Next_State = UI_UPDATE_ZONE;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
				{
					Next_State = UI_UPDATE_ZONE;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					break;
				}
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				Test_Menu_Prev_State = UI_UPDATE_ZONE;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			display_Time_Zone_List();
			Next_State = UI_UPDATE_ZONE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		}
		break;

	case UI_ABOUT_DEVICE:
		printf("\nUI_ABOUT_DEVICE\n");
		/*
		 * BUTTON	NEW STATE				SOFTWARE ACTION
         LEFT	Settings Menu
         RIGHT	--	
         UP		--	
         DOWN	--	
         TEST	Test Menu (default)

		 * 
		 * 
		 */

		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				Next_State = UI_SETTINGS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Settings_menu_selection=0;
				display_Settings_Main();

				break;

			case RIGHT_BUTTON_PRESSED:
			case UP_BUTTON_PRESSED:		
				
				Time_Delay_Sleep(1000);
				if((lwgpio_get_pin_input(RIGHT_BUTTON) == 0) && (lwgpio_get_pin_input(UP_BUTTON) == 0))
				{
				   Start_sensor_output_screen_timer(2000);
				   while((lwgpio_get_pin_input(RIGHT_BUTTON) == 0) && (lwgpio_get_pin_input(UP_BUTTON) == 0) && Sensor_screan_timeout == 0)
				   {
				   }
					 if(Sensor_screan_timeout == 1)
					 {
						 Sensor_screan_timeout =0;

						 Next_State = UI_SENSOR_OUTPUT;
						 State_of_Screen = UI_SENSOR_OUTPUT;
						 
						 //Start_ADC();
						 power_rail_enable();
						 ADC_Init();
						 lsm303_i2c0_init();
						 Start_LSM();
						 if((GPS_Current_State == GPS_LOCKING)||(GPS_Current_State == GPS_POWER_ON))
						 {
							 lwgpio_int_enable(&FIX_AVL, FALSE );
						 }

						 GPS_Prev_State = GPS_Current_State;
						 GPS_Current_State = GPS_STANDBY;
						 resetCC4000API();  
						 gpsCC4000OStartFixes(); 
						 GPS_uart_init();

						 //Gps_on_for_sensor_output();
						 ui_timer_start(500);


						 break;
					 }
					 else
					 {
						 printf("Up Button Pressed \n");
						 Next_State = UI_ABOUT_DEVICE;
						 State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
						 break;
					 }
				}
				else
				{
					printf("Up Button Pressed \n");
					Next_State = UI_ABOUT_DEVICE;
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					break;
				}
				
//			case RIGHT_BUTTON_PRESSED:
//				printf("RIght Button Pressed \n");
//				Next_State = UI_ABOUT_DEVICE;
//				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				break;
//
//			case UP_BUTTON_PRESSED:
//
//				printf("Up Button Pressed \n");
//				Next_State = UI_ABOUT_DEVICE;
//				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_ABOUT_DEVICE;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				//                        Write_Data_file_information_To_Sdcard(); //Store settings to sd card
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_ABOUT_DEVICE;
				display_Testmenu();

				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_ABOUT_DEVICE;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_about_device();
		}
		break;


	case UI_BLUETOOTH_MAIN:
		//     printf("\nUI_BLUETOOTH_MAIN\n"

		if(temp_ble_status != ble_transmission_status)
		{
			switch(ble_transmission_status)
			{
				
			case BLUETOOTH_DEVICE_NOT_CONNECTED: 
				
				if(temp_ble_status == BLUETOOTH_DEVICE_CONNECTED)
				{
					State_of_Screen = Next_State;
				}
				else
				{
					display_Bluetooth_main();	
				}				
				break;

			case BLUETOOTH_DEVICE_CONNECTED:  			
				display_Bluetooth_main();
				break;


			case BLUETOOTH_QUERY_LIST_COMPLETED:			
				if(Send_File(Query_list_name,Query_list_name+2) == 0)
				{
					ble_transmission_status = BLUETOOTH_WAIT_FOR_SYNC;    		
					//printf("\nSending query list %s Success\n",Query_list_name);
				}
				else
				{
					ble_transmission_status = BLUETOOTH_QRY_LIST_NOT_CREATED;
					display_Bluetooth_main();
					Time_Delay_Sleep(1200);
					ble_transmission_status = BLUETOOTH_DEVICE_CONNECTED;
					printf("\nSending query list %s failed",Query_list_name);
					printf("\nSending %s Again\n",Query_list_name);
				}
				display_Bluetooth_main();
				break;
				
			case BLUETOOTH_SYNC_FILES:				
				display_Bluetooth_main();  
				
				Ble_status = Send_Files_From_Manifest_List("a:Manifest_File.txt");
				if(Ble_status == 0)
				{
					ble_transmission_status = BLUETOOTH_TRANSFER_COMPLETED;					
					display_Bluetooth_main(); 
//					iDevices_Disconnect();	
//					ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
					printf("\nSending Files From Manifest List : Success\n");
				}
				else if(Ble_status == -1)
				{   
					Stop_BLE_bulk_transfer();
					ble_transmission_status = BLUETOOTH_TRANSFER_FAILED;
					display_Bluetooth_main();

					printf("\nSending Files From Manifest List : Failed\n");
					
					if(Ble_Module_init())
					{
						printf("\nBLE Init failed.. Please try again\n"); 					
					}				
					ble_transmission_status = BLUETOOTH_TRANSFER_FAILED;
				}
				else if(Ble_status == 1)
				{
					ble_transmission_status = BLUETOOTH_TRANSFER_FAILED;
					display_Bluetooth_main();
					printf("\nSending Files From Manifest List : Failed\n");					
				}
				break;

			default:
				break;
			}
			
			temp_ble_status = ble_transmission_status;
			
		}
		
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				
				printf("Left Button Pressed \n");
				
				if(ble_transmission_status == BLUETOOTH_TRANSFER_FAILED)
				{
					if(Ble_status == -1)
					{
						lwgpio_int_enable(&BLE_host_interrupt, TRUE);
					}
					else{
						iDevices_Disconnect();
					}
					
				} 
				else
				{
					iDevices_Disconnect();
				}
						
				ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
				temp_ble_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
				State_of_Screen = Next_State;
//				lwgpio_int_enable(&BLE_host_interrupt, TRUE);
				
				break;

			case RIGHT_BUTTON_PRESSED:

				printf("\nRight Button Pressed \n");

				if(ble_transmission_status == BLUETOOTH_TRANSFER_COMPLETED)
				{
					//Need to modify
					
					iDevices_Disconnect();
				//	Next_State = Prev_State_of_Screen;
				//	State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
					ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
					temp_ble_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
//					Main_menu_selection = 0;
//					display_Mainmenu();
//					display_Bluetooth_main();
					State_of_Screen = Next_State;
//					lwgpio_int_enable(&BLE_host_interrupt, TRUE);
				}
				else if(ble_transmission_status == BLUETOOTH_TRANSFER_FAILED)
				{					
					if(Ble_status == -1){
						ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
					}
					else{
						ble_transmission_status = BLUETOOTH_DEVICE_CONNECTED;
					}
					display_Bluetooth_main();
				}
				
				
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//      Next_State = UI_BLUETOOTH_MAIN;  
				State_of_Screen = UI_BLUETOOTH_MAIN; 
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//      Next_State = UI_BLUETOOTH_MAIN;  
				State_of_Screen = UI_BLUETOOTH_MAIN; 
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
				iDevices_Disconnect();
//				Stop_BLE_bulk_transfer();
				State_of_Screen = UI_NEW_TEST_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		
		break;

	case UI_SETTINGS_PASSWORD:
		printf("UI_SETTINGS_PASSWORD\n");
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
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				if (Check_Ui_BackAction() == TRUE )
				{
					Next_State = UI_MAINMENU_LIST;  
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
					Main_menu_selection=0;
					display_Mainmenu();
				}
				else
				{
					Password_navigate_left();
					display_Settings_Password();
					Next_State = UI_SETTINGS_PASSWORD;  
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				}
				break;

			case RIGHT_BUTTON_PRESSED:
				if (Check_User_Authentication() == TRUE )
				{
					Next_State = UI_SETTINGS_SELECTION;  
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
					Settings_selection_menu_selection = 0;
					display_Settings_Selection();
				}
				else
				{
					Password_navigate_right();
					display_Settings_Password();
					Next_State = UI_SETTINGS_PASSWORD;  
					State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				}
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");
				Next_State = UI_SETTINGS_PASSWORD;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				Password_key_up();
				display_Settings_Password();

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Next_State = UI_SETTINGS_PASSWORD;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				Password_key_down();
				display_Settings_Password();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;  
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_SETTINGS_PASSWORD;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_Settings_Password();
		}
		break;

	case UI_SETTINGS_SELECTION:
		printf("UI_SETTINGS_SELECTION\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *

             BUTTON	NEW STATE	SOFTWARE ACTION
             LEFT	Main Menu

             RIGHT	--	
             UP		--	
             DOWN	--	
             TEST	Test Menu (default)

             Additional Notes:
             Populate labels with sensor data

			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");

				Next_State = UI_MAINMENU_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Main_menu_selection = 0;
				display_Mainmenu();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				Config_settings_selection = SELECTION_SAMPLING_RATE;
				Settings_menu_selection=0;   
				State_of_Screen=SettingsSelection_Table[Settings_selection_menu_selection].UI_state;
				SettingsSelection_Table[Settings_selection_menu_selection].menu_Function();
				Next_State=State_of_Screen;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Settings_Selection_Key_up();
				display_Settings_Selection();
				Next_State = UI_SETTINGS_SELECTION;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Settings_Selection_Key_down();
				display_Settings_Selection();
				Next_State = UI_SETTINGS_SELECTION;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				Test_Menu_Prev_State = UI_SETTINGS_SELECTION;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if (Check_UI_Timer_Timeout() == TIME_OUT)
		{
			Next_State = UI_SETTINGS_SELECTION;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF; 
			display_Settings_Selection();
		}
		break;

	case UI_SENSOR_OUTPUT:
		//    	printf("UI_SENSOR_OUTPUT\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			/*
			 *

             BUTTON	NEW STATE	SOFTWARE ACTION
             LEFT	Main Menu

             RIGHT	--	
             UP		--	
             DOWN	--	
             TEST	Test Menu (default)

             Additional Notes:
             Populate labels with sensor data

			 * */
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");

//				Next_State = UI_MAINMENU_LIST;
//				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;				
				
				printf("Left Button Pressed \n");
				Next_State = UI_SETTINGS;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
//				Settings_menu_selection=0;
				display_Settings_Main();
				
				Main_menu_selection = 0;

				ADC_deinit();
				Stop_LSM();
				Lsm303_deinit();
				//					Gps_off_for_sensor_output();
				//					gpsCC4000Off();
				//					gps_disable();
				gpsCC4000OStopFixes();
				GPS_uart_deinit();
				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
					lwgpio_int_enable(&FIX_AVL, TRUE );
				}

				GPS_Current_State = GPS_Prev_State;
//				display_Mainmenu();

				break;

			case RIGHT_BUTTON_PRESSED:

				printf("Right Button Pressed \n");
				//Nothing to be done
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				//Nothing to be done

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				//Nothing to be done
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Next_State = UI_NEW_TEST_LIST;
				State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
				ADC_deinit();
				Stop_LSM();
				Lsm303_deinit();
				//Gps_off_for_sensor_output();
				GPS_uart_deinit();
				if((GPS_Prev_State == GPS_LOCKING)||(GPS_Prev_State == GPS_POWER_ON))
				{
					lwgpio_int_enable(&FIX_AVL, TRUE );
				}
				else
				{
					gpsCC4000OStopFixes();
				}
				GPS_Current_State = GPS_Prev_State;
				Test_Menu_Prev_State = UI_MAINMENU_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;

			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else if (Check_UI_Timer_Timeout() == TIME_OUT)
		{
			display_Sensor_Output();
			ui_timer_start(500);
		}
		break;

	case UI_USB_MASS_STORAGE:
		printf("UI_USB_MASS_STORAGE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	case UI_MASS_STORAGE_ENABLE:
		//    	printf("UI_MASS_STORAGE_ENABLE\n");
		if(((1 << GPIO_PIN17) & GPIOA_PDIR) != 0)
		{
			USB_DPin_Low();
			/* Switch to core to high frequency */
			Switch_to_idle_mode();

			BLE_Advertising(ON);
			
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			
			char pathname[261];
			ioctl(filesystem_handle, IO_IOCTL_GET_CURRENT_DIR,(uint_32_ptr) pathname);
			ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
			
			if(Check_Firmware_Update()==1)
			{
				ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
				State_of_Screen = UI_FIRMWARE_UPDATE;
				Display_Firmware_Update_Available();
			}
			else
			{
				ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR,(uint_32_ptr) pathname);
				State_of_Screen = Next_State;
			}
		}
		break;
		
		
	case UI_FIRMWARE_UPDATE:
		printf("\nUI_FIRMWARE_UPDATE\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:
				printf("Left Button Pressed \n");
				Display_Firmware_Update_Available();
				break;

			case RIGHT_BUTTON_PRESSED:
				printf("Right Button Pressed \n");
				Write_Flash_DFU();
				Switch_acquisition_mode();
				Sd_Card_deinit();
				Software_reboot();
				break;

			case UP_BUTTON_PRESSED:
				printf("Up Button Pressed \n");
				Display_Firmware_Update_Available();
				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");
				Display_Firmware_Update_Available();
				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				Display_Firmware_Update_Available();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		else
		{
			Next_State = UI_FIRMWARE_UPDATE;
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			Display_Firmware_Update_Available();
		}
		break;

	case UI_CANT_PROCEED_NEED_TO_TURN_OFF:
		printf("UI_CANT_PROCEED_NEED_TO_TURN_OFF\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{
			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	case UI_REBOOT_CONFIRM:
		printf("UI_REBOOT_CONFIRM\n");
		if (Button_Press_Status != BUTTON_PRESS_NOT_PENDING)
		{

			switch (Button_Press_Status)
			{
			case LEFT_BUTTON_PRESSED:

				printf("Left Button Pressed \n");
				State_of_Screen = UI_CANT_PROCEED_NEED_TO_TURN_OFF;
				Software_reboot();
				break;

			case RIGHT_BUTTON_PRESSED:
				State_of_Screen = UI_MAINMENU_LIST;
				display_Mainmenu();
				break;

			case UP_BUTTON_PRESSED:

				printf("Up Button Pressed \n");

				break;

			case DOWN_BUTTON_PRESSED:
				printf("Down Button Pressed \n");

				break;

			case TEST_BUTTON_PRESSED:
				printf("Test Button Pressed \n");
				State_of_Screen = UI_NEW_TEST_LIST;
				display_Testmenu();
				break;

			default:
				printf("Invalid Button Pressed \n");
				break;
			}

			Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
		}
		break;

	default:
		printf("DEFAULT_CASE\n");
		Next_State = UI_SNOW_PROFILE_LIST;
		State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
		printf("Invalid UI State \n");
		break;
	}
}

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
