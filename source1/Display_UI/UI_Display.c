/******************************************************************************
 *
 *   Filename: $UI_Display.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of display screens
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <string.h>
#include<stdlib.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include <mfs.h>
#include "bitmap.h"
#include "spi_lcd.h"
#include "spi_config.h"
#include "config.h"
#include "UI_Display.h"
#include "UI_Display_Time.h"
#include "UI_task.h"
#include "UI_Settings.h"
#include "Sd_Card.h"
#include "Data_Processessing_Functions.h"
#include "UI_icon_manipulation.h"
#include "Gps.h"
#include "Sensor_Configuration.h"
#include "Bulk_transfer.h"
#include "common_headers.h"

#define BATTERY_VALUE_DISPLAY_ENABLE 0
void Increment_Default_Graph_View_from_Settings(void);
static void Create_favourites_menu_Content(void);
static void Create_about_device_Content(void);
static void Create_Reboot_confirm_Content(void);
static void Create_dispaly_Usb_Connected_Content(void);
static void Create_display_Mass_Storage_Enabled_Content(void);
static void Create_dispaly_battery_low(void);
static void Create_record_not_found_content(void);
static void Create_dispaly_rebooting_content(void);
static void Create_dispaly_sdcard_insert(void);
static void Create_dispaly_Waiting_for_file_Content(void);
static void Create_New_Snow_Profile_Collecting_Content(void);
static void Create_New_Snow_Profile_Calibration_Content(void);
static void Create_Bluetooth_main_Content(void);
static void Create_Sensor_Output_Content(void);
static void Create_Slope_Measurement_Content(void);
static void Create_MultipleTestmenu_Content(void);
static void Create_Mainmenu_Content(void);
//static void Create_Settings_Selection_Content(void);
extern void Change_Defualt_Gragh_View(void);
static void Create_GPSmenu_Content(void);



static void Create_IRDMSmenu_Content(void);
static void Create_PressureMenu_Content(void);
static void Create_ROS1_Menu_Content(void);
static void Create_ROS2_Menu_Content(void);
static void Create_AccelerometerMenu_Content(void);
static void Create_Accelerometer_Calib_Values_Error_Screen_Content(void);
static void Create_MagnetometerMenu_Content(void);
static void Create_compass_calibration_screen_content(void); 
static void Create_accelerometer_calibration_screen_content(void); 

int comp(const void * a, const void * b);

void display_IRDMS_Calibration(void);
void display_Pressure_sensor_Calibration(void);
void display_Accelerometer_Calibration(void);
void display_Magnetometer_Calibration(void);
void display_Collect_Accelerometer_Calibration_Data(void);
extern void display_Accelerometer_Calib_Values_Error_Screen();
void display_ROS1_Calibration(void);
void display_ROS2_Calibration(void);
void display_GPS_Calibration(void);
void display_compass_calibration_screen(void);

void print_calib_data(CalibTable *Sensor_Calib_Table,uint_8 No_of_calib_condition);
void Read_calib_data(const char file_name[32],CalibTable *Sensor_Calib_Table);


uint_8 calibrate_IRDMS(void);
uint_8 calibrate_Pressure_Sensor(void);

extern void Add_Item_To_Menu(const char *menu_Text, uint_8 menu_position,
		uint_16 menu_selection_status);
extern void Create_Footer(char * leftMenu,uint16_t left_len, char * rightMenu,uint16_t right_len);
void Create_Title(char * aString, uint16_t string_length);
void Create_Title_calib(char * aString, uint16_t string_length,uint_16 title_x,uint_16 title_y);
static void Create_Title_old(char * titleString);
void delete_snow_profile_selected_from_favourite(void);
void Low_battery_shutdown(void);
void Battery_checkon_powerup(void);
void Enable_all_sensors(void);
void Disable_all_sensors(void);

uint_8 Calib_status[8] ={0};

unsigned char Test_disp_ctr=0;

volatile uint_8 Folder_name_index=0;
volatile uint_8 File_name_index=0;
char temp_test_file_name[16] = {"TEST"};
uint_8 tempCount;

volatile float Battery_minimum_voltage_check = 3.0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 File_name_count=0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Folder_name_count=0;

volatile uint_8 Date_Folder_name_count=0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Folder_name_selection=0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 File_name_selection=0;

extern uchar usb_test_flag;
//This variable is incremented and decremented according to navigation key 
volatile uint_8 Main_menu_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Test_menu_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Settings_selection_menu_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_16 Snow_Profiles_File_List_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_16 Snow_Profiles_List_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_16 Favourites_List_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Settings_menu_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Default_graph_view_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Snow_profile_options_selection=2;




//This variable is incremented and decremented according to navigation key 
volatile uint_8 multiple_tests_status=MULTIPLE_TESTS_OFF;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 test_number=TEST_NUMBER_ONE;

//This variable is adjusted according to bluetooth status
volatile uint_8 ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;

volatile uint_16 Favourites_List_count = 0;
volatile uint_16 Favourites_file_count=0;

volatile uint_8 Snow_Profiles_File_List_count=0;
volatile uint_16 Snow_Profiles_List_count = 0;
const uint_16 Setings_Password_Key = 313;
volatile uint_8 Setings_Password_Key_X_position = 0;

volatile uint_8 Setings_Password_Key_X1 = 0;
volatile uint_8 Setings_Password_Key_X2 = 0;
volatile uint_8 Setings_Password_Key_X3 = 0;

volatile uint_8 Config_settings_selection = 0;

uint_8 Battery_adc_stucked_and_shows_zero = 0;
uint_8 Battery_low_status = 0;

uint_8 compass_calibration_screen = COMPASS_CALIBRATION_START_SCREEN;
uint_8 accelerometer_calibration_screen = ACC_CALIBRATION_ACC_SCREEN_DOWN;

//DateFolderEntry Date_Folder[100];


////This variable is incremented and decremented according to navigation key 
//volatile uint_8 IRDMS_Condition_selection = 0;
//volatile uint_8 Pressure_Condition_selection = 0;
//volatile uint_8 ROS_Condition_selection = 0;
//volatile uint_8 Accelerometer_Condition_selection = 0;
//volatile uint_8 Magnetometer_Condition_selection = 0;
//volatile uint_8 Acc_reading_status = 0;


char Snow_Profile_Directory[64][16];  //Folder Name: Format is "YYYYMMDD"

char Snow_Profile_List_Data[MAX_NO_OF_FILES_IN_DIR][16];	//File Date: Format is "(X/B)HHMM(A/P)SS" {X and B- type of file; A/P- AM or PM

char Snow_Profile_File_Name[MAX_NO_OF_FILES_IN_DIR][16]; 	//File Name: Format is "(X/B)0000001" where 1 is the test count.

FavouriteTableEntry FavouriteEntry[64];

//Table for displaying mainmenu and corresponding functions and UI states
MenuTableEntry Mainmenu_Table[] = {
		//   Num,   display_name,       	function,                     state	
		{ 0, "IRDMS",      			display_IRDMS_Calibration,          	UI_CALIBRATION_IRDMS 			}, 
		{ 1, "PRESSURE SENSOR", 	display_Pressure_sensor_Calibration, 	UI_CALIBRATION_PRESSURE 		}, 
		{ 2, "ACCELEROMETER 1",    	display_Collect_Accelerometer_Calibration_Data,   	UI_ACCELEROMETER_CALIBRATION_SCREEN_DOWN}, 
		{ 3, "ACCELEROMETER 2",    	display_Accelerometer_Calibration,   	UI_CALIBRATION_ACCELEROMETER 	}, 
//		{ 3, "MAGNETOMETER",      	display_Magnetometer_Calibration,  		UI_CALIBRATION_MAGNETOMETER		}, 
		{ 4, "MAGNETOMETER",	display_compass_calibration_screen,  		UI_COMPASS_CALIBRATION			}, 	
		{ 5, "ROS 1", 				display_ROS1_Calibration ,        		UI_CALIBRATION_ROS_1 			}, 
		{ 6, "ROS 2", 				display_ROS2_Calibration ,        		UI_CALIBRATION_ROS_2 			},
		{ 7, "GPS", 				display_GPS_Calibration,    			UI_CALIBRATION_GPS 				}, 

#define NUM_OF_MAINMENU_ITEM 8

};


//Table for displaying Test menu and corresponding functions and UI states
MenuTableEntry Testmenu_Table[] = {

		{ 0, "Snow Profiles", display_New_Snow_profile, UI_NEW_SNOW_PROFILE }, { 1, "Slope Measurement",
				display_Slope_Measurment, UI_SLOP_MEASURMENTS },
				//        
				//		{ 0, "NEW TEST", display_Testmenu, UI_NEW_TEST_LIST }, 
				//		{ 1, "Slope Measurement",display_Slope_Measurment, UI_SLOP_MEASURMENTS },

#define NUM_OF_TESTMENU_ITEM 2
};

//Table for displaying Settings selection and corresponding functions and UI states
MenuTableEntry SettingsSelection_Table[] = 
{
		{ 0, "SETTINGS MENU", 	    	 display_Settings_Main, 		 UI_SETTINGS    },
		{ 1, "CONFIG SETTINGS", 		 display_Config_Settings, 		 UI_CONFIG_SETTINGS	  }, 


#define NUM_OF_SETTINGSSELECTION_ITEM 2
};

//Table for displaying settings menu and corresponding functions and UI states
MenuTableEntry Settingsmenu_Table[] = {
		//   Num,   display_name,       	function,                    		 state	
		{ 0, "DATE & TIME",         display_Date_Time_Menu,         		UI_DATE_AND_TIME }, 
		{ 1, "GPS",      			display_gps_toggle,           			UI_SETTINGS }, 
		{ 2, "BLUETOOTH", 			display_bluetooth_toggle, 				UI_SETTINGS }, 
		//{ 3, "MULTIPLE TESTS", 		display_Settings_Main,  				UI_SETTINGS }, 
		//        { 3, "MULTIPLE TESTS", 		display_multiple_tests_toggle,  		UI_SETTINGS }, 
		{ 3, "GRAPH DETAIL",  		Increment_Default_Graph_View_from_Settings, 			UI_SETTINGS }, 
		{ 4, "ABOUT DEVICE",        display_about_device,         		    UI_ABOUT_DEVICE }, 
		//     { 5, "Reboot", display_confirm_reboot,            UI_REBOOT_CONFIRM },

#define NUM_OF_SETTINGSMENU_ITEM 5	

};

//Table for displaying date and time menu and corresponding functions and UI states
MenuTableEntry Date_Time_menu_Table[] = {
		//   Num,   display_name,       	function,                    		 state	
		{ 0, "DATE",         		display_Update_Date_Time_Menu,     	UI_UPDATE_DATE	 }, 
		{ 1, "TIME FORMAT",      	display_time_format_toggle,         UI_DATE_AND_TIME }, 
		{ 2, "TIME", 				display_Update_Date_Time_Menu,		UI_UPDATE_TIME	 }, 
		{ 3, "ZONE", 				display_Time_Zone_List,  			UI_UPDATE_ZONE	 },
		{ 4, "DST",  				display_dst_toggle,		    		UI_DATE_AND_TIME }, 

#define NUM_OF_DATE_TIME_MENU_ITEM 5	

};

//Table for displaying Snow profile options menu and corresponding functions and UI states
MenuTableEntry Snowprofileoptions_Table[] = {
		//   Num,   display_name,       function,                     			state	
		{ 2, "FAVORITE",     	display_set_favourite,      		UI_SET_FAVOURITE }, 
		{ 3, "SLOPE", 			display_save_slope, 				UI_SAVE_SLOPE	 }, 
		{ 4, "GPS",    			dummy_function,      				UI_GPS_LOCATION	 }, 
		{ 5, "DETAIL",     		dummy_function,  					UI_VIEW_CHARTS	 }, 
		{ 6, "DELETE",         	display_Snow_Profiles_Delete,       UI_DELETE_PROFILE}, 


#define NUM_OF_SNOWPROFILEOPTIONS_ITEM 5	

};




//Table for displaying Snow profile options menu and corresponding functions and UI states
MenuTableEntry Defaultgraphview_Table[] = {
		//   Num,   display_name,       function,                     state	
		{ 0, "GRAPH DETAIL 0",      dummy_function,			UI_SETTINGS }, 
		{ 1, "GRAPH DETAIL 1", 		dummy_function, 		UI_SETTINGS },
		{ 0, "GRAPH DETAIL 2",      dummy_function,			UI_SETTINGS }, 
		{ 1, "GRAPH DETAIL 3", 		dummy_function, 		UI_SETTINGS }, 
		{ 0, "GRAPH DETAIL 4",      dummy_function,    		UI_SETTINGS }

		//     { 5, "Reboot", display_confirm_reboot,            UI_REBOOT_CONFIRM },

#define NUM_OF_DEFAULTGRAPHVIEW_ITEM 5	

};

/*-----------------------------------------------------------------------------* 
 * Function:    display_IRDMS_Calibration
 * Brief:       display_IRDMS_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_IRDMS_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("IRDMS ",strlen("IRDMS "),0,22);
	Create_IRDMSmenu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("BACK"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Pressure_sensor_Calibration
 * Brief:       display_Pressure_sensor_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Pressure_sensor_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("PRESSURE SENSOR",strlen("PRESSURE SENSOR"),0,22);
	Create_PressureMenu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("BACK"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Accelerometer_Calibration
 * Brief:       display_Accelerometer_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Accelerometer_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("ACCELEROMETER",strlen("ACCELEROMETER"),0,22);
	Create_AccelerometerMenu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("BACK"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Accelerometer_Calib_Values_Error_Screen
 * Brief:       display_Accelerometer_Calib_Values_Error_Screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Accelerometer_Calib_Values_Error_Screen(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("ACCELEROMETER",strlen("ACCELEROMETER"),0,22);
	Create_Accelerometer_Calib_Values_Error_Screen_Content();
	Create_Footer( "BACK",strlen("BACK"), "",strlen(""));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}


/*-----------------------------------------------------------------------------* 
 * Function:    display_Accelerometer_Calibration
 * Brief:       display_Accelerometer_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Collect_Accelerometer_Calibration_Data(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib(" CALIBRATION",strlen(" CALIBRATION"),0,2);
	Create_Title_calib("ACCELEROMETER",strlen("ACCELEROMETER"),0,22);
	Create_accelerometer_calibration_screen_content();
	Create_Footer( "BACK",strlen("BACK"), "COLLECT",strlen("COLLECT"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Magnetometer_Calibration
 * Brief:       display_Magnetometer_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Magnetometer_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("MAGNETOMETER",strlen("MAGNETOMETER"),0,22);
	Create_MagnetometerMenu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("SAVE"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_compass_calibration_screen
 * Brief:       Display the compass calibration screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_compass_calibration_screen(void)
{
	//buff_clear();
	//	if(compass_calibration_screen == COMPASS_CALIBRATION_START_SCREEN)
	//	{
	//		Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	//	}
	//	else
	//	{
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	//	}
	//	Create_Header();
	//	Create_Title("COMPASS CALIBRATION", strlen("COMPASS CALIBRATION"));
	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("COMPASS",strlen("COMPASS"),0,22);

	Create_compass_calibration_screen_content();

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);


}

/*-----------------------------------------------------------------------------* 
 * Function:    display_ROS_Calibration
 * Brief:       display_ROS_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_ROS1_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("ROS 1",strlen("ROS 1"),0,22);
	Create_ROS1_Menu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("BACK"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_ROS_Calibration
 * Brief:       display_ROS_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_ROS2_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("ROS 2",strlen("ROS 2"),0,22);
	Create_ROS2_Menu_Content();
	Create_Footer( "BACK",strlen("BACK"), "SAVE",strlen("BACK"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_GPS_Calibration
 * Brief:       display_GPS_Calibration
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_GPS_Calibration(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);

	Create_Title_calib("CALIBRATION",strlen("CALIBRATION"),0,2);
	Create_Title_calib("GPS",strlen("GPS"),0,22);
	Create_GPSmenu_Content();
	Create_Footer( "BACK",strlen("BACK"), "",0);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_IRDMSmenu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_GPSmenu_Content(void)
{
	uint_16 x_position, y_position;
	char GPS_Data[10];

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("SEARCHING...")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "SEARCHING...", COLOUR_BLACK, MEDIUM_FONT);

	memset(GPS_Data,0x00,10);
	RTC_TIME_STRUCT Gps_Time;
	_rtc_get_time (&Gps_Time);
	sprintf(GPS_Data,"%d",Gps_Time.seconds);

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(GPS_Data)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, GPS_Data, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);
}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_IRDMSmenu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_IRDMSmenu_Content(void)
{
	uint_16 x_position, y_position;
	float IRDMS_voltage = 0.0;
	char  IRDMS_voltage_string[16];

	ADC_RESULT_STRUCT adc_out;
	memset(IRDMS_voltage_string,0x00,16);	

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2)  -50 - (((strlen(IRDMS_Calib_Table[IRDMS_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) IRDMS_Calib_Table[IRDMS_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if (read(ir_sens, &adc_out,sizeof(adc_out)))
	{
		//		printf("Adc_out = %X",adc_out.result);
		IRDMS_voltage = (adc_out.result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		sprintf(IRDMS_voltage_string,"%0.2f V",IRDMS_voltage);
		printf("\nIRDMS Voltage =%0.2f V",IRDMS_voltage);
	}

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(IRDMS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, IRDMS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);


	if(IRDMS_Calib_Table[IRDMS_Condition_selection].curr_voltage < 3.5)
	{
		sprintf(IRDMS_voltage_string,"%0.2f V",IRDMS_Calib_Table[IRDMS_Condition_selection].curr_voltage);
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(IRDMS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, IRDMS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	}

}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_IRDMSmenu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_PressureMenu_Content(void)
{
	uint_16 x_position, y_position;
	float PRESSURE_voltage = 0.0;
	char  PRESSURE_voltage_string[16];

	ADC_RESULT_STRUCT adc_out;

	memset(PRESSURE_voltage_string,0x00,16);	

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2)  -30 - (((strlen(Pressure_Calib_Table[Pressure_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) Pressure_Calib_Table[Pressure_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if (read(force_sens, &adc_out,sizeof(adc_out)))
	{
		//		printf("Adc_out = %X",adc_out.result);
		PRESSURE_voltage = (adc_out.result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		sprintf(PRESSURE_voltage_string,"%0.2f V",PRESSURE_voltage);
		printf("\nPressure Voltage =%0.2f V",PRESSURE_voltage);
	}

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(PRESSURE_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, PRESSURE_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);


	if(Pressure_Calib_Table[Pressure_Condition_selection].curr_voltage < 3.5)
	{
		sprintf(PRESSURE_voltage_string,"%0.4f V",Pressure_Calib_Table[Pressure_Condition_selection].curr_voltage);
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(PRESSURE_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, PRESSURE_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_ROS_Menu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_ROS1_Menu_Content(void)
{
	uint_16 x_position, y_position;
	float ROS_voltage = 0.0;
	char  ROS_voltage_string[16];

	ADC_RESULT_STRUCT adc_out;

	memset(ROS_voltage_string,0x00,16);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2) -50 - (((strlen(ROS1_Calib_Table[ROS1_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) ROS1_Calib_Table[ROS1_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if (read(ros_sens1, &adc_out,sizeof(adc_out)))
	{
		//		printf("Adc_out = %X",adc_out.result);
		ROS_voltage = (adc_out.result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		sprintf(ROS_voltage_string,"%0.2f V",ROS_voltage);
		printf("\nROS Voltage =%0.2f V",ROS_voltage);
	}

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(ROS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, ROS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);


	if(ROS1_Calib_Table[ROS1_Condition_selection].curr_voltage < 3.5)
	{
		sprintf(ROS_voltage_string,"%0.2f V",ROS1_Calib_Table[ROS1_Condition_selection].curr_voltage);
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(ROS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, ROS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	}

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_ROS_Menu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_ROS2_Menu_Content(void)
{
	uint_16 x_position, y_position;
	float ROS_voltage = 0.0;
	char  ROS_voltage_string[16];

	ADC_RESULT_STRUCT adc_out;

	memset(ROS_voltage_string,0x00,16);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2) -50 - (((strlen(ROS2_Calib_Table[ROS2_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) ROS2_Calib_Table[ROS2_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if (read(ros_sens2, &adc_out,sizeof(adc_out)))
	{
		//		printf("Adc_out = %X",adc_out.result);
		ROS_voltage = (adc_out.result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		sprintf(ROS_voltage_string,"%0.2f V",ROS_voltage);
		printf("\nROS Voltage =%0.2f V",ROS_voltage);
	}

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(ROS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, ROS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);


	if(ROS2_Calib_Table[ROS2_Condition_selection].curr_voltage < 3.5)
	{
		sprintf(ROS_voltage_string,"%0.2f V",ROS2_Calib_Table[ROS2_Condition_selection].curr_voltage);
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(ROS_voltage_string)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, ROS_voltage_string, COLOUR_BLACK, MEDIUM_FONT);
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_IRDMSmenu_Content
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_AccelerometerMenu_Content(void)
{

	char  	tempString[16];
	char 	aspect[4];
	uint_16 magnetic_heading = 0xffff;
	int_16  tempAngle = 0xffff;
	uint_16 x_position, y_position;


	memset(tempString,0x0,16);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2) - 10 -
			(((strlen(Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if(Acc_reading_status == 1)
	{
		Acc_reading_status = 0;
		tempAngle = (int_16)(Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage);
		sprintf(tempString, "%d°", tempAngle);
		printf("\nSlope Angle = %d°",tempAngle);
	}
	else if(Count_angle != 0)
	{
		Angle_result = (((float)(Angle_result/Count_angle)) + 0.5);
		sprintf(tempString, "%d°", Angle_result);
		printf("\nSlope Angle = %d°",Angle_result);
		Count_angle = 0;
		Angle_result = 0;
	}
	else if(get_slope_measurement(&tempAngle, aspect, &magnetic_heading) == 0)
	{
		sprintf(tempString, "%d°", tempAngle);
		printf("\nSlope Angle = %d°",tempAngle);
	}


	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(tempString)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, tempString, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);

	if(Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage < 150)
	{
		sprintf(tempString,"%d°",(int_16)(Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage));
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(tempString)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, tempString, COLOUR_BLACK, MEDIUM_FONT);
	}

}



/*-----------------------------------------------------------------------------* 
 * Function:    Create_Accelerometer_Calib_Values_Error_Screen_Content
 * Brief:       Create_Accelerometer_Calib_Values_Error_Screen_Content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Accelerometer_Calib_Values_Error_Screen_Content(void)
{
	int x_position, y_position;
	
	x_position = (DISPLAY_X_MAX / 2) - (((strlen("WARNING:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 150;
	Draw_string(x_position, y_position, (uint_8 *) "WARNING:", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("NO ACC CONSTANTS")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 180;
	Draw_string(x_position, y_position, (uint_8 *) "NO ACC CONSTANTS", COLOUR_BLACK);\

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("FOUND!")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 200;
	Draw_string(x_position, y_position, (uint_8 *) "FOUND!!", COLOUR_BLACK);
	
}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_MagnetometerMenu_Content
 * Brief:       Create_MagnetometerMenu_Content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_MagnetometerMenu_Content(void)
{

	char  	tempString[16];
	char 	aspect[4];
	uint_16 magnetic_heading = 0xffff;
	int_16  tempAngle = 0xffff;
	uint_16 x_position, y_position;

	memset(tempString,0x0,16);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("CONDITION:")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 125;
	Draw_string_new(x_position, y_position, (uint_8 *) "CONDITION:", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2) - 30 -
			(((strlen(Magnetometer_Calib_Table[Magnetometer_Condition_selection].Calib_condition)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 160;
	Draw_string_new(x_position, y_position,
			(uint_8 *) Magnetometer_Calib_Table[Magnetometer_Condition_selection].Calib_condition, 
			COLOUR_BLACK, LARGE_FONT);


	if(Count_heading != 0)
	{		
		qsort(mag_temp_heading_buff, Count_heading, sizeof(uint_16), comp);
		Heading_result = mag_temp_heading_buff[(Count_heading / 2)];
		sprintf(tempString, "%d°", Heading_result);
		printf("\nSlope Angle = %d°",Heading_result);

		memset(mag_temp_heading_buff,0x00,200);
		Count_heading = 0;
		Heading_result = 0;
	}
	else if(get_slope_measurement(&tempAngle, aspect, &magnetic_heading) == 0)
	{
		sprintf(tempString, "%d°", magnetic_heading);
		printf("\nAspect = %d°",magnetic_heading);
	}

	x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(tempString)) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = 251;
	Draw_string_new(x_position, y_position, tempString, COLOUR_BLACK, MEDIUM_FONT);
	Draw_Rect_Box(70,246,160,286,COLOUR_BLACK);

	if(Magnetometer_Calib_Table[Magnetometer_Condition_selection].curr_voltage < 500)
	{
		sprintf(tempString,"%d°",(int_16)(Magnetometer_Calib_Table[Magnetometer_Condition_selection].curr_voltage));
		y_position = 300;
		x_position = (DISPLAY_X_MAX / 2) -5 - (((strlen(tempString)) / 2) * NUM_X_PIXEL_PER_CHAR);
		Draw_string_new(x_position, y_position, tempString, COLOUR_BLACK, MEDIUM_FONT);
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_accelerometer_calibration_screen_content
 * Brief:       This will add device details screen content in frame buffer
 * Parameter:   None
 * Return:      None
-----------------------------------------------------------------------------*/
static void Create_accelerometer_calibration_screen_content(void)
{

	uint_16 y_position;

	if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_SCREEN_DOWN)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 1:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(53, y_position + 30, "SCREEN DOWN", COLOUR_BLACK, MEDIUM_FONT);
	}
	else if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_SCREEN_UP)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 2:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(68, y_position + 30, "SCREEN UP", COLOUR_BLACK, MEDIUM_FONT);
	}
	else if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_TIP_POINT_DOWN)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 3:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(43, y_position + 30, "TIP POINT DOWN", COLOUR_BLACK, MEDIUM_FONT);
	}
	else if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_TIP_POINT_UP)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 4:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(60, y_position + 30, "TIP POINT UP", COLOUR_BLACK, MEDIUM_FONT);
	}
	else if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_SCREEN_FACE_IN)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 5:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(42, y_position + 30, "SCREEN FACE IN", COLOUR_BLACK, MEDIUM_FONT);
	}
	else if(accelerometer_calibration_screen == ACC_CALIBRATION_ACC_SCREEN_FACE_OUT)
	{
		y_position = 175;
		Draw_string_new(63,y_position,"CONDITION 6:",COLOUR_BLACK, MEDIUM_FONT);
		Draw_string_new(36, y_position + 30, "SCREEN FACE OUT", COLOUR_BLACK, MEDIUM_FONT);
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_compass_calibration_screen_content
 * Brief:       This will add device details screen content in frame buffer
 * Parameter:   None
 * Return:      None
-----------------------------------------------------------------------------*/
static void Create_compass_calibration_screen_content(void)
{

	uint_16 y_position;

	//	if(compass_calibration_screen == COMPASS_CALIBRATION_START_SCREEN)
	//	{
	y_position = 75;
	Draw_string_new(25,y_position,"DO THE FOLLOWING ",COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(25, y_position + 20, "FOR 15 SECONDS", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(38, 182, "ROLL", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(38, 244, "SPIN", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(38, 306, "FLIP", COLOUR_BLACK, MEDIUM_FONT);
	Draw_image(130, 155, COMPASS_ALL_ICON_NUM, COLOUR_BLACK);

	Create_Footer("BACK",strlen("BACK"), "",strlen(""));
	//	}
	//	else if(compass_calibration_screen == COMPASS_CALIBRATION_ROLL_SCREEN)
	//	{
	//		y_position = 120;
	//		Draw_string_new(97,y_position,"ROLL",COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_string_new(58, y_position + 20, "(5 SECONDS)", COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_image(50, 187, COMPASS_ROLL_ICON_NUM, COLOUR_BLACK);
	//		Create_Footer("CANCEL",strlen("CANCEL"), "",strlen(""));	
	//
	//	}
	//	else if(compass_calibration_screen == COMPASS_CALIBRATION_SPIN_SCREEN)
	//	{
	//		y_position = 120;
	//		Draw_string_new(97,y_position,"SPIN",COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_string_new(58, y_position + 20, "(5 SECONDS)", COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_image(55, 212, COMPASS_SPIN_ICON_NUM, COLOUR_BLACK);
	//		Create_Footer("CANCEL",strlen("CANCEL"), "",strlen(""));	
	//
	//	}
	//	else if(compass_calibration_screen == COMPASS_CALIBRATION_FLIP_SCREEN)
	//	{
	//		y_position = 120;
	//		Draw_string_new(97,y_position,"FLIP",COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_string_new(58, y_position + 20, "(5 SECONDS)", COLOUR_BLACK, MEDIUM_FONT);
	//		Draw_image(81, 187, COMPASS_FLIP_ICON_NUM, COLOUR_BLACK);
	//		Create_Footer("CANCEL",strlen("CANCEL"), "",strlen(""));	
	//
	//	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    gps_toggle
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_gps_toggle(void)
{
	if(Settings_Data.GPS_power_status == GPS_ON)
	{
		Settings_Data.GPS_power_status = GPS_OFF;
		lwgpio_int_enable(&FIX_AVL, FALSE );
		gpsCC4000OStopFixes(); 
		//		GPS_uart_deinit();
		//Turn gps off code
		gps_power_off();
		GPS_Current_State = GPS_POWER_OFF;
	}
	else
	{
		Settings_Data.GPS_power_status= GPS_ON;
		//Turn gps on code
		GPS_OFF_timer_start(GPS_OFF_TIMEOUT);	// 3 min
		GPS_Current_State = GPS_POWER_ON;

		//	halCC4000Init(); removed 	uart_init();
		resetCC4000API();
		/*GPIO for GPS*/
		gps_gpio_init();

		gps_regulator_init(); 
		gps_power_on();
		lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
		//    gpsCC4000OStopFixes();	
		gpsCC4000OStartFixes(); 
	}
	display_Settings_Main();
	Write_Data_file_information_To_Sdcard();
}

/*-----------------------------------------------------------------------------* 
 * Function:    bluetooth_toggle
 * Brief:       Turn bluetooth on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_bluetooth_toggle(void)
{

	Settings_Data.BLE_power_status = !(Settings_Data.BLE_power_status);
	display_Settings_Main();

	if(Settings_Data.BLE_power_status == BLUETOOTH_ON)
	{
		ble_transmission_status = BLUETOOTH_INITIALIZING;
		display_Bluetooth_main();
		Ble_Module_init(); 
		ble_transmission_status = BLUETOOTH_DEVICE_NOT_CONNECTED;
		BLE_host_wakeup_flag = NO;
		lwgpio_int_enable(&BLE_host_interrupt, TRUE );  	
	}
	else
	{
		lwgpio_int_enable(&BLE_host_interrupt, FALSE );
		Stop_BLE_bulk_transfer();
	}
	display_Settings_Main();
	Write_Data_file_information_To_Sdcard();
}

/*-----------------------------------------------------------------------------* 
 * Function:    multiple_tests_toggle
 * Brief:       Turn bluetooth on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_multiple_tests_toggle(void)
{
	if(multiple_tests_status == MULTIPLE_TESTS_ON)
	{
		multiple_tests_status = MULTIPLE_TESTS_OFF;

	}
	else
	{
		multiple_tests_status = MULTIPLE_TESTS_ON;

	}
	display_Settings_Main();
}





/*-----------------------------------------------------------------------------* 
 * Function:    Settings_navigate_down
 * Brief:       Increment selection in the settings screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Config_Settings_navigate_down(void)
{

	Config_settings_selection = (Config_settings_selection + 1) % SETTINGS_MENU_ITEM;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Settings_navigate_up
 * Brief:       Decrement selection in the settings screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Config_Settings_navigate_up(void)
{
	if (Config_settings_selection != 0)
		Config_settings_selection--;
}
/*-----------------------------------------------------------------------------* 
 * Function:    clear_all_user_typed_pass
 * Brief:       clear password for displaying settings screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void clear_all_user_typed_pass(void)
{

	Setings_Password_Key_X1 = 0;
	Setings_Password_Key_X2 = 0;
	Setings_Password_Key_X3 = 0;
	Setings_Password_Key_X_position = 0;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Password_navigate_right
 * Brief:       Navigate to right side in the settings password
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Password_navigate_right(void)
{
	Setings_Password_Key_X_position = (Setings_Password_Key_X_position + 1) % 3;

}
/*-----------------------------------------------------------------------------* 
 * Function:    Password_navigate_left
 * Brief:       Navigate to left side in the settings password
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Password_navigate_left(void)
{
	if (Setings_Password_Key_X_position != 0)
		Setings_Password_Key_X_position--;

}
/*-----------------------------------------------------------------------------* 
 * Function:    Password_key_up
 * Brief:       Increment 1 in password screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Password_key_up(void)
{
	volatile uint_8 * Setings_Password_Key;

	switch (Setings_Password_Key_X_position)
	{
	case 0:
		Setings_Password_Key = &Setings_Password_Key_X1;
		break;
	case 1:
		Setings_Password_Key = &Setings_Password_Key_X2;
		break;
	case 2:
		Setings_Password_Key = &Setings_Password_Key_X3;
		break;
	}

	if (*Setings_Password_Key < 9)
	{
		*Setings_Password_Key = *Setings_Password_Key + 1;
	}

}
/*-----------------------------------------------------------------------------* 
 * Function:    Password_key_down
 * Brief:       decrement 1 in password screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Password_key_down(void)
{
	volatile uint_8 * Setings_Password_Key;
	switch (Setings_Password_Key_X_position)
	{
	case 0:
		Setings_Password_Key = &Setings_Password_Key_X1;
		break;
	case 1:
		Setings_Password_Key = &Setings_Password_Key_X2;
		break;
	case 2:
		Setings_Password_Key = &Setings_Password_Key_X3;
		break;
	}
	if (*Setings_Password_Key > 0)
	{
		*Setings_Password_Key = *Setings_Password_Key - 1;
	}

}
/*-----------------------------------------------------------------------------* 
 * Function:    Check_User_Authentication
 * Brief:       Password authentication
 * Parameter:   None
 * Return:      validity of password
 -----------------------------------------------------------------------------*/
boolean Check_User_Authentication(void)
{
	if ((Setings_Password_Key
			== ((Setings_Password_Key_X1 * 100) + (Setings_Password_Key_X2 * 10)
					+ (Setings_Password_Key_X3))) && (Setings_Password_Key_X_position == 2))
		return TRUE ;
	else
		return FALSE ;

}
/*-----------------------------------------------------------------------------* 
 * Function:    Check_Ui_BackAction
 * Brief:       It check wether screen need to navigate up or do back action
 * Parameter:   None
 * Return:      validity 
 -----------------------------------------------------------------------------*/
boolean Check_Ui_BackAction(void)
{
	if (Setings_Password_Key_X_position == 0)
		return TRUE ;
	else
		return FALSE ;

}
/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_List_Key_up
 * Brief:       Decrement Snow Profiles List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_List_Key_up(void)
{
	if (Snow_Profiles_List_selection == 0)
	{
		Snow_Profiles_List_selection = Snow_Profiles_List_count - 1;

	}
	else
	{
		Snow_Profiles_List_selection--;
	}

	if(Snow_Profiles_List_selection >= NUM_OF_LIST_ITEM_IN_A_VIEW)
	{
		uint_16 a=Snow_Profiles_List_selection;
		while((a%NUM_OF_LIST_ITEM_IN_A_VIEW)!=0)
		{
			--a;
		}
		if((a % NUM_OF_LIST_ITEM_IN_A_VIEW)  ==  0)
		{
			Folder_name_count = a / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}
	}
	else
	{
		Folder_name_count=0;
	}

	Snow_Profiles_List_selection = Snow_Profiles_List_selection % Snow_Profiles_List_count;

}

/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_List_Key_down
 * Brief:       Increment Snow Profiles List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_List_Key_down(void)
{
	Snow_Profiles_List_selection++;
	if(Snow_Profiles_List_selection > 0)
	{
		if(Snow_Profiles_List_selection >= Snow_Profiles_List_count)
		{
			Snow_Profiles_List_selection = 0;
			Folder_name_count=0;
		}
		if((Snow_Profiles_List_selection % NUM_OF_LIST_ITEM_IN_A_VIEW)  ==  0)
		{
			Folder_name_count = Snow_Profiles_List_selection / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}

	}  
	if(Snow_Profiles_List_selection ==0)
	{
		Folder_name_count=0;
	}


	Snow_Profiles_List_selection = Snow_Profiles_List_selection % Snow_Profiles_List_count;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_Options_Key_up
 * Brief:       Decrement Snow Profiles options selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_Options_Key_up(void)
{

	if (Snow_profile_options_selection == SNOWPROFILEOPTIONS_DISPLACEMENT)
		Snow_profile_options_selection = NUM_OF_SNOWPROFILEOPTIONS_ITEM + SNOWPROFILEOPTIONS_DISPLACEMENT - 1;
	else
		Snow_profile_options_selection--;

	Snow_profile_options_selection = Snow_profile_options_selection % (NUM_OF_SNOWPROFILEOPTIONS_ITEM+SNOWPROFILEOPTIONS_DISPLACEMENT);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_Options_Key_down
 * Brief:       Increment Snow Profiles options selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_Options_Key_down(void)
{
	Snow_profile_options_selection++;
	Snow_profile_options_selection = Snow_profile_options_selection % (NUM_OF_SNOWPROFILEOPTIONS_ITEM+SNOWPROFILEOPTIONS_DISPLACEMENT);
	if(Snow_profile_options_selection == 0)
	{
		Snow_profile_options_selection += SNOWPROFILEOPTIONS_DISPLACEMENT;
	}
}



/*-----------------------------------------------------------------------------* 
 * Function:    Favourites_List_Key_up
 * Brief:       Decrement Favourites List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Favourites_List_Key_up(void)
{
	if (Favourites_List_selection == 0)
	{
		Favourites_List_selection = Favourites_List_count - 1;

	}
	else
	{
		Favourites_List_selection--;
	}

	if(Favourites_List_selection >= NUM_OF_LIST_ITEM_IN_A_VIEW)
	{
		uint_16 a=Favourites_List_selection;
		while((a%NUM_OF_LIST_ITEM_IN_A_VIEW)!=0)
		{
			--a;
		}
		if((a % NUM_OF_LIST_ITEM_IN_A_VIEW)  ==  0)
		{
			File_name_count = a / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}
	}
	else
	{
		File_name_count=0;
	}

	Favourites_List_selection = Favourites_List_selection % Favourites_List_count;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Favourites_List_Key_down
 * Brief:       Increment Favourites List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Favourites_List_Key_down(void)
{
	Favourites_List_selection++;
	if(Favourites_List_selection > 0)
	{
		if(Favourites_List_selection >= Favourites_List_count)
		{
			Favourites_List_selection = 0;
			File_name_count=0;
		}
		if((Favourites_List_selection % NUM_OF_LIST_ITEM_IN_A_VIEW)  ==  0)
		{
			File_name_count = Favourites_List_selection / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}

	}  
	if(Favourites_List_selection ==0)
	{
		File_name_count=0;
	}


	Favourites_List_selection = Favourites_List_selection % Favourites_List_count;
}


/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void IRDMS_Condition_Key_up(void)
{
	IRDMS_Condition_selection++;
	IRDMS_Condition_selection = IRDMS_Condition_selection % NUM_OF_IRDMS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void IRDMS_Condition_Key_down(void)
{
	if (IRDMS_Condition_selection == 0)
		IRDMS_Condition_selection = NUM_OF_IRDMS_CONDITION - 1;
	else
		IRDMS_Condition_selection--;

	IRDMS_Condition_selection = IRDMS_Condition_selection % NUM_OF_IRDMS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Pressure_Condition_Key_up(void)
{
	Pressure_Condition_selection++;
	Pressure_Condition_selection = Pressure_Condition_selection % NUM_OF_PRESSURE_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Pressure_Condition_Key_down(void)
{
	if (Pressure_Condition_selection == 0)
		Pressure_Condition_selection = NUM_OF_IRDMS_CONDITION - 1;
	else
		Pressure_Condition_selection--;

	Pressure_Condition_selection = Pressure_Condition_selection % NUM_OF_PRESSURE_CONDITION;
}


/*-----------------------------------------------------------------------------* 
 * Function:    Accelerometer_Condition_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Accelerometer_Condition_Key_up(void)
{
	Accelerometer_Condition_selection++;
	Accelerometer_Condition_selection = Accelerometer_Condition_selection % NUM_OF_ACC_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Accelerometer_Condition_Key_down(void)
{
	if (Accelerometer_Condition_selection == 0)
		Accelerometer_Condition_selection = NUM_OF_ACC_CONDITION - 1;
	else
		Accelerometer_Condition_selection--;

	Accelerometer_Condition_selection = Accelerometer_Condition_selection % NUM_OF_ACC_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Accelerometer_Condition_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Magnetometer_Condition_Key_up(void)
{
	Magnetometer_Condition_selection++;
	Magnetometer_Condition_selection = Magnetometer_Condition_selection % NUM_OF_MAG_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Magnetometer_Condition_Key_down(void)
{
	if (Magnetometer_Condition_selection == 0)
		Magnetometer_Condition_selection = NUM_OF_MAG_CONDITION - 1;
	else
		Magnetometer_Condition_selection--;

	Magnetometer_Condition_selection = Magnetometer_Condition_selection % NUM_OF_MAG_CONDITION;
}



/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ROS1_Condition_Key_up(void)
{
	ROS1_Condition_selection++;
	ROS1_Condition_selection = ROS1_Condition_selection % NUM_OF_ROS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ROS1_Condition_Key_down(void)
{
	if (ROS1_Condition_selection == 0)
		ROS1_Condition_selection = NUM_OF_ROS_CONDITION - 1;
	else
		ROS1_Condition_selection--;

	ROS1_Condition_selection = ROS1_Condition_selection % NUM_OF_ROS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ROS2_Condition_Key_up(void)
{
	ROS2_Condition_selection++;
	ROS2_Condition_selection = ROS2_Condition_selection % NUM_OF_ROS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void ROS2_Condition_Key_down(void)
{
	if (ROS2_Condition_selection == 0)
		ROS2_Condition_selection = NUM_OF_ROS_CONDITION - 1;
	else
		ROS2_Condition_selection--;

	ROS2_Condition_selection = ROS2_Condition_selection % NUM_OF_ROS_CONDITION;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_up
 * Brief:       decrement Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Mainmenu_Key_up(void)
{
	if (Main_menu_selection == 0)
		Main_menu_selection = NUM_OF_MAINMENU_ITEM - 1;
	else
		Main_menu_selection--;

	Main_menu_selection = Main_menu_selection % NUM_OF_MAINMENU_ITEM;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Mainmenu_Key_down
 * Brief:       increment Main menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Mainmenu_Key_down(void)
{
	Main_menu_selection++;
	Main_menu_selection = Main_menu_selection % NUM_OF_MAINMENU_ITEM;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Testmenu_Key_up
 * Brief:       decrement test menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Testmenu_Key_up(void)
{
	if (Test_menu_selection == 0)
		Test_menu_selection = NUM_OF_TESTMENU_ITEM - 1;
	else
		Test_menu_selection--;

	Test_menu_selection = Test_menu_selection % NUM_OF_TESTMENU_ITEM;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Testmenu_Key_down
 * Brief:       increment test menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Testmenu_Key_down(void)
{
	Test_menu_selection++;
	Test_menu_selection = Test_menu_selection % NUM_OF_TESTMENU_ITEM;
}


/*-----------------------------------------------------------------------------* 
 * Function:    Settings_Selection_Key_up
 * Brief:       decrement settings selection menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Settings_Selection_Key_up(void)
{
	if (Settings_selection_menu_selection == 0)
		Settings_selection_menu_selection = NUM_OF_SETTINGSSELECTION_ITEM - 1;
	else
		Settings_selection_menu_selection--;

	Settings_selection_menu_selection = Settings_selection_menu_selection % NUM_OF_SETTINGSSELECTION_ITEM;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Settings_Selection_Key_down
 * Brief:       increment settings selection menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Settings_Selection_Key_down(void)
{
	Settings_selection_menu_selection++;
	Settings_selection_menu_selection = Settings_selection_menu_selection % NUM_OF_SETTINGSSELECTION_ITEM;
}


/*-----------------------------------------------------------------------------* 
 * Function:    Default_Graph_View_Key_up
 * Brief:       Decrement Default Graph View selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Default_Graph_View_Key_up(void)
{
	if (Default_graph_view_selection == 0)
		Default_graph_view_selection = NUM_OF_DEFAULTGRAPHVIEW_ITEM - 1;
	else
		Default_graph_view_selection--;

	Default_graph_view_selection = Default_graph_view_selection % NUM_OF_DEFAULTGRAPHVIEW_ITEM;
}



/*-----------------------------------------------------------------------------* 
 * Function:    Default_Graph_View_Key_down
 * Brief:       Increment Default Graph View selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Default_Graph_View_Key_down(void)
{
	Default_graph_view_selection++;
	Default_graph_view_selection = Default_graph_view_selection % NUM_OF_DEFAULTGRAPHVIEW_ITEM;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Increment_Default_Graph_View_from_Settings
 * Brief:       Increment Default Graph View selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Increment_Default_Graph_View_from_Settings(void)
{
	Default_Graph_View_Key_down();
	Change_Defualt_Gragh_View();
	Write_Data_file_information_To_Sdcard();
	display_Settings_Main();
}
/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_File_List_Key_down
 * Brief:       Increment Snow Profile File List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_File_List_Key_down(void)
{
	Snow_Profiles_File_List_selection++;
	if(Snow_Profiles_File_List_selection > 0)
	{
		if(Snow_Profiles_File_List_selection >= Snow_Profiles_File_List_count)
		{
			Snow_Profiles_File_List_selection = 0;
			File_name_count=0;
			return;
		}
		else
		{
			File_name_count = Snow_Profiles_File_List_selection / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}
	}  
	else
	{
		File_name_count=0;
	}


	Snow_Profiles_File_List_selection = Snow_Profiles_File_List_selection % Snow_Profiles_File_List_count;

}

/*-----------------------------------------------------------------------------* 
 * Function:    Snow_Profiles_File_List_Key_up
 * Brief:       Decrement Snow Profile Folder List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Snow_Profiles_File_List_Key_up(void)
{

	if (Snow_Profiles_File_List_selection == 0)
	{
		if(Snow_Profiles_File_List_count >0)
		{
			Snow_Profiles_File_List_selection = Snow_Profiles_File_List_count - 1;
		}
	}
	else
	{
		Snow_Profiles_File_List_selection--;
	}

	if(Snow_Profiles_File_List_selection > NUM_OF_LIST_ITEM_IN_A_VIEW-1)
	{
		uint_16 a=Snow_Profiles_File_List_selection;
		while((a%NUM_OF_LIST_ITEM_IN_A_VIEW)!=0)
		{
			--a;
		}
		if((a % NUM_OF_LIST_ITEM_IN_A_VIEW)  ==  0)
		{
			File_name_count = a / NUM_OF_LIST_ITEM_IN_A_VIEW;
		}
	}
	else
	{
		File_name_count=0;
	}

	Snow_Profiles_File_List_selection = Snow_Profiles_File_List_selection % Snow_Profiles_File_List_count;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Settingsmenu_Key_up
 * Brief:       decrement test menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Settingsmenu_Key_up(void)
{
	if (Settings_menu_selection == 0)
		Settings_menu_selection = NUM_OF_SETTINGSMENU_ITEM - 1;
	else
		Settings_menu_selection--;

	Settings_menu_selection = Settings_menu_selection % NUM_OF_SETTINGSMENU_ITEM;
}
/*-----------------------------------------------------------------------------* 
 * Function:    Settingsmenu_Key_down
 * Brief:       increment test menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Settingsmenu_Key_down(void)
{
	Settings_menu_selection++;
	Settings_menu_selection = Settings_menu_selection % NUM_OF_SETTINGSMENU_ITEM;
}

/*-----------------------------------------------------------------------------* 
 * Function:    battery_Voltage_Check
 * Brief:       Read and check battery voltage
 * Parameter:   None
 * Return:      Battery voltage
 -----------------------------------------------------------------------------*/
float battery_Voltage_Check(void)
{
	//	uint_8 status = 5;

	uint_16 bat_volt = 0;

	//	static battery_low_check_count = 0;


	Data_header.Battery_Voltage =  Read_Battery_Capacity();
	
	Settings_Data.battery_threshold = 10.0;

	if (Data_header.Battery_Voltage < Settings_Data.battery_threshold)
	{
		printf("Battery Low\n");
		//		if(Core_clock_selection == ACQUISITION_MODE)
		//		{        	
		//			De_Init_Hw_Timer();
		//			Switch_to_idle_mode();
		//		}

		Low_battery_shutdown();
	}

	//	if (V_Bat < Settings_Data.battery_threshold)
	//	{
	//		battery_low_check_count++;
	//	}
	//	else
	//	{
	//		battery_low_check_count = 0;
	//	}
	//	//    }
	//	if (battery_low_check_count >= 1)
	//	{
	//		Battery_low_status = 1;
	//	}
	//	if (usb_test_flag == 0)
	//	{
	//		Handle_battery_related_issues();
	//	}
	//	else
	//	{
	//		battery_low_check_count = 0;
	//		Battery_adc_stucked_and_shows_zero = 0;
	//	}
	return Data_header.Battery_Voltage;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Batterry_checkon_powerup
 * Brief:       Battery check on power up
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Battery_checkon_powerup(void)
{

	uint_16 bat_volt = 0;
	float V_Bat = 0;

#if 1

	//	Enable_all_sensors();	
	//	ui_timer_start(100);
	ADC_Init();
	bat_volt = Read_Battery();
	Data_header.Battery_Voltage =  V_Bat = (bat_volt) * BATTERY_VOLTAGE_MULTIPILER;

	if (V_Bat < Settings_Data.battery_threshold)
	{

		Buzzer_Init();
		Buzzer_On();
		Buzzer_Off();

		GPS_Current_State = GPS_POWER_ON;
		ui_timer_init();
		Buzzer_Short_Beep(1);


		dispaly_battery_low();
		Kill_Controller_Init();
		while(1)
		{};
	}
	else
	{	
		ADC_deinit();
		//		Disable_all_sensors();
	}

#endif

}

/*-----------------------------------------------------------------------------* 
 * Function:    Enable_all_sensors
 * Brief:       Enable all sensors before battery check
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Enable_all_sensors(void)
{
	power_rail_init();
	power_rail_enable();
	//	power_rail_enable();
	//	ADC_Init();
	lsm303_i2c0_init();
	Start_LSM();

	gps_gpio_init();
	gps_regulator_init(); 
	gps_power_on();
	lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
	gpsCC4000OStartFixes(); 

	if((GPS_Current_State == GPS_LOCKING)||(GPS_Current_State == GPS_POWER_ON))
	{
		lwgpio_int_enable(&FIX_AVL, FALSE );
	}
	GPS_Prev_State = GPS_Current_State;
	GPS_Current_State = GPS_STANDBY;
	resetCC4000API();  
	gpsCC4000OStartFixes(); 
	GPS_uart_init();
}

/*-----------------------------------------------------------------------------* 
 * Function:    Disable_all_sensors
 * Brief:       disables all sensors before battery check
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Disable_all_sensors(void)
{
	//	ADC_deinit();
	Stop_LSM();
	Lsm303_deinit();
	gpsCC4000Off();
	//gps_disable();
	gpsCC4000OStopFixes();
	GPS_uart_deinit();
	power_rail_disable();
}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Header
 * Brief:       Create header data to frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/

void Create_Header(void)
{
	static uint_8 status = 5;

	float V_Bat = 0;
	//	char temp[16];
	//    Rect_Fill(DISPLAY_HEADER_X_MIN, DISPLAY_HEADER_Y_MIN, DISPLAY_HEADER_X_MAX,
	//            DISPLAY_HEADER_Y_MAX, COLOUR_BLACK);

	if((State_of_Screen == UI_NEW_SNOW_PROFILE_COLLECTING)||(State_of_Screen == UI_NEW_SNOW_PROFILE_CALIBRATING) || (State_of_Screen == UI_NEW_SNOW_PROFILE_PROCESSING))
	{
		;// handle with previous battery voltage status
	}
	else
	{
		Data_header.Battery_Voltage = Read_Battery_Capacity();
		status = BATTERY_HALF_FULL;
//		if(V_Bat < Battery_minimum_voltage_check) 
//		{
//			Battery_minimum_voltage_check = V_Bat;
//		}
//
//		if(Battery_minimum_voltage_check >= BATTERY_FULL_VOLTAGE)
//		{
//			status = BATTERY_FULL;
//		}
//		else if((Battery_minimum_voltage_check < BATTERY_FULL_VOLTAGE) && (Battery_minimum_voltage_check >= BATTERY_HALF_FULL_VOLTAGE))
//		{
//			status = BATTERY_HALF_FULL;
//		}
//		else
//		{
//			status = BATTERY_NEARLY_EMPTY;
//		}
	}






#if BATTERY_VALUE_DISPLAY_ENABLE 
	Draw_string_new(190, 4, temp, COLOUR_BLACK, MEDIUM_FONT);
#endif

	switch (status)
	{
	case BATTERY_FULL:     //i. Full >4.2V
		Draw_header_icons((DISPLAY_HEADER_X_MIN),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BATTERY_FULL_ICON, COLOUR_BLACK);
		break;
	case BATTERY_HALF_FULL:    //ii. Half Full 4.2V - 3.6V
		Draw_header_icons((DISPLAY_HEADER_X_MIN),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BATTERY_HALF_ICON, COLOUR_BLACK);
		break;      
	case BATTERY_NEARLY_EMPTY:   //iii. Near Empty <3.6V
		Draw_header_icons((DISPLAY_HEADER_X_MIN),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BATTERY_EMPTY_ICON, COLOUR_BLACK);
		break;
	default:
		Draw_header_icons((DISPLAY_HEADER_X_MIN),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BATTERY_EMPTY_ICON, COLOUR_BLACK);
		break;
	}

	//Get bluetooth status;
	//status=get_Bluetooth_Status();
	switch (Settings_Data.BLE_power_status)
	{
	case BLUETOOTH_ON:				//i.	Bluetooth connected
		Draw_header_icons((DISPLAY_HEADER_X_MIN+NUM_X_PIXEL_BATTERY_ICON),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BLUETOOTH_CONNECTED_ICON, COLOUR_BLACK);        
		break;
	case BLUETOOTH_OFF:	//ii.	Bluetooth ON but not connected
		break;

	default:
		Draw_header_icons((DISPLAY_HEADER_X_MIN+NUM_X_PIXEL_BATTERY_ICON),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), BLUETOOTH_CONNECTED_ICON, COLOUR_BLACK);            
		break;

	}

	//Get GPS Status
	//status=get_Gps_Status();
	switch (Settings_Data.GPS_power_status)
	{
	case GPS_ON:					//i.	GPS Linked
		//		if(bluetooth_status==BLUETOOTH_ON)
		//		{
		Draw_header_icons((DISPLAY_HEADER_X_MIN+NUM_X_PIXEL_BATTERY_ICON+NUM_X_PIXEL_BLUETOOTH_ICON),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), GPS_LINKED_ICON, COLOUR_BLACK);
		//		}
		//		else
		//		{
		//			Draw_header_icons((DISPLAY_HEADER_X_MIN+NUM_X_PIXEL_BATTERY_ICON),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), GPS_LINKED_ICON, COLOUR_BLACK);	
		//		}
		break;
	case GPS_OFF:				//ii.	GPS not Linked
		break;

	default:
		Draw_header_icons((DISPLAY_HEADER_X_MIN+NUM_X_PIXEL_BATTERY_ICON+NUM_X_PIXEL_BLUETOOTH_ICON),DISPLAY_HEADER_Y_MIN + (DISPLAY_HEADER_WIDTH / 7), GPS_LINKED_ICON, COLOUR_BLACK);
		break;
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Title
 * Brief:       Create Title data to frame buffer
 * Parameter:   Title name
 * Return:      None
 -----------------------------------------------------------------------------*/
void Create_Title(char * aString, uint16_t string_length)
{
	uint_16 title_x, title_y;

	if(string_length>18)
	{
		printf("Create_Title overflow\n");
		while(1)
		{
		}
	}

	//Find Starting pixel to print title
	title_x = (DISPLAY_X_MAX / 2) - ((string_length / 2) * NUM_X_PIXEL_PER_CHAR_MEDIUM);
	if(title_x>DISPLAY_X_MAX)
	{
		printf("\nCreate_Title buffer>240\n");
		title_x=1;    	
	}
	//    title_y = DISPLAY_TITLE_Y_MIN + (DISPLAY_TITLE_WIDTH / 14);
	title_y = DISPLAY_TITLE_Y_MIN ;

	Draw_string_new(title_x, title_y, (uint_8 *) aString, COLOUR_BLACK,MEDIUM_FONT);
}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Title
 * Brief:       Create Title data to frame buffer
 * Parameter:   Title name
 * Return:      None
 -----------------------------------------------------------------------------*/
void Create_Title_calib(char * aString, uint16_t string_length,uint_16 title_x,uint_16 title_y)
{
	//	uint_16 title_x, title_y;

	if(string_length>18)
	{
		printf("Create_Title overflow\n");
		while(1)
		{
		}
	}

	//Find Starting pixel to print title
	title_x = (DISPLAY_X_MAX / 2) - ((string_length / 2) * NUM_X_PIXEL_PER_CHAR_MEDIUM);
	if(title_x>DISPLAY_X_MAX)
	{
		printf("\nCreate_Title buffer>240\n");
		title_x=1;    	
	}

	Draw_string_new(title_x, title_y, (uint_8 *) aString, COLOUR_BLACK,MEDIUM_FONT);
}


static void Create_Title_old(char * titleString)
{

	uint_8 string_length = strlen(titleString);
	uint_16 title_x, title_y;

	//Find Starting pixel to print title
	title_x = (DISPLAY_X_MAX / 2) - ((string_length / 2) * NUM_X_PIXEL_PER_CHAR);
	//    title_y = DISPLAY_TITLE_Y_MIN + (DISPLAY_TITLE_WIDTH / 14);
	title_y = DISPLAY_TITLE_Y_MIN ;

	Draw_string(title_x, title_y, (uint_8 *) titleString, COLOUR_BLACK);

}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_Footer
 * Brief:       Create footer data to frame buffer
 * Parameter:   left menu name and right menu name
 * Return:      None
 -----------------------------------------------------------------------------*/
void Create_Footer(char * leftMenu,uint16_t left_len, char * rightMenu,uint16_t right_len)
{

	uint_16 menu_x, menu_y;

	if((left_len>8)||(right_len>8))
	{
		printf("Create_Footer overflow\n");
		while(1)
		{

		}
	}

	//Draw left menu
	menu_x = (DISPLAY_X_MAX / 4) - ((left_len / 2) * NUM_X_PIXEL_PER_CHAR) - 14;

	if(menu_x>DISPLAY_X_MAX)
	{
		printf("\nCreate_Footer Overflow Error1\n");
		menu_x=1;
	}
	menu_y = DISPLAY_FOOTER_Y_MIN + (DISPLAY_FOOTER_WIDTH / 7);
	menu_y = menu_y - 3;
	Draw_string_new(menu_x, menu_y, (uint_8 *) leftMenu, COLOUR_WHITE,MEDIUM_FONT);

	//Draw right menu
	menu_x = (3 * DISPLAY_X_MAX / 4) - ((right_len / 2) * NUM_X_PIXEL_PER_CHAR) + 13;
	if(menu_x>DISPLAY_X_MAX)
	{
		printf("\nCreate_Footer Overflow Error2\n");
		menu_x=1;
	}
	//	menu_y = DISPLAY_FOOTER_Y_MIN + (DISPLAY_FOOTER_WIDTH / 7);
	Draw_string_new(menu_x, menu_y, (uint_8 *) rightMenu, COLOUR_WHITE,MEDIUM_FONT);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Add_Item_To_Menu
 * Brief:       This will add an item in to the list
 * Parameter:   display name ,position, selection status
 * Return:      None
 -----------------------------------------------------------------------------*/
void Add_Item_To_Menu(const char *menu_data, uint_8 menu_position,
		uint_16 menu_selection_status)
{
	uint_16 y_position_min, y_position_max;
	char menu_Text[22];

	memset(menu_Text,0x00,20);
	strncpy(menu_Text,menu_data,19);
	if(strlen(menu_Text)>19)
	{
		printf("\nADD ITEM TO MENU NAME OF STRING EXCEEDED SIZE LIMIT; SIZE IS %d,\n %s", strlen(menu_Text),menu_Text);
		menu_Text[19]=0;
	}

	//
	//	if(State_of_Screen == UI_SNOW_PROFILE_LIST)
	//	{
	//		y_position_min = DISPLAY_CONTAINER_Y_MIN + 9 + (menu_position * (DISPLAY_MENU_WIDTH-8));
	//		y_position_max = DISPLAY_CONTAINER_Y_MIN + 9 + ((menu_position + 1) * (DISPLAY_MENU_WIDTH-8));
	//	}
	//	else
	//	{
	y_position_min = DISPLAY_CONTAINER_Y_MIN + (menu_position * DISPLAY_MENU_WIDTH);
	y_position_max = DISPLAY_CONTAINER_Y_MIN + ((menu_position + 1) * DISPLAY_MENU_WIDTH);
	//	}


	if (menu_selection_status == MENU_SELECTED)
	{

		//		if(State_of_Screen == UI_SNOW_PROFILE_LIST)
		//		{
		//			//Fill rect with black colour
		//			Rect_Fill(DISPLAY_CONTAINER_X_MIN, y_position_min, DISPLAY_CONTAINER_X_MAX, y_position_max,
		//					COLOUR_BLACK);
		//			Draw_string_new((DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR),
		//					(y_position_min + ((DISPLAY_MENU_WIDTH-8) / 4)), (uint_8 *) menu_Text, COLOUR_WHITE,MEDIUM_FONT);
		//		}
		//		else  
		//		{
		if((State_of_Screen == UI_SNOW_PROFILE_OPTIONS) || (State_of_Screen == UI_SET_FAVOURITE) || 
				(State_of_Screen == UI_SAVE_SLOPE) || (State_of_Screen == UI_GPS_LOCATION) ||
				(State_of_Screen == UI_VIEW_CHARTS) || (State_of_Screen == UI_DELETE_PROFILE))
		{
			Rect_fill_with_horizontal_lines(170, y_position_min-1, DISPLAY_CONTAINER_X_MAX, y_position_max,
					COLOUR_BLACK);
		}
		else
		{
			//Fill rect with black colour
			Rect_fill_with_horizontal_lines(DISPLAY_CONTAINER_X_MIN, y_position_min-1, DISPLAY_CONTAINER_X_MAX, y_position_max,
					COLOUR_BLACK);
		}

		Draw_string_new((DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR),
				(y_position_min + (DISPLAY_MENU_WIDTH / 4))-4, (uint_8 *) menu_Text, COLOUR_WHITE ,MEDIUM_FONT);
		//		}


	}
	else
	{
		if((State_of_Screen == UI_SNOW_PROFILE_OPTIONS) || (State_of_Screen == UI_SET_FAVOURITE) || 
				(State_of_Screen == UI_SAVE_SLOPE) || (State_of_Screen == UI_GPS_LOCATION) ||
				(State_of_Screen == UI_VIEW_CHARTS) || (State_of_Screen == UI_DELETE_PROFILE))
		{
			//Fill rectangle in favourites with white colour and then fill corresponding selection with black
			Rect_fill_with_horizontal_lines(170, 130, DISPLAY_CONTAINER_X_MAX, 330, COLOUR_WHITE);
			y_position_min = DISPLAY_CONTAINER_Y_MIN + (Snow_profile_options_selection * DISPLAY_MENU_WIDTH);
			y_position_max = DISPLAY_CONTAINER_Y_MIN + ((Snow_profile_options_selection + 1) * DISPLAY_MENU_WIDTH);
			Rect_fill_with_horizontal_lines(170, y_position_min-1, DISPLAY_CONTAINER_X_MAX, y_position_max, COLOUR_BLACK);
		}
		else
		{
			//Fill rect with white colour
			//            Draw_Rect(DISPLAY_CONTAINER_X_MIN, y_position_min, DISPLAY_CONTAINER_X_MAX, y_position_max,
			//                    COLOUR_BLACK);
		}



		//Display string in black colour
		//		if(State_of_Screen == UI_SNOW_PROFILE_LIST)
		//		{
		//			Draw_string_new((DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR),
		//					(y_position_min + ((DISPLAY_MENU_WIDTH-8) / 4)), (uint_8 *) menu_Text, COLOUR_BLACK,MEDIUM_FONT);
		//		}
		//		else
		//		{
		Draw_string_new((DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR),
				(y_position_min + (DISPLAY_MENU_WIDTH / 4))-4, (uint_8 *) menu_Text, COLOUR_BLACK, MEDIUM_FONT);
		//		}


	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    create_Location_informations
 * Brief:       This will add slope angle Aspect ,gps latitude and longitude in the screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void create_Location_informations(void)
{

	uint_16 x_position, y_position;

	char tempString[16];

	//for slop angle
	x_position = DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR;
	y_position = DISPLAY_CONTAINER_Y_MIN + (NUM_Y_PIXEL_PER_CHAR / 2);
	//Display Slop angle in black colour
	sprintf(tempString, "%d", Data_header.Slope_Angle);
	Draw_string(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK);

	//for Latitude

	x_position = DISPLAY_CONTAINER_X_MAX
			- (NUM_X_PIXEL_PER_CHAR * (strlen(Data_header.Gps_data.Latitude) + 2));
	y_position = DISPLAY_CONTAINER_Y_MIN + (NUM_Y_PIXEL_PER_CHAR / 2);
	//Display Latitude in black colour
	Draw_string(x_position, y_position, (uint_8 *) Data_header.Gps_data.Latitude,
			COLOUR_BLACK);

	x_position = DISPLAY_CONTAINER_X_MIN + NUM_X_PIXEL_PER_CHAR;
	y_position = DISPLAY_CONTAINER_Y_MIN + (2 * NUM_Y_PIXEL_PER_CHAR);
	//Display Aspect in black colour
	Draw_string(x_position, y_position, (uint_8 *) Data_header.Aspect, COLOUR_BLACK);

	//for Longitude

	x_position = DISPLAY_CONTAINER_X_MAX
			- (NUM_X_PIXEL_PER_CHAR * (strlen(Data_header.Gps_data.Longitude) + 2));
	y_position = DISPLAY_CONTAINER_Y_MIN + (2 * NUM_Y_PIXEL_PER_CHAR);
	//Display Longitude in black colour
	Draw_string(x_position, y_position, (uint_8 *) Data_header.Gps_data.Longitude,
			COLOUR_BLACK);

}
/*-----------------------------------------------------------------------------* 
 * Function:    select_graph
 * Brief:       This will draw graph in the frame buffer
 * Parameter:   Pixels_buffer,tempChart
 * Return:      None
 -----------------------------------------------------------------------------*/
void select_graph(uint_16 * Pixels_buff,uint_8 *tempChart)
{
	uint16_t i=0;
	for ( i = 0; i < GRAPH_RAW_PIXEL_COUNT; i++)
	{
		if(Pixels_buff[i]>200)
		{
			tempChart[i] = 200;
		}
		else
		{
			tempChart[i]=Pixels_buff[i];
		}
	}

}
///*-----------------------------------------------------------------------------* 
// * Function:    create_Snow_Profile_Chart
// * Brief:       This will draw graph in the frame buffer
// * Parameter:   None
// * Return:      None
// -----------------------------------------------------------------------------*/
//static void create_Snow_Profile_Chart(void)
//{
//	uint_16 x1_position = DISPALY_GRAPH_X_MIN, y1_position = DISPALY_GRAPH_Y_MIN;
//
//	uint_8 tempChart[GRAPH_RAW_PIXEL_COUNT];
//
//	//    graph_to_bitmap(tempChart);
//	//    Pixelated_Means(tempChart);
//
//	// endDepth in processed Data file
//	EndDepth = Pixels_buffer[5*GRAPH_RAW_PIXEL_COUNT] ;
//	
//	switch(Settings_Data.default_gragh_view)
//	{
//	case GRAPH_DETAIL_0:
//		select_graph(Pixels_buffer,tempChart);
//		break;
//	case GRAPH_DETAIL_1:
//		select_graph(Pixels_buffer+(1*GRAPH_RAW_PIXEL_COUNT),tempChart);
//		break;
//	case GRAPH_DETAIL_2:
//		select_graph(Pixels_buffer+(2*GRAPH_RAW_PIXEL_COUNT),tempChart);
//		break;
//	case GRAPH_DETAIL_3:
//		select_graph(Pixels_buffer+(3*GRAPH_RAW_PIXEL_COUNT),tempChart);
//		break;
//	case GRAPH_DETAIL_4:
//		select_graph(Pixels_buffer+(4*GRAPH_RAW_PIXEL_COUNT),tempChart);
//		break;
//	default:
//		break;
//
//
//	}
//
//	for (uint16_t i = 0; i < GRAPH_RAW_PIXEL_COUNT; i++)
//	{
//
//		Draw_Line(x1_position, y1_position + i, x1_position + tempChart[i], y1_position + i,
//				COLOUR_BLACK);
//	}
//
//	    Rect_fill_with_dithered_lines(x1_position+1,DISPALY_GRAPH_Y_MIN+ (EndDepth/5), DISPLAY_X_MAX - 6 , DISPLAY_Y_MAX - DISPLAY_FOOTER_WIDTH - 6, COLOUR_BLACK );
//
//}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_Mainmenu_Content
 * Brief:       This will main menu content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Mainmenu_Content(void)
{


	for (uint_8 i = 0; i < NUM_OF_MAINMENU_ITEM; i++)
	{
		if (i == Main_menu_selection)
		{			
			Add_Item_To_Menu(Mainmenu_Table[i].Menu_String, i, MENU_SELECTED);
			if(Calib_status[i] == COMPLETED)
			{
				Draw_string_new(210,(10 + DISPLAY_CONTAINER_Y_MIN + (i * DISPLAY_MENU_WIDTH )), "*", COLOUR_WHITE ,MEDIUM_FONT);
			}								 
		}
		else
		{
			Add_Item_To_Menu(Mainmenu_Table[i].Menu_String, i, MENU_NOT_SELECTED);
			if(Calib_status[i] == COMPLETED)
			{
				Draw_string_new(210,(10 + DISPLAY_CONTAINER_Y_MIN + (i * DISPLAY_MENU_WIDTH )), "*", COLOUR_BLACK ,MEDIUM_FONT);
			}
		}

	}

}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_MultipleTestmenu_Content
 * Brief:       This will create multiple test menu content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_MultipleTestmenu_Content(void)
{	
	switch (test_number)
	{
	case TEST_NUMBER_ONE:
		Create_Title("NEW TEST 1/3",strlen("NEW TEST 1/3"));	
		Create_Footer("CANCEL",strlen("CANCEL"),"SLOPE",strlen("SLOPE"));
		test_number=TEST_NUMBER_TWO;
		break;

	case TEST_NUMBER_TWO:  
		Create_Title("NEW TEST 2/3",strlen("NEW TEST 2/3"));	
		Create_Footer("CANCEL",strlen("CANCEL"),"",0);
		Rect_Fill(50,60,190,220,COLOUR_WHITE);
		Draw_favourites_icons(10, 150, CIRCLE_BLACK, COLOUR_BLACK);
		Draw_favourites_icons(70, 150, CIRCLE_GREY, COLOUR_BLACK);
		Draw_favourites_icons(130, 150, CIRCLE_GREY, COLOUR_BLACK);
		test_number=TEST_NUMBER_THREE;

		break;

	case TEST_NUMBER_THREE:
		Create_Title("NEW TEST 3/3",strlen("NEW TEST 3/3"));
		Create_Footer("CANCEL",strlen("CANCEL"),"",0);
		Rect_Fill(50,60,190,220,COLOUR_WHITE);
		Draw_favourites_icons(10, 150, CIRCLE_BLACK, COLOUR_BLACK);
		Draw_favourites_icons(70, 150, CIRCLE_BLACK, COLOUR_BLACK);
		Draw_favourites_icons(130, 150, CIRCLE_GREY, COLOUR_BLACK);
		test_number=TEST_NUMBER_ONE;

		break;
	}

}



/*-----------------------------------------------------------------------------* 
 * Function:    Change_Defualt_Gragh_View
 * Brief:       Changes default gragh view after testing
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Change_Defualt_Gragh_View(void)
{
	Settings_Data.default_gragh_view = Default_graph_view_selection; 
}




/*-----------------------------------------------------------------------------* 
 * Function:    Create_favourites_menu_Content
 * Brief:       This will add favourites menu screen content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_favourites_menu_Content(void)
{

	uint_16 display_list, temp_i, i = 0, j=0, cnt=0;
	//char Test_Name[24];

	Favourites_List_count = Favourites_file_count;
	display_list = (Favourites_List_selection / NUM_OF_LIST_ITEM_IN_A_VIEW);
	temp_i = (display_list * NUM_OF_LIST_ITEM_IN_A_VIEW) + i;

	for (i = 0; i < NUM_OF_LIST_ITEM_IN_A_VIEW; i++)
	{
		display_list = (Favourites_List_selection / NUM_OF_LIST_ITEM_IN_A_VIEW);
		temp_i = (display_list * NUM_OF_LIST_ITEM_IN_A_VIEW) + i;
		if (temp_i >= Favourites_List_count)
		{
			break;
		}
		cnt=i+(File_name_count*NUM_OF_LIST_ITEM_IN_A_VIEW);

		if (i == (Favourites_List_selection % NUM_OF_LIST_ITEM_IN_A_VIEW))
		{
			// Add_Item_To_Menu(Snow_Profiles_List_Text[temp_i],i,MENU_SELECTED);	
			//            sprintf(Test_Name, FavouriteEntry[i+(File_name_count*NUM_OF_LIST_ITEM_IN_A_VIEW)].Favourite_Directory, NULL);
			//            Folder_name_index= (i+(File_name_count*NUM_OF_LIST_ITEM_IN_A_VIEW));
			Add_Item_To_Menu(FavouriteEntry[cnt].Favourite_Directory, i, MENU_SELECTED);
		}
		else
		{
			//	 Add_Item_To_Menu(Snow_Profiles_List_Text[temp_i],i,MENU_NOT_SELECTED);	
			//            sprintf(Test_Name, FavouriteEntry[i+(File_name_count*NUM_OF_LIST_ITEM_IN_A_VIEW)].Favourite_Directory, NULL);
			Add_Item_To_Menu(FavouriteEntry[cnt].Favourite_Directory, i, MENU_NOT_SELECTED);
		}
		//Code to Draw the line after each day in favourites menu list.
		if(cnt<Favourites_file_count - 1)
		{
			if(strcmp(FavouriteEntry[cnt].Favourite_Folder_Name, FavouriteEntry[cnt+1].Favourite_Folder_Name) != 0)
			{
				draw_line_horizontal(1,240,DISPLAY_CONTAINER_Y_MIN + ((i + 1) * (DISPLAY_MENU_WIDTH)),COLOUR_BLACK);
			}
		}
	}

}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_about_device_Content
 * Brief:       This will add device details screen content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_about_device_Content(void)
{

	uint_16 x_position, y_position;
	char  serilal_no[20] = {'\0'};

	sprintf(serilal_no,"SN: %s",Device_id);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen("AVATECH SP1")) / 2) * NUM_X_PIXEL_PER_CHAR);

	y_position = 140;
	Draw_string_new(x_position, y_position, (uint_8 *) "AVATECH SP1", COLOUR_BLACK, MEDIUM_FONT);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen(serilal_no)) / 2) * NUM_X_PIXEL_PER_CHAR);

	y_position = 190;
	Draw_string_new(x_position, y_position, serilal_no, COLOUR_BLACK, MEDIUM_FONT);

	x_position = (DISPLAY_X_MAX / 2) - 10 - (((strlen(Software_ver)) / 2) * NUM_X_PIXEL_PER_CHAR);

	y_position = 240;
	Draw_string_new(x_position, y_position, (uint_8 *) Software_ver, COLOUR_BLACK, MEDIUM_FONT);


}



/*-----------------------------------------------------------------------------* 
 * Function:    Create_Slope_Measurement_Content
 * Brief:       This will Create Slope Measurement screen Content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Slope_Measurement_Content(void)
{
	uint_16 x_position, y_position, string_length;
	char tempString[16];
	char aspect[4], list_num[5];
	uint_16 magnetic_heading = 0xffff;
	int_16 tempAngle = 0xffff;
	memset(tempString,0x00,16);
	get_slope_measurement(&tempAngle, aspect, &magnetic_heading);


	if (magnetic_heading != 0xffff)
	{
		sprintf(tempString, "%d", tempAngle);
	}

	string_length = strlen(tempString);

	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/8);

	x_position = (DISPLAY_X_MAX / 2) - ((string_length / 2) * NUM_X_PIXEL_PER_CHAR_MEDIUM);
	y_position = y_position + (3 * NUM_Y_PIXEL_PER_CHAR_MEDIUM);
	Draw_string_new(x_position, y_position,(uint_8 *) tempString, COLOUR_BLACK, LARGE_FONT);

	Draw_favourites_icons(x_position-(NUM_X_PIXEL_PER_CHAR_MEDIUM * 2),y_position+10, ANGLE_ICON, COLOUR_BLACK );

	y_position = y_position + (5 * NUM_Y_PIXEL_PER_CHAR_MEDIUM);

	Draw_string_new(x_position, y_position,(uint_8 *) aspect, COLOUR_BLACK, LARGE_FONT);
	Draw_favourites_icons(x_position-(NUM_X_PIXEL_PER_CHAR_MEDIUM * 2),y_position+10, ASPECT_ICON, COLOUR_BLACK );

	sprintf(list_num, "%d", Sd_card_file_count-File_name_index);
	Draw_string_new(120, 2, list_num, COLOUR_BLACK, MEDIUM_FONT);
}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_Sensor_Output_Content
 * Brief:       This will Create Sensor output screen Content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Sensor_Output_Content(void)
{
	uint_16 x_position, y_position;
	char tempString[16];
	char aspect[4];
	uint_16 magnetic_heading = 0xffff;
	int_16 tempAngle = 0xfff;

	float tempForce = 0xffff, tempDepth = 0xffff;
	uint_16 raw_pressure = 0, raw_depth = 0,tempQrd = 0xffff;
	Test_Gps_sensor_output();
	sprintf(tempString, "NA");

	GetSensor_Output(&tempForce, &tempDepth, &tempQrd, &raw_pressure, &raw_depth);
	get_slope_measurement(&tempAngle, aspect, &magnetic_heading);


	x_position =12;
	y_position = DISPLAY_CONTAINER_Y_MIN -(20)+ ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/8);
	Draw_string_new(x_position, y_position, (uint_8 *) "PRESSURE:", COLOUR_BLACK, MEDIUM_FONT);


	sprintf(tempString, "%d ", (raw_pressure ));
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);


	Draw_string_new(x_position, (y_position), (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

	x_position =12;
	y_position = y_position + 10 +(2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "DEPTH:", COLOUR_BLACK, MEDIUM_FONT);


	sprintf(tempString, "%d ", (raw_depth));
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);


	Draw_string_new(x_position, (y_position), (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

	x_position =12;
	y_position = y_position + 10 + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "EYE:", COLOUR_BLACK, MEDIUM_FONT);

	if (tempQrd != 0xffff)
		sprintf(tempString, "%d ", (tempQrd ));
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

	sprintf(tempString, "NA");
	x_position =12;
	y_position = y_position + 10 + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "TILT:", COLOUR_BLACK, MEDIUM_FONT);


	if (tempAngle != 0xffff)
		sprintf(tempString, "%d° ", tempAngle);
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

	x_position =12;
	y_position = y_position + 10 + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "ASPECT:", COLOUR_BLACK, MEDIUM_FONT);


	sprintf(tempString, "NA");
	if (magnetic_heading != 0xffff)
	{
		magnetic_heading = 360 - magnetic_heading;
		sprintf(tempString, "%d° %s ", magnetic_heading, aspect);
	}

	x_position = (DISPLAY_X_MAX -20) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

	x_position =12;
	y_position = y_position + 10 + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) "GPS:", COLOUR_BLACK, MEDIUM_FONT);

	if (tempAngle != 0xffff)
		sprintf(tempString, "%s ", Data_header.Gps_data.Latitude);
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);


	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR)-10;

	if (tempAngle != 0xffff)
		sprintf(tempString, "%s ", Data_header.Gps_data.Longitude);
	x_position = (DISPLAY_X_MAX -12) - (strlen(tempString)* NUM_X_PIXEL_PER_CHAR);
	Draw_string_new(x_position, y_position, (uint_8 *) tempString, COLOUR_BLACK, MEDIUM_FONT);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Bluetooth_main_Content
 * Brief:       This will Create bluetooth main screen Content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Bluetooth_main_Content(void)
{
	uint_16 x_position, y_position;    

	if(ble_transmission_status == BLUETOOTH_INITIALIZING)
	{
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("INITIALIZING")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "INITIALIZING", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("BLUETOOTH ")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "BLUETOOTH ", COLOUR_BLACK);

		//        x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE WAIT")) / 2) * NUM_X_PIXEL_PER_CHAR);
		//        y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		//        Draw_string(x_position, y_position, (uint_8 *) "PLEASE WAIT", COLOUR_BLACK);

		Create_Footer("",0, "",0);
	}
	else if(ble_transmission_status == BLUETOOTH_DEVICE_NOT_CONNECTED)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("THE DEVICE IS NOW")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "THE DEVICE IS NOW", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("DISCOVERABLE.")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "DISCOVERABLE.", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE CONNECT")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "PLEASE CONNECT", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2)
    		   - (((strlen("YOUR DEVICE")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "YOUR DEVICE", COLOUR_BLACK);

		Create_Footer("BACK",strlen("BACK"), "",0);
	}
	else if(ble_transmission_status == BLUETOOTH_DEVICE_CONNECTED)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("DEVICE IS NOW")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "DEVICE IS NOW", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("CONNECTED.")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "CONNECTED.", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("SEND QUERY LIST")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "SEND QUERY LIST", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2)
    		   - (((strlen("FROM APP")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "FROM APP", COLOUR_BLACK);

		Create_Footer("BACK",strlen("BACK"),"",0);
	}
	//    else if(ble_transmission_status == BLUETOOTH_QUERY_LIST_COMPLETED)
	//    {
	////     x_position = (DISPLAY_X_MAX / 2) - (((strlen("SENDING QUERY")) / 2) * NUM_X_PIXEL_PER_CHAR);
	//     y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
	////     Draw_string(x_position, y_position, (uint_8 *) "SENDING QUERY", COLOUR_BLACK);
	//     
	//     //MANIFEST NAME
	//     x_position = (DISPLAY_X_MAX / 2) - (((strlen("SENDING QUERY LIST")) / 2) * NUM_X_PIXEL_PER_CHAR);
	//     y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	//     Draw_string(x_position, y_position, (uint_8 *) "SENDING QUERY LIST", COLOUR_BLACK);
	//     
	//     x_position = (DISPLAY_X_MAX / 2) - (((strlen((const char *)(Query_list_name + 3))) / 2) * NUM_X_PIXEL_PER_CHAR);
	//     y_position = y_position + (3 * NUM_Y_PIXEL_PER_CHAR);
	//     Draw_string(x_position, y_position, (uint_8 *) (Query_list_name + 3), COLOUR_BLACK);
	//   
	//     
	//     Create_Footer("BACK",strlen("BACK"), "",0);
	//    }
	else if(ble_transmission_status == BLUETOOTH_WAIT_FOR_SYNC)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("WAITING FOR TEST")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "WAITING FOR TEST", COLOUR_BLACK);

		//MANIFEST NAME
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("FILE REQ LIST")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "FILE REQ LIST", COLOUR_BLACK);

		//     x_position = (DISPLAY_X_MAX / 2) - (((strlen("SENDING FILE")) / 2) * NUM_X_PIXEL_PER_CHAR);
		//     y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		//     Draw_string(x_position, y_position, (uint_8 *) "SENDING FILE", COLOUR_BLACK);
		//   

		Create_Footer("BACK",strlen("BACK"), "",0);
	}
	else if(ble_transmission_status == BLUETOOTH_SYNC_FILES)
	{

		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		//MANIFEST NAME
		//     y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);    
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("SYNCING")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (3 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "SYNCING", COLOUR_BLACK);


		Create_Footer("CANCEL",strlen("CANCEL"), "",0);
	}
	else if(ble_transmission_status == BLUETOOTH_TRANSFER_COMPLETED)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("DATA TRANSFER")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "DATA TRANSFER", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("COMPLETE!")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "COMPLETE!", COLOUR_BLACK);

		Create_Footer("",0, "ACCEPT",strlen("ACCEPT"));
	}
	else if(ble_transmission_status == BLUETOOTH_TRANSFER_FAILED)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("DATA TRANSFER")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "DATA TRANSFER", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("FAILED!")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "FAILED!", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE TRY")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "PLEASE TRY", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2)
    		   - (((strlen("AGAIN.")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "AGAIN.", COLOUR_BLACK);


		Create_Footer("CANCEL",strlen("CANCEL"), "RETRY",strlen("RETRY"));
	}
	else if(ble_transmission_status == BLUETOOTH_TURNED_OFF)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE TURN")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "PLEASE TURN", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("ON BLUETOOTH")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "ON BLUETOOTH", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("TO SEND")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "TO SEND", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2)
    		   - (((strlen("THE FILE")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "THE FILE", COLOUR_BLACK); 

		Create_Footer("BACK",strlen("BACK"), "",0);
	}  
	else if(ble_transmission_status == BLUETOOTH_QRY_LIST_NOT_CREATED)
	{
		x_position = (DISPLAY_X_MAX / 2) - (((strlen("QUERY LIST IS")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/6);
		Draw_string(x_position, y_position, (uint_8 *) "QUERY LIST IS", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("EMPTY.")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "EMPTY.", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE RUN")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (4 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "PLEASE RUN", COLOUR_BLACK);

		x_position = (DISPLAY_X_MAX / 2)
    	    				   - (((strlen("SOME TEST")) / 2) * NUM_X_PIXEL_PER_CHAR);
		y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
		Draw_string(x_position, y_position, (uint_8 *) "SOME TEST", COLOUR_BLACK);

		Create_Footer(" ",strlen(" "),"",0);
	}  

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_New_Snow_Profile_Collecting_Content
 * Brief:       This will Create New Snow Profile Collecting screen Content in frame buffer
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_New_Snow_Profile_Collecting_Content(void)
{
	uint_16 x_position, y_position;

	//    static uint_8 tempCount = 0;

	x_position = (DISPLAY_X_MAX / 2) - 20- (((strlen("COLLECTING DATA")) / 2) * NUM_X_PIXEL_PER_CHAR);

	y_position = 150;
	Draw_string_new(x_position, y_position, (uint_8 *) "COLLECTING DATA", COLOUR_BLACK, MEDIUM_FONT);


	x_position = (DISPLAY_X_MAX / 2) - 10;

	switch (tempCount)
	{
	case 4:
		//            y_position = 270;
		//            Rect_Fill(x_position, y_position, x_position + 20, y_position + 20, COLOUR_BLACK);
		Draw_loading_image(48, 180, LOADING_IMAGE_ALL_BLACK, COLOUR_BLACK);
		break;

	case 3:
		//            y_position = 270;
		//            Rect_Fill(x_position, y_position, x_position + 20, y_position + 20, COLOUR_BLACK);
		Draw_loading_image(48, 180, LOADING_IMAGE_THREE_BLACK, COLOUR_BLACK);
		break;

	case 2:
		//            y_position = 270 - 40;
		//            Rect_Fill(x_position, y_position, x_position + 20, y_position + 20, COLOUR_BLACK);
		Draw_loading_image(48, 180, LOADING_IMAGE_TWO_BLACK, COLOUR_BLACK);
		break;
	case 1:
		//            y_position = 270 - 80;
		//            Rect_Fill(x_position, y_position, x_position + 20, y_position + 20, COLOUR_BLACK);
		Draw_loading_image(48, 180, LOADING_IMAGE_ONE_BLACK, COLOUR_BLACK);
		break;
	case 0:
		Draw_loading_image(48, 180, LOADING_IMAGE_NO_BLACK, COLOUR_BLACK);
		break;
	default:
		tempCount = 0;
		break;

	}
	tempCount++;
	tempCount = tempCount % 5;

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_Mainmenu
 * Brief:       This will display Main menu screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Mainmenu(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	//	Battery_ADC_Init();
	//	Create_Header();
	//	Battery_ADC_Deinit();

	Create_Title_calib(Serial_Numbr,strlen(Serial_Numbr),0,2);
	Create_Title_calib(" CALIBRATION MENU",strlen(" CALIBRATION MENU"),0,22);
	Create_Mainmenu_Content();
	Create_Footer( "",0, "SELECT",strlen("SELECT"));

	//    Draw_Image_on_Buffer((uint_8 *) bluetooth_icon);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_MultipleTestMenu
 * Brief:       This will display Multiple Test menu screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_MultipleTestMenu(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) test_button_background);
	Create_Header();
	Create_MultipleTestmenu_Content();

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Testmenu
 * Brief:       This will display test menu screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Testmenu(void)
{
	if(multiple_tests_status == MULTIPLE_TESTS_ON)
	{
		State_of_Screen = UI_NEW_MULTIPLE_TEST_LIST;
		display_MultipleTestMenu();
		return;	
	}

	buff_clear();  
	Draw_Image_on_Buffer((uint_8 *) test_button_background);
	Create_Header();
	Create_Title("NEW TEST",strlen("NEW TEST"));
	//	Create_Testmenu_Content();
	Create_Footer("BACK",strlen("BACK"), "SLOPE",strlen("SLOPE"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_favourites_menu
 * Brief:       This will display favourites menu screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_favourites_menu(void)
{

	buff_clear();

	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("FAVORITES",strlen("FAVORITES"));
	Create_favourites_menu_Content();
	Create_Footer("MENU",strlen("MENU"), "SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}


/*-----------------------------------------------------------------------------* 
 * Function:    display_Snow_Profiles_List
 * Brief:       This will display snow profile list screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Snow_Profiles_List(void)
{
	buff_clear();

	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("SNOW PROFILES",strlen("SNOW PROFILES"));
	//	Create_Snow_Profiles_List_Content();
	Create_Footer("BACK",strlen("BACK"), "SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Snow_Profiles_File_List
 * Brief:       This will display snow profile File list screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Snow_Profiles_File_List(void)
{
	buff_clear();
	char Time_stamp[16];
	uint_8 cnt=Folder_name_index;
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();

	sprintf(Time_stamp, "%c%c/%c%c/%c%c%c%c", Snow_Profile_Directory[cnt][4],Snow_Profile_Directory[cnt][5],
			Snow_Profile_Directory[cnt][6],Snow_Profile_Directory[cnt][7],
			Snow_Profile_Directory[cnt][0],Snow_Profile_Directory[cnt][1],
			Snow_Profile_Directory[cnt][2],Snow_Profile_Directory[cnt][3]);

	Create_Title(Time_stamp,strlen(Time_stamp));
	//	Create_Snow_Profiles_File_List_Content();
	Create_Footer("BACK",strlen("BACK"), "SELECT",strlen("SELECT"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}


/*-----------------------------------------------------------------------------* 
 * Function:    display_Snow_Profiles_Options
 * Brief:       This will display snow profile options screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Snow_Profiles_Options(void)
{

	char tempstring[3];

	display_Snow_Profile_Details();
	//	Create_Snowprofileoptions_Content();

	// Draw the icons in the rectangle

	Draw_favourites_icons(200, 143, FAV_ICON, COLOUR_WHITE);
	Draw_favourites_icons(190, 180, ANGLE_ICON, COLOUR_WHITE);
	Draw_favourites_icons(210, 180, ASPECT_ICON , COLOUR_WHITE);
	Draw_favourites_icons(188, 220, LOCATION_ICON, COLOUR_WHITE);
	Draw_favourites_icons(190, 260, VIEW_ICON, COLOUR_WHITE);
	sprintf(tempstring, "%d", Settings_Data.default_gragh_view + 1);
	Draw_string_new(219, 254, (uint_8*) tempstring, COLOUR_WHITE, MEDIUM_FONT);
	Draw_favourites_icons(200, 298, DELETE_ICON, COLOUR_WHITE);


	//	 Draw_Rect(170,129,240,329,COLOUR_BLACK);
	draw_line_horizontal(200,240,130,COLOUR_BLACK);
	draw_line_horizontal(200,240,129,COLOUR_BLACK);
	draw_line_horizontal(200,240,128,COLOUR_BLACK);
	draw_line_horizontal(200,240,328,COLOUR_BLACK);
	draw_line_horizontal(200,240,329,COLOUR_BLACK);
	draw_line_horizontal(200,240,330,COLOUR_BLACK);
	draw_line_vertical(170,156,300,COLOUR_BLACK);
	draw_line_vertical(169,156,300,COLOUR_BLACK);
	draw_line_vertical(171,156,300,COLOUR_BLACK);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    dispaly_Snow_Profile_Details
 * Brief:       This will display snow profile details screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Snow_Profile_Details(void)
{

	buff_clear();

	Draw_Image_on_Buffer((uint_8 *) chart_image);

	Create_Header();

	//	if((State_of_Screen==UI_SNOW_PROFILE_DETAILS)||(State_of_Screen==UI_SET_FAVOURITE))
	//	{
	//
	//	//	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	//	}
	//		Create_Snow_Profile_Details_Content();
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Snow_Profiles_Delete
 * Brief:       This will display snow profile delete screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Snow_Profiles_Delete(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Draw_string_new(60, 150, (uint_8 *) "DELETE TEST?", COLOUR_BLACK, MEDIUM_FONT);

	char Time_Stamp[32], dummy_file_name[16], dummy_folder_name[16];


	sprintf(dummy_file_name, " %c%c:%c%c%c", Snow_Profile_List_Data[File_name_index][1],
			Snow_Profile_List_Data[File_name_index][2],Snow_Profile_List_Data[File_name_index][3],
			Snow_Profile_List_Data[File_name_index][4],Snow_Profile_List_Data[File_name_index][5]);


	sprintf(dummy_folder_name, "%c%c/%c%c/%c%c", Snow_Profile_Directory[Folder_name_index][4],Snow_Profile_Directory[Folder_name_index][5],
			Snow_Profile_Directory[Folder_name_index][6],Snow_Profile_Directory[Folder_name_index][7],
			Snow_Profile_Directory[Folder_name_index][2],Snow_Profile_Directory[Folder_name_index][3]);
	sprintf(Time_Stamp, dummy_folder_name);

	sprintf(Time_Stamp, "%s%s", dummy_folder_name, dummy_file_name);
	//Code to add the star icon to the favourite menu.
	uint_8 j=0;
	if(Favourites_file_count>0)
	{
		for(j=0; j<Favourites_file_count; j++)
		{
			if(strcmp(FavouriteEntry[j].Favourite_File_Name+1,Snow_Profile_File_Name[File_name_index]+1)==0)
			{
				strcat(Time_Stamp, "*");
				j=0;
				break;
			}
		}
	}       
	Create_Title(Time_Stamp,strlen(Time_Stamp));
	Create_Footer("DELETE",strlen("DELETE"), "CANCEL",strlen("CANCEL"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}


/*-----------------------------------------------------------------------------* 
 * Function:    delete_snow_profile_selected_from_favourite
 * Brief:       This will delete the snow profile dir name from favourites list if present there.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void delete_snow_profile_selected_from_favourite(void)
{

	uint_8 count=0;
	while(count<Favourites_file_count)
	{
		if(strcmp(FavouriteEntry[count].Favourite_File_Name+1,Snow_Profile_File_Name[File_name_index]+1)==0)
		{
			printf("\nDeleted Fav Directory %s\n", FavouriteEntry[count].Favourite_Directory);
			--Favourites_file_count;
			for(;count<Favourites_file_count;count++)
			{
				//				sprintf(FavouriteEntry[count].Favourite_Directory, 		FavouriteEntry[count+1].Favourite_Directory);
				//				sprintf(FavouriteEntry[count].Favourite_Folder_Name, 	FavouriteEntry[count+1].Favourite_Folder_Name);
				//				sprintf(FavouriteEntry[count].Favourite_File_Name, 		FavouriteEntry[count+1].Favourite_File_Name);
				//				sprintf(FavouriteEntry[count].Favourite_File_Date, 		FavouriteEntry[count+1].Favourite_File_Date);
				//				FavouriteEntry[count].Favourite_Folder_Index 	=		FavouriteEntry[count+1].Favourite_Folder_Index;
				//				FavouriteEntry[count].Favourite_File_Index 		= 		FavouriteEntry[count+1].Favourite_File_Index;
				FavouriteEntry[count] = FavouriteEntry[count+1];
			}
			for(count = 0; count <= Favourites_file_count; count++)
			{
				FavouriteEntry[count].Favourites_Dir_Count = Favourites_file_count;
			}
			break;
		}
		++count;
	}
	Write_Favourite_file_information_To_Sdcard();
}   


/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_Waiting_for_file_Content
 * Brief:       This will create waiting for file screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_dispaly_Waiting_for_file_Content(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE WAIT")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/3);
	Draw_string(x_position, y_position, (uint_8 *) "PLEASE WAIT", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("WHILE READING")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "WHILE READING", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("THE RECORD")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "THE RECORD", COLOUR_BLACK);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_Waiting_for_file_Content
 * Brief:       This will display waiting for file screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void dispaly_Waiting_for_file(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("READING RECORD",strlen("READING RECORD"));
	Create_dispaly_Waiting_for_file_Content();
	Create_Footer("",0, "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_sdcard_insert
 * Brief:       This will create sd card insert screen 
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_dispaly_sdcard_insert(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE INSERT")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/3);
	Draw_string(x_position, y_position, (uint_8 *) "PLEASE INSERT", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("A VALID SDCARD")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "A VALID SDCARD", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("TO THE DEVICE")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "TO THE DEVICE", COLOUR_BLACK);
}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_rebooting_content
 * Brief:       This will create reboot screen content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_dispaly_rebooting_content(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("REBOOTING...")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/3);
	Draw_string(x_position, y_position, (uint_8 *) "REBOOTING...", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("PLEASE WAIT...")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "PLEASE WAIT...", COLOUR_BLACK);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_record_not_found_content
 * Brief:       This will create record not found content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_record_not_found_content(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("SORRY..")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/3);
	Draw_string(x_position, y_position, (uint_8 *) "SORRY..", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("UNABLE TO READ")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "UNABLE TO READ", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("REQUESTED RECORD")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "REQUESTED RECORD", COLOUR_BLACK);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_battery_low
 * Brief:       This will create battery low screen content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_dispaly_battery_low(void)
{
	uint_16 y_position, cnt = 21;

	y_position = DISPLAY_CONTAINER_Y_MIN + (DISPLAY_MENU_WIDTH*2);
	Draw_string((DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length("BATTERY IS LOW.", MEDIUM_FONT))/2 - 10,
			y_position, (uint_8 *) "BATTERY IS LOW.", COLOUR_BLACK);


	y_position = DISPLAY_CONTAINER_Y_MIN + (DISPLAY_MENU_WIDTH*3);
	Draw_string((DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length("INSERT NEW", MEDIUM_FONT))/2 - 10,
			y_position, (uint_8 *) "INSERT NEW", COLOUR_BLACK);

	y_position += cnt;
	Draw_string((DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length("BATTERIES BEFORE", MEDIUM_FONT))/2 - 10,
			y_position, (uint_8 *) "BATTERIES BEFORE", COLOUR_BLACK);

	y_position += cnt;
	Draw_string((DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length("NEXT TEST", MEDIUM_FONT))/2 - 10,
			y_position, (uint_8 *) "NEXT TEST", COLOUR_BLACK);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_display_Mass_Storage_Connected_Content
 * Brief:       This will create mass storage connected screen content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_display_Mass_Storage_Enabled_Content(void)
{

	Draw_string_new(50, 170, (uint_8 *) "MASS STORAGE", COLOUR_BLACK, MEDIUM_FONT);

	Draw_string_new(80, 220, (uint_8 *) "ENABLED", COLOUR_BLACK, MEDIUM_FONT);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_dispaly_Usb_Connected_Content
 * Brief:       This will create usb connected screen content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_dispaly_Usb_Connected_Content(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("DEVICE")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/3);
	Draw_string(x_position, y_position, (uint_8 *) "DEVICE", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("CONNECTED WITH PC")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "CONNECTED WITH PC", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("AS MASS STORAGE")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "AS MASS STORAGE", COLOUR_BLACK);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Create_Reboot_confirm_Content
 * Brief:       This will create reboot confirm screen content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Reboot_confirm_Content(void)
{
	uint_16 x_position, y_position;

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("ARE YOU SURE YOU")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = DISPLAY_CONTAINER_Y_MIN + ((DISPLAY_CONTAINER_Y_MAX - DISPLAY_CONTAINER_Y_MIN)/4);
	Draw_string(x_position, y_position, (uint_8 *) "ARE YOU SURE YOU", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("WANT TO REBOOT")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "WANT TO REBOOT", COLOUR_BLACK);

	x_position = (DISPLAY_X_MAX / 2) - (((strlen("THIS DEVICE?")) / 2) * NUM_X_PIXEL_PER_CHAR);
	y_position = y_position + (2 * NUM_Y_PIXEL_PER_CHAR);
	Draw_string(x_position, y_position, (uint_8 *) "THIS DEVICE?", COLOUR_BLACK);

}

/*-----------------------------------------------------------------------------* 
 * Function:    dispaly_Usb_Connected
 * Brief:       This will display usb connected screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void dispaly_Usb_Connected(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("USB MASS STORAGE",strlen("USB MASS STORAGE"));
	Create_dispaly_Usb_Connected_Content();
	Create_Footer("",0, "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    dispaly_Bluetooth_main
 * Brief:       This will display bluetooth main screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Bluetooth_main(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("BLUETOOTH",strlen("BLUETOOTH"));
	Create_Bluetooth_main_Content();
	//    Create_Footer("BACK", "SEND");

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}


/*-----------------------------------------------------------------------------* 
 * Function:    display_mass_storage_enable
 * Brief:       This will display sensor output screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_mass_storage_enable(void)
{
	Refresh_Lcd_Buffer((uint_8 *) Mass_Storage_Conn);
	//	buff_clear();
	//	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	//	Create_Header();
	//	Create_Title("MASS STORAGE ON",strlen("MASS STORAGE ON"));
	//
	//	Create_display_Mass_Storage_Enabled_Content();
	//
	//	Create_Footer("",0, "",0);
	//
	//	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_Config_Settings
 * Brief:       This will display sensor output screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Config_Settings(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("CONFIG SETTINGS",strlen("CONFIG SETTINGS"));
	//	Create_Settings_Main_Content();
	Create_Footer("BACK",strlen("BACK"), "SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Sensor_Output
 * Brief:       This will display sensor output screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Settings_Selection(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("SETTINGS SEL",strlen("SETTINGS SEL"));
	//	Create_Settings_Selection_Content();
	Create_Footer("BACK",strlen("BACK"), "SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Sensor_Output
 * Brief:       This will display sensor output screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Sensor_Output(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	Create_Header();
	Create_Title("SENSOR OUTPUT",strlen("SENSOR OUTPUT"));
	Create_Sensor_Output_Content();
	Create_Footer("BACK",strlen("BACK"), "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

///*-----------------------------------------------------------------------------* 
// * Function:    display_default_graph_view
// * Brief:       This will display default graph view
// * Parameter:   None
// * Return:      None
// -----------------------------------------------------------------------------*/
//void display_default_graph_view(void)
//{
//	buff_clear();
//	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
//	Create_Header();
//	Create_Title("GRAPH DETAIL",strlen("GRAPH DETAIL"));
//	Create_Default_Graph_View_Content();
//	Create_Footer("BACK",strlen("BACK"), "SELECT",strlen("SELECT"));
//	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
//}


/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile
 * Brief:       This will display new snow profile screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) test_button_background);
	Create_Header();
	Create_Title("NEW SNOW PROFILE",strlen("NEW SNOW PROFILE"));
	//	Create_New_Snow_Profile_Content();
	Create_Footer("BACK",strlen("BACK"), "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile_collecting
 * Brief:       This will display new snow profile collecting screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile_collecting(void)
{
	switch(Test_disp_ctr)
	{
	case 0:
		Draw_Image_on_Buffer((uint_8 *) Collecting_Image0);
		Test_disp_ctr = 1;
		break;

	case 1:
		Draw_Image_on_Buffer((uint_8 *) Collecting_Image1);
		Test_disp_ctr = 2;
		break;

	case 2:
		Draw_Image_on_Buffer((uint_8 *) Collecting_Image2);
		Test_disp_ctr = 3;
		break;

	case 3:
		Draw_Image_on_Buffer((uint_8 *) Collecting_Image3);
		Test_disp_ctr = 4;
		break;

	case 4:
		Draw_Image_on_Buffer((uint_8 *) Collecting_Image4);
		Test_disp_ctr = 0;
		break;

	default:
		break;
	}
	Create_Header();
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

void Update_Collecting_Only()
{
	Create_New_Snow_Profile_Collecting_Content();
	Refresh_Lcd_Buffer_Loading_Image((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile_calibrating
 * Brief:       This will display new snow profile calibrating screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile_calibrating(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	Create_Header();
	Create_Title("NEW TEST",strlen("NEW TEST"));
	Create_New_Snow_Profile_Calibration_Content();
	Create_Footer("CANCEL",strlen("CANCEL"), "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile_processing
 * Brief:       This will display new snow profile processing screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile_processing(void)
{
	switch(Test_disp_ctr)
	{
	case 0:
		Draw_Image_on_Buffer((uint_8 *) Processing_Image0);
		Test_disp_ctr = 1;
		break;

	case 1:
		Draw_Image_on_Buffer((uint_8 *) Processing_Image1);
		Test_disp_ctr = 2;
		break;

	case 2:
		Draw_Image_on_Buffer((uint_8 *) Processing_Image2);
		Test_disp_ctr = 3;
		break;

	case 3:
		Draw_Image_on_Buffer((uint_8 *) Processing_Image3);
		Test_disp_ctr = 4;
		break;

	case 4:
		Draw_Image_on_Buffer((uint_8 *) Processing_Image4);
		Test_disp_ctr = 0;
		break;

	default:
		break;
	}
	Create_Header();
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

void display_New_Snow_profile_processing_Only()
{
	//	Create_New_Snow_Profile_processing_Content();
	//	 Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	Refresh_Lcd_Buffer_Loading_Image((uint_8 *) frame_buff);
}
/*-----------------------------------------------------------------------------* 
 * Function:    display_Slope_Measurment
 * Brief:       This will display slope measurement
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Slope_Measurment(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	Create_Header();
	Create_Title("SLOPE MEASUREMENT",strlen("SLOPE MEASUREMENT"));
	Create_Slope_Measurement_Content();
	Create_Footer("BACK",strlen("BACK"), "",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile_complete
 * Brief:       This will display New Snow profile complete
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile_complete(void)
{

	buff_clear();
	uint16_t i=0;
	char list_num[4];
	Sd_Card_Folder_List();
	while(i<Sd_card_folder_count)
	{
		if(!strcmp(Test_file_directory.test_folder_name,Snow_Profile_Directory[i]))
		{
			Folder_name_index=i;
			break;
		}
		++i;
		Folder_name_index=Sd_card_folder_count;
	}
	Sd_Card_File_List(Folder_name_index);
	i=0;
	char new_test_file_name[16];
	if(Sd_card_file_count>=64)
	{
		printf("\nOverflow Error\n");
	}
	while(i<Sd_card_file_count)
	{


		sprintf(new_test_file_name, "X%s",Test_file_directory.test_file_name+1 );
		if(!strcmp(new_test_file_name,Snow_Profile_File_Name[i]))
		{
			File_name_index=i;
			break;
		}


		//        sprintf(new_test_file_name, "X%s",Test_file_directory.test_file_name );
		//    	if(!strcmp(new_test_file_name,Snow_Profile_List_Data[i]))
		//    	{
		//    		File_name_index=i;
		//    		break;
		//    	}
		++i;
		File_name_index=Sd_card_file_count;
	}



	Draw_Image_on_Buffer((uint_8 *) chart_image);

	char Time_Stamp[32];


	sprintf(Time_Stamp, "%c%c/%c%c/%c%c %c%c:%c%c%c", Test_file_directory.test_folder_name[4],Test_file_directory.test_folder_name[5],
			Test_file_directory.test_folder_name[6],Test_file_directory.test_folder_name[7],
			Test_file_directory.test_folder_name[2],Test_file_directory.test_folder_name[3],
			Snow_Profile_List_Data[File_name_index][1],Snow_Profile_List_Data[File_name_index][2],
			Snow_Profile_List_Data[File_name_index][3],Snow_Profile_List_Data[File_name_index][4],
			Snow_Profile_List_Data[File_name_index][5]);


	Create_Title(Time_Stamp,strlen(Time_Stamp));
	sprintf(list_num, "%d", Sd_card_file_count-File_name_index);
	Draw_string_new(120, 2, list_num, COLOUR_BLACK, MEDIUM_FONT);

	Create_Header();
	//	Create_Snow_Profile_Details_Content();

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_New_Snow_profile_discard
 * Brief:       This will display New Snow profile discard screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_New_Snow_profile_discard(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("NEW SNOW PROFILE",strlen("NEW SNOW PROFILE"));
	//	Create_Snow_Profile_Discard_Content();
	Create_Footer("DISCARD",strlen("DISCARD"),"CANCEL",strlen("CANCEL"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}
/*-----------------------------------------------------------------------------* 
 * Function:    display_confirm_reboot
 * Brief:       This will display confirm reboot screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_confirm_reboot(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("SW REBOOT",strlen("SW REBOOT"));
	Create_Reboot_confirm_Content();
	Create_Footer("REBOOT",strlen("REBOOT"),"CANCEL",strlen("CANCEL"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}
/*-----------------------------------------------------------------------------* 
 * Function:    display_Link_Slope_with_Profile
 * Brief:       This will display Link Slope with Profile
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Link_Slope_with_Profile(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("LINK SNOW PROFILE",strlen("LINK SNOW PROFILE"));
	//	Create_Snow_Profiles_List_Content();
	Create_Footer("BACK",strlen("BACK"),"LINK",strlen("LINK"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_save_slope
 * Brief:       This will display save slope measurement screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_save_slope(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("SLOPE MEASUREMENT",strlen("SLOPE MEASUREMENT"));

	Create_Slope_Measurement_Content();
	Create_Footer("CANCEL",strlen("CANCEL"),"SAVE",strlen("SAVE"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    gps_toggle
 * Brief:       Turn gps on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void dummy_function(void)
{


}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Settings_Password
 * Brief:       This will display Settings Password screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Settings_Password(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("ENTER PASSWORD",strlen("ENTER PASSWORD"));
	//	Create_Settings_Password_Content();
	Create_Footer("BACK",strlen("BACK"),"NEXT",strlen("NEXT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_Settings_Main
 * Brief:       This will display Settings Main screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Settings_Main(void)
{
	//buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("SETTINGS",strlen("SETTINGS"));
	Create_Footer("BACK",strlen("BACK"),"SELECT",strlen("SELECT"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_about_device
 * Brief:       This will display details about the device
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_about_device(void)
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	Create_Header();
	Create_Title("ABOUT DEVICE",strlen("ABOUT DEVICE"));
	Create_about_device_Content();
	Create_Footer("BACK",strlen("BACK"),"",0);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);	


}
/*-----------------------------------------------------------------------------* 
 * Function:    display_record_not_found
 * Brief:       This will display record not found screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_record_not_found(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
	Create_Header();

	Create_Title("ERROR",strlen("ERROR"));
	Create_record_not_found_content();
	Create_Footer("BACK",strlen("BACK"),"",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    display_sd_error_screen
 * Brief:       This will display sdcard error screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_sd_error_screen(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_dispaly_sdcard_insert();
	Create_Title("REMOVE SDCARD",strlen("REMOVE SDCARD"));
	Create_Footer("",0,"",0);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_error_screen_during_test
 * Brief:       This will display error screen during test.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_error_screen_during_test(void)
{

	buff_clear();  
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("NEW TEST",strlen("NEW TEST"));
	//	Create_error_screen_during_test_Content();
	Create_Footer("CANCEL",strlen("CANCEL"), "RETRY",strlen("RETRY"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    dispaly_battery_booting
 * Brief:       This will display battery rebooting screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void dispaly_battery_booting(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_dispaly_rebooting_content();
	Create_Title("REBOOTING",strlen("REBOOTING"));
	Create_Footer("",0,"",0);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}
/*-----------------------------------------------------------------------------* 
 * Function:    dispaly_battery_low
 * Brief:       This will display battery low screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void dispaly_battery_low(void)
{
	Refresh_Lcd_Buffer((uint_8 *) Battery_low_1);
	_time_delay(1000);
	Refresh_Lcd_Buffer((uint_8 *) Battery_low_2);
	_time_delay(1000);
	Refresh_Lcd_Buffer((uint_8 *) Battery_low_1);
	_time_delay(500);

}
/*-----------------------------------------------------------------------------* 
 * Function:    Handle_battery_related_issues
 * Brief:       This will check battery related issues
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Handle_battery_related_issues(void)
{

	if (Battery_adc_stucked_and_shows_zero > 3)
	{

		State_of_Screen = UI_CANT_PROCEED_NEED_TO_TURN_OFF;
		Software_reboot();
		while (1)
		{

		}
	}
	if ((Battery_low_status == 1))
	{
		Battery_low_status = 0;
		printf("Battery Low\n");
		if(Core_clock_selection == ACQUISITION_MODE)
		{        	
			De_Init_Hw_Timer();
			Switch_to_idle_mode();
		}
		dispaly_battery_low();
		State_of_Screen = UI_CANT_PROCEED_NEED_TO_TURN_OFF;
		De_Init_HardWare_When_Battery_Low();

		while (1)
		{

		}
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_set_favourite
 * Brief:       This will toggle the favourite icon in the header and add/remove them from favourites menu.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_set_favourite(void)
{
	uint_8 count=0,  status=FILE_NOT_PRESENT;
	char dummy_space[5]={"     "};
	printf("Adding/Deleting Favourite name to the list!! (display_set_favourite)\n");


	for(count=0; count<Favourites_file_count; count++)
	{
		if(strcmp(Snow_Profile_File_Name[File_name_index]+1, FavouriteEntry[count].Favourite_File_Name+1)==0)
		{

			printf("\n\nFAVOURITE CONTENT TO BE DELETED IS: ");
			printf("\nFav folder name: %s", FavouriteEntry[count].Favourite_Folder_Name);
			printf("\nFav file name: %s", FavouriteEntry[count].Favourite_File_Name);
			printf("\nFAVOURITES FILE COUNT %d\n", Favourites_file_count);
			//Delete Code Here
			Favourites_file_count = Favourites_file_count - 1;
			for(;count<Favourites_file_count;count++)
			{
				sprintf(FavouriteEntry[count].Favourite_Directory, 		FavouriteEntry[count+1].Favourite_Directory);
				sprintf(FavouriteEntry[count].Favourite_Folder_Name, 	FavouriteEntry[count+1].Favourite_Folder_Name);
				sprintf(FavouriteEntry[count].Favourite_File_Name, 		FavouriteEntry[count+1].Favourite_File_Name);
				sprintf(FavouriteEntry[count].Favourite_File_Date, 		FavouriteEntry[count+1].Favourite_File_Date);
				FavouriteEntry[count].Favourite_Folder_Index 	=		FavouriteEntry[count+1].Favourite_Folder_Index;
				FavouriteEntry[count].Favourite_File_Index 		= 		FavouriteEntry[count+1].Favourite_File_Index;
			}
			count = 0;
			status=FILE_PRESENT;
			break;

		}
	}

	if(status == FILE_NOT_PRESENT)	//If the counter reaches the end of the list or if there is no favourite content present
	{

		FavouriteEntry[Favourites_file_count].Favourite_Folder_Index = Folder_name_index;
		FavouriteEntry[Favourites_file_count].Favourite_File_Index = Sd_card_file_count - File_name_index;	
		if(FavouriteEntry[Favourites_file_count].Favourite_File_Index < 10)
		{
			dummy_space[3] = '\0';
			printf("\n Dummy_Space %d", strlen(dummy_space));
		}
		else if((FavouriteEntry[Favourites_file_count].Favourite_File_Index >= 10) && (FavouriteEntry[Favourites_file_count].Favourite_File_Index < 100))
		{
			dummy_space[2] = '\0';
			printf("\n Dummy_Space %d", strlen(dummy_space));
		}
		else if((FavouriteEntry[Favourites_file_count].Favourite_File_Index >= 100) && (FavouriteEntry[Favourites_file_count].Favourite_File_Index < 1000))
		{
			dummy_space[1] = '\0';
			printf("\n Dummy_Space %d", strlen(dummy_space));

		}
		sprintf(FavouriteEntry[Favourites_file_count].Favourite_Folder_Name,Snow_Profile_Directory[Folder_name_index]);
		sprintf(FavouriteEntry[Favourites_file_count].Favourite_File_Name, 	Snow_Profile_File_Name[File_name_index]);
		sprintf(FavouriteEntry[Favourites_file_count].Favourite_File_Date, 	Snow_Profile_List_Data[File_name_index]);
		sprintf(FavouriteEntry[Favourites_file_count].Favourite_Directory, "%c%c/%c%c/%c%c %d%s%c%c:%c%c%c", 
				FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[4], FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[5],
				FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[6], FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[7],
				FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[2], FavouriteEntry[Favourites_file_count].Favourite_Folder_Name[3],
				FavouriteEntry[Favourites_file_count].Favourite_File_Index, dummy_space,
				FavouriteEntry[Favourites_file_count].Favourite_File_Date[1], 	FavouriteEntry[Favourites_file_count].Favourite_File_Date[2],
				FavouriteEntry[Favourites_file_count].Favourite_File_Date[3],	FavouriteEntry[Favourites_file_count].Favourite_File_Date[4],
				FavouriteEntry[Favourites_file_count].Favourite_File_Date[5]);

		printf("\n\nFAVOURITE CONTENTS ADDED ARE: ");
		printf("\nFav folder name: %s", FavouriteEntry[Favourites_file_count].Favourite_Folder_Name);
		printf("\nFav file name: %s", FavouriteEntry[Favourites_file_count].Favourite_File_Name);
		printf("\nFav Directory: %s", FavouriteEntry[Favourites_file_count].Favourite_Directory);
		++Favourites_file_count;
		printf("\nFAVOURITES FILE COUNT %d\n", Favourites_file_count);

	}
	for(count = 0; count <= Favourites_file_count; count++)
	{
		FavouriteEntry[count].Favourites_Dir_Count =	Favourites_file_count;
		//		printf("\nTotal Fav Count: %d  Fav File Index= %d  Fav File Directory: %s", FavouriteEntry[count].Favourites_Dir_Count,
		//				FavouriteEntry[count].Favourite_File_Index, 	FavouriteEntry[count].Favourite_Directory);
		//		printf("\nFav fol name: %s  Fav File Name= %s  Fav File Date: %s\n", FavouriteEntry[count].Favourite_Folder_Name,
		//				FavouriteEntry[count].Favourite_File_Name, 	FavouriteEntry[count].Favourite_File_Date);
	}
	sort_favourite_list();
	//	Write_Favourite_file_information_To_Sdcard();	//Called inside sort_favourite_list!!
}


/*-----------------------------------------------------------------------------* 
 * Function:    sort_favourite_list
 * Brief:       This will sort the favourite list according to the date and time .
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void sort_favourite_list(void)
{
	FavouriteTableEntry temp_Favourite_Entry;
	char temp_Favourite_Folder_Name[16], temp_Favourite_File_Name[16], temp_Favourite_File_Date[16];


	uint_8 fav_cnt,sel_cnt;
	char temp_date[6];
	uint_32  fav_date_time_in_seconds, sel_date_time_in_seconds;
	DATE_STRUCT fav_date, sel_date;

	Favourites_file_count = FavouriteEntry[0].Favourites_Dir_Count;
	for(fav_cnt = 0; fav_cnt < Favourites_file_count; fav_cnt++)
	{
		sprintf(temp_date, "%c%c%c%c", FavouriteEntry[fav_cnt].Favourite_Folder_Name[0], FavouriteEntry[fav_cnt].Favourite_Folder_Name[1],
				FavouriteEntry[fav_cnt].Favourite_Folder_Name[2], FavouriteEntry[fav_cnt].Favourite_Folder_Name[3]);
		fav_date.YEAR = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", FavouriteEntry[fav_cnt].Favourite_Folder_Name[4],FavouriteEntry[fav_cnt].Favourite_Folder_Name[5]);
		fav_date.MONTH = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", FavouriteEntry[fav_cnt].Favourite_Folder_Name[6], FavouriteEntry[fav_cnt].Favourite_Folder_Name[7]);
		fav_date.DAY = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", FavouriteEntry[fav_cnt].Favourite_File_Date[1], FavouriteEntry[fav_cnt].Favourite_File_Date[2]);
		fav_date.HOUR = (uint_16)strtol(temp_date, NULL, 10);
		if(FavouriteEntry[fav_cnt].Favourite_File_Date[5] == 'P')
		{
			fav_date.HOUR += 12;
		}

		sprintf(temp_date, "%c%c", FavouriteEntry[fav_cnt].Favourite_File_Date[3], FavouriteEntry[fav_cnt].Favourite_File_Date[4]);
		fav_date.MINUTE = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", FavouriteEntry[fav_cnt].Favourite_File_Date[6], FavouriteEntry[fav_cnt].Favourite_File_Date[7]);
		fav_date.SECOND = (uint_16)strtol(temp_date, NULL, 10);

		fav_date_time_in_seconds = date_time_to_seconds(&fav_date);

		for(sel_cnt = fav_cnt; sel_cnt < Favourites_file_count; sel_cnt++)
		{
			sprintf(temp_date, "%c%c%c%c", FavouriteEntry[sel_cnt].Favourite_Folder_Name[0], FavouriteEntry[sel_cnt].Favourite_Folder_Name[1],
					FavouriteEntry[sel_cnt].Favourite_Folder_Name[2], FavouriteEntry[sel_cnt].Favourite_Folder_Name[3]);
			sel_date.YEAR = (uint_16)strtol(temp_date, NULL, 10);

			sprintf(temp_date, "%c%c", FavouriteEntry[sel_cnt].Favourite_Folder_Name[4],FavouriteEntry[sel_cnt].Favourite_Folder_Name[5]);
			sel_date.MONTH = (uint_16)strtol(temp_date, NULL, 10);

			sprintf(temp_date, "%c%c", FavouriteEntry[sel_cnt].Favourite_Folder_Name[6], FavouriteEntry[sel_cnt].Favourite_Folder_Name[7]);
			sel_date.DAY = (uint_16)strtol(temp_date, NULL, 10);

			sprintf(temp_date, "%c%c", FavouriteEntry[sel_cnt].Favourite_File_Date[1], FavouriteEntry[sel_cnt].Favourite_File_Date[2]);
			sel_date.HOUR = (uint_16)strtol(temp_date, NULL, 10);
			if(FavouriteEntry[sel_cnt].Favourite_File_Date[5] == 'P')
			{
				sel_date.HOUR += 12;
			}

			sprintf(temp_date, "%c%c", FavouriteEntry[sel_cnt].Favourite_File_Date[3], FavouriteEntry[sel_cnt].Favourite_File_Date[4]);
			sel_date.MINUTE = (uint_16)strtol(temp_date, NULL, 10);
			sel_date.SECOND = 0;
			sel_date_time_in_seconds = date_time_to_seconds(&sel_date);

			if(fav_date_time_in_seconds < sel_date_time_in_seconds)	//Swapping
			{	
				temp_Favourite_Entry = FavouriteEntry[sel_cnt];
				FavouriteEntry[sel_cnt] = FavouriteEntry[fav_cnt];
				FavouriteEntry[fav_cnt] = temp_Favourite_Entry;
				break;
			}
		}

	}
	//	printf("\n\nSORTED FAVOURITES LIST!!!!!\n\n");
	//	for(uint_8 count = 0; count < Favourites_file_count; count++)
	//	{
	//		printf("\nTotal Fav Count: %d  Fav File Index= %d  Fav File Directory: %s", FavouriteEntry[count].Favourites_Dir_Count,
	//				FavouriteEntry[count].Favourite_File_Index, 	FavouriteEntry[count].Favourite_Directory);
	//		printf("\nFav fol name: %s  Fav File Name= %s  Fav File Date: %s\n", FavouriteEntry[count].Favourite_Folder_Name,
	//				FavouriteEntry[count].Favourite_File_Name, 	FavouriteEntry[count].Favourite_File_Date);
	//	}
	Write_Favourite_file_information_To_Sdcard();
}

void Update_Fav_List_File_Index_After_File_Deletion(void)
{
	uint_8 fav_cnt, fol_cnt, file_cnt;
	char dummy_space[5] = "     ";

	for(fol_cnt = 0; fol_cnt < Sd_card_folder_count; fol_cnt++)
	{
		Sd_Card_File_List(fol_cnt);
		printf("\n\n*****Update_Fav_List_File_Index_After_File_Deletion*****\n ");
		for(file_cnt = 0; file_cnt < Sd_card_file_count; file_cnt++)
		{
			for(fav_cnt = 0; fav_cnt < Favourites_file_count; fav_cnt++)
			{

				if(strcmp(FavouriteEntry[fav_cnt].Favourite_File_Name+1,Snow_Profile_File_Name[file_cnt]+1)==0)
				{
					printf("\nCOMPARE SUCCESS!! Favourite_File_Name %s  Snow_Profile_File_Name %s ", FavouriteEntry[fav_cnt].Favourite_File_Name, Snow_Profile_File_Name[file_cnt]);
					FavouriteEntry[fav_cnt].Favourite_File_Index = Sd_card_file_count - file_cnt;
					if(File_name_index < 10)
					{
						dummy_space[3] = '\0';
						printf("\n Dummy_Space %d", strlen(dummy_space));
					}
					else if((File_name_index >= 10) && (File_name_index < 100))
					{
						dummy_space[2] = '\0';
						printf("\n Dummy_Space %d", strlen(dummy_space));
					}
					else if((File_name_index >= 100) && (File_name_index < 1000))
					{
						dummy_space[1] = '\0';
						printf("\n Dummy_Space %d", strlen(dummy_space));

					}
					sprintf(FavouriteEntry[fav_cnt].Favourite_Directory, "%c%c/%c%c/%c%c %d%s%c%c:%c%c%c", 
							FavouriteEntry[fav_cnt].Favourite_Folder_Name[4], FavouriteEntry[fav_cnt].Favourite_Folder_Name[5],
							FavouriteEntry[fav_cnt].Favourite_Folder_Name[6], FavouriteEntry[fav_cnt].Favourite_Folder_Name[7],
							FavouriteEntry[fav_cnt].Favourite_Folder_Name[2], FavouriteEntry[fav_cnt].Favourite_Folder_Name[3],
							FavouriteEntry[fav_cnt].Favourite_File_Index, dummy_space,
							FavouriteEntry[fav_cnt].Favourite_File_Date[1], FavouriteEntry[fav_cnt].Favourite_File_Date[2],
							FavouriteEntry[fav_cnt].Favourite_File_Date[3],	FavouriteEntry[fav_cnt].Favourite_File_Date[4],
							FavouriteEntry[fav_cnt].Favourite_File_Date[5]);
					//					printf("\nTotal Fav Count: %d  Fav File Index= %d  Fav File Directory: %s", FavouriteEntry[fav_cnt].Favourites_Dir_Count,
					//							FavouriteEntry[fav_cnt].Favourite_File_Index, 	FavouriteEntry[fav_cnt].Favourite_Directory);
					//						printf("\nFav fol name: %s  Fav File Name= %s  Fav File Date: %s\n", FavouriteEntry[fav_cnt].Favourite_Folder_Name,
					//								FavouriteEntry[fav_cnt].Favourite_File_Name, 	FavouriteEntry[fav_cnt].Favourite_File_Date);
					break;
				}
				FavouriteEntry[fav_cnt].Favourites_Dir_Count = Favourites_file_count;
			}
		}	
	}
}


/*-----------------------------------------------------------------------------* 
 * Function:    Low_battery_shutdown
 * Brief:       This will kill the device when battery voltage is less than threshold value.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Low_battery_shutdown(void)
{
	Buzzer_Short_Beep(1);
	dispaly_battery_low();
	State_of_Screen = UI_CANT_PROCEED_NEED_TO_TURN_OFF;
	De_Init_HardWare_When_Battery_Low();
	Kill_OFF_Controller();
	while(1) 
	{};

}
/*-------------- -------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
