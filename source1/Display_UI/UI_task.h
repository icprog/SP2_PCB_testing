/******************************************************************************
 *
 *   Filename: $UI_task.h
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains header information of processing user feedback
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#ifndef UI_TASK_H_
#define UI_TASK_H_

typedef enum
{
UI_IDLE_CHECK_DISPLAY_OFF,
UI_IDLE_CHECK_KILL,
UI_LOW_PWR_SLEEP_MODE,
UI_WAKING_UP_FROM_SLEEP,
UI_MAINMENU_LIST,
UI_SNOW_PROFILE_LIST,
UI_BLUETOOTH_MAIN,
UI_SLOP_MEASURMENTS,
UI_SENSOR_OUTPUT,
UI_NEW_TEST_LIST,
UI_NEW_SNOW_PROFILE,
UI_SNOW_PROFILE_DETAILS,
UI_SNOW_PROFILE_OPTIONS,
UI_SNOW_PROFILE_DELETE,
UI_NEW_SNOW_PROFILE_COLLECTING,
UI_NEW_SNOW_PROFILE_PROCESSING,
UI_NEW_SNOW_PROFILE_COMPLETE,
UI_NEW_SNOW_PROFILE_DISCARD,
UI_LINK_SLOP_WITH_PROFILE,
UI_SETTINGS_PASSWORD,
UI_SETTINGS,
UI_NEW_SNOW_PROFILE_CALIBRATING,
UI_USB_MASS_STORAGE,
UI_CANT_PROCEED_NEED_TO_TURN_OFF,
UI_REBOOT_CONFIRM,
UI_ABOUT_DEVICE,
UI_FAVOURITES_LIST,
UI_SET_FAVOURITE,
UI_SAVE_SLOPE,
UI_GPS_LOCATION,
UI_VIEW_CHARTS,
UI_DELETE_PROFILE,
//UI_DEFAULT_GRAPH_VIEW,
UI_NEW_MULTIPLE_TEST_LIST,
UI_SNOW_PROFILE_FILE_LIST,
UI_MASS_STORAGE_ENABLE,
UI_SETTINGS_SELECTION,
UI_CONFIG_SETTINGS,
UI_DATE_AND_TIME,
UI_UPDATE_DATE,
UI_UPDATE_TIME,
UI_UPDATE_ZONE,
UI_DISPLAY_ERROR_SCREENS,
UI_FIRMWARE_UPDATE
}STATE_OF_SCREEN;

#define FOLDER_EQUAL	0
#define FOLDER_UNEQUAL 	1

#define SYNC_DAILY_FILES 	0
#define SYNC_WEEKLY_FILES 	1
#define SYNC_ALL_FILES 		2

#define OUTPUT_FILE_FORMAT						'X'
#define OUTPUT_FILE_FORMAT_SMOOTH_DATA			'B'
extern char test_folder_name[], test_file_name[];
extern char ble_daily_sync_file_name[20];
extern uint32_t Total_Test_Ctr;
extern uint8_t type_of_error;

extern STATE_OF_SCREEN State_of_Screen,Next_State;
extern STATE_OF_SCREEN Prev_State_of_Screen;
void User_intrface_task(uint_32 initial_data);
#endif /* UI_TASK_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
