/*
 * UI_Display.h
 *
 *  Created on: Dec 13, 2013
 *      Author: GADGEON-03
 */

#ifndef UI_DISPLAY_H_
#define UI_DISPLAY_H_

#define DISPLAY_X_MIN 	1
#define DISPLAY_Y_MIN 	1
#define DISPLAY_X_MAX 	240
#define DISPLAY_Y_MAX 	400

#define DISPLAY_HEADER_WIDTH 	22
#define DISPLAY_TITLE_WIDTH 	26
#define DISPLAY_FOOTER_WIDTH 	30
#define DISPLAY_CONTAINER_WIDTH 	(DISPLAY_Y_MAX-(DISPLAY_HEADER_WIDTH+DISPLAY_TITLE_WIDTH+DISPLAY_FOOTER_WIDTH))
#define DISPLAY_MENU_WIDTH 40

#define DISPLAY_HEADER_X_MIN	DISPLAY_X_MIN
#define DISPLAY_HEADER_Y_MIN	DISPLAY_Y_MIN
#define DISPLAY_HEADER_X_MAX 	DISPLAY_X_MAX
#define DISPLAY_HEADER_Y_MAX	(DISPLAY_Y_MIN+DISPLAY_HEADER_WIDTH)

#define DISPLAY_TITLE_X_MIN		DISPLAY_X_MIN
#define DISPLAY_TITLE_Y_MIN		(DISPLAY_Y_MIN+DISPLAY_HEADER_WIDTH)
#define DISPLAY_TITLE_X_MAX 	DISPLAY_X_MAX
#define DISPLAY_TITLE_Y_MAX		(DISPLAY_Y_MIN+DISPLAY_HEADER_WIDTH+DISPLAY_TITLE_WIDTH)

#define DISPLAY_CONTAINER_X_MIN		DISPLAY_X_MIN
#define DISPLAY_CONTAINER_Y_MIN		(DISPLAY_Y_MIN+DISPLAY_HEADER_WIDTH+DISPLAY_TITLE_WIDTH)
#define DISPLAY_CONTAINER_X_MAX 	DISPLAY_X_MAX
#define DISPLAY_CONTAINER_Y_MAX		(DISPLAY_Y_MIN+DISPLAY_HEADER_WIDTH+DISPLAY_TITLE_WIDTH+DISPLAY_CONTAINER_WIDTH)

#define DISPLAY_FOOTER_X_MIN		DISPLAY_X_MIN
#define DISPLAY_FOOTER_Y_MIN		DISPLAY_CONTAINER_Y_MAX
#define DISPLAY_FOOTER_X_MAX 		DISPLAY_X_MAX
#define DISPLAY_FOOTER_Y_MAX		DISPLAY_Y_MAX
#define DISPALY_GRAPH_X_MIN	17	
#define DISPALY_GRAPH_Y_MIN	66

#define DISPALY_GRAPH_X_MAX	(DISPALY_GRAPH_X_MIN+200)
#define DISPALY_GRAPH_Y_MAX	(DISPALY_GRAPH_Y_MIN+217)

#define COLOUR_BLACK 	1
#define COLOUR_WHITE	0

#define LARGE_FONT			1
#define SMALL_FONT          0
#define MEDIUM_FONT			2

#define BATTERY_FULL 2
#define BATTERY_HALF_FULL 1
#define BATTERY_NEARLY_EMPTY 0

#define BLUETOOTH_CONNECTED 1
#define BLUETOOTH_ON_BUT_NOT_CONNECTED 2

#define GPS_NOT_LINKED 0
#define GPS_LINKED 1

#define NUM_X_PIXEL_PER_CHAR 10
#define NUM_Y_PIXEL_PER_CHAR 16

#define NUM_X_PIXEL_PER_CHAR_MEDIUM 13
#define NUM_Y_PIXEL_PER_CHAR_MEDIUM 20

#define NUM_OF_LIST_ITEM_IN_A_VIEW 8

#define MENU_NOT_SELECTED 0
#define MENU_SELECTED 1

#define COMPASS_NORTH 0
#define COMPASS_NORTY_EAST 1
#define COMPASS_EAST 2
#define COMPASS_SOUTH_EAST 3
#define COMPASS_SOUTH 4
#define COMPASS_SOUTH_WEST 5
#define COMPASS_WEST 6
#define COMPASS_NORTH_WEST 7

//xxxxxxxxxxxxxxxxxxxxxxxxxxx ver 2.0 New defines  xxxxxxxxxxxxxxxxxxxxxxxxxxx

#define ALL_ICONS						 		0
#define BLUETOOTH_CONNECTED_ICON				1
#define BLUETOOTH_ON_BUT_NOT_CONNECTED_ICON 	2
#define GPS_NOT_LINKED_ICON 					3		
#define GPS_LINKED_ICON 						4
#define BATTERY_HALF_ICON 						5
#define FAV_ICON								7	
#define LOCATION_ICON							8
#define ANGLE_ICON								9
#define ASPECT_ICON								10
#define VIEW_ICON								11
#define DELETE_ICON								12
#define MULTIPLETEST_ICON						13
#define MULTIPLETEST1_ICON						14
#define GRAY									15
#define CIRCLE_GREY								16
#define CIRCLE_BLACK							17
#define TOP_RECT_WHITE							18
#define TOP_RECT_BLACK							19
#define BOTTOM_RECT_WHITE						20
#define BOTTOM_RECT_BLACK						21
#define BATTERY_FULL_ICON       				22
#define BATTERY_EMPTY_ICON       				23
#define COMPASS_ALL_ICON_NUM					24
#define COMPASS_ROLL_ICON_NUM					25
#define COMPASS_SPIN_ICON_NUM					26
#define COMPASS_FLIP_ICON_NUM					27

#define NUM_X_PIXEL_BATTERY_ICON				30
#define NUM_X_PIXEL_BLUETOOTH_ICON				16
#define NUM_X_PIXEL_GPS_ICON				 	16

#define NUM_X_PIXEL_FAV_ICON					14					
#define NUM_X_PIXEL_LOCATION_ICON				21
#define NUM_X_PIXEL_ANGLE_ICON					12
#define NUM_X_PIXEL_ASPECT_ICON					11
#define NUM_X_PIXEL_VIEW_ICON					24
#define NUM_X_PIXEL_DELETE_ICON					15
#define NUM_X_PIXEL_MULTIPLETEST_ICON			16
#define NUM_X_PIXEL_MULTIPLETEST1_ICON			31
#define NUM_X_GRAY								49
#define NUM_X_CIRCLE_GREY						32
#define NUM_X_CIRCLE_BLACK						32
//#define NUM_X_TOP_RECT						59
//#define NUM_X_BOTTOM_RECT						59
#define NUM_X_TOP_RECT							36
#define NUM_X_BOTTOM_RECT						36
#define NUM_X_COMPASS_ALL_ICON					1800
#define NUM_X_COMPASS_ROLL_ICON					2312
#define NUM_X_COMPASS_SPIN_ICON					1280
#define NUM_X_COMPASS_FLIP_ICON					1280

#define NUM_Y_PIXEL_FAV_ICON					16					
#define NUM_Y_PIXEL_LOCATION_ICON				24
#define NUM_Y_PIXEL_ANGLE_ICON					16
#define NUM_Y_PIXEL_ASPECT_ICON					16
#define NUM_Y_PIXEL_DELETE_ICON					16
#define NUM_Y_PIXEL_VIEW_ICON					16
#define NUM_Y_PIXEL_MULTIPLETEST				16
#define NUM_Y_PIXEL_MULTIPLETEST1				32
#define NUM_Y_GRAY								24
#define NUM_Y_CIRCLE_GREY						32
#define NUM_Y_CIRCLE_BLACK						32
//#define NUM_Y_TOP_RECT						24
//#define NUM_Y_BOTTOM_RECT						32
#define NUM_Y_TOP_RECT							32
#define NUM_Y_BOTTOM_RECT						32
#define NUM_Y_COMPASS_ALL_ICON					200	
#define NUM_Y_COMPASS_ROLL_ICON					136
#define NUM_Y_COMPASS_SPIN_ICON					80
#define NUM_Y_COMPASS_FLIP_ICON					128

#define LOADING_IMAGE_NO_BLACK				 	 0
#define LOADING_IMAGE_ONE_BLACK				 	 1
#define LOADING_IMAGE_TWO_BLACK				 	 2
#define LOADING_IMAGE_THREE_BLACK				 3
#define LOADING_IMAGE_ALL_BLACK				 	 4

#define BLUETOOTH_ON							 0
#define BLUETOOTH_OFF							 1	

#define GPS_ON							 		 0
#define GPS_OFF									 1	

#define MULTIPLE_TESTS_ON						 0
#define MULTIPLE_TESTS_OFF						 1

#define TEST_NUMBER_ONE							 1
#define TEST_NUMBER_TWO						 	 2
#define TEST_NUMBER_THREE						 3


#define SNOWPROFILEOPTIONS_DISPLACEMENT  		 2
#define FAV_STAR_LOCATION						 7
#define FILE_PRESENT							 0
#define FILE_NOT_PRESENT						 1

#define BLUETOOTH_DEVICE_NOT_CONNECTED     		 0
#define BLUETOOTH_DEVICE_CONNECTED      		 1
#define BLUETOOTH_TURNED_OFF      				 2
#define BLUETOOTH_INITIALIZING         			 3
#define BLUETOOTH_QUERY_LIST_COMPLETED			 4
#define BLUETOOTH_WAIT_FOR_SYNC					 5
#define BLUETOOTH_SYNC_FILES     				 6
#define BLUETOOTH_TRANSFER_COMPLETED			 7
#define BLUETOOTH_TRANSFER_FAILED 				 8
#define BLUETOOTH_QRY_LIST_NOT_CREATED 			 9

#define MAX_NO_OF_FILES_IN_DIR					 (64)
#define MAX_NO_OF_DIR							 (64)

#define CAL_REFRESH_TIME						 (1000)
#define GPS_REFRESH_TIME						 (250)

#define ACC_REFRESH_TIME						 (5000)
#define ACC_BUZZER_BEEP_TIME					 (1000)

typedef void (*menuFunction)(void);
// Entry for a monitor command table

typedef struct
{
    uint_8 Menu_Index;
    char Menu_String[32];	// Mainmenu text
    menuFunction menu_Function;		// menu implementation function
    uint_8 UI_state;	//State which enter after selecting menu item
    
} MenuTableEntry;

typedef struct
{
    char  Calib_condition[32];		// Calib_condition text
    float min_voltage;				// min voltage range for calibration
    float max_voltage;				// max voltage range for calibration
    float curr_voltage;
    char  Calib_status;
} CalibTable;

typedef struct FavouriteTable
{
	char Favourite_Folder_Name[16];
	char Favourite_File_Name[16];
	char Favourite_File_Date[16];
	uint_8 Favourite_Folder_Index;
	uint_8 Favourite_File_Index;
	char Favourite_Directory[24];
	uint_16 Favourites_Dir_Count;
} FavouriteTableEntry;

typedef struct TimeFileEntry
{
	char File_Name[16];

} TimeFileEntry;

//typedef struct DateFolderContent
//{
//	char Folder_Name[16];
//	uint_8 File_Count;
//	TimeFileEntry TimeFileName[];
//	
//} DateFolderEntry;
//
//extern DateFolderEntry Date_Folder[];

extern MenuTableEntry Mainmenu_Table[];
extern MenuTableEntry Testmenu_Table[];
extern MenuTableEntry Settingsmenu_Table[];
extern MenuTableEntry Snowprofileoptions_Table[];
extern MenuTableEntry Date_Time_menu_Table[];
extern MenuTableEntry Defaultgraphview_Table[];
extern FavouriteTableEntry FavouriteEntry[];
extern MenuTableEntry SettingsSelection_Table[];
extern const unsigned char loading_image_all[];

extern volatile uint_8 Folder_name_index;
extern volatile uint_8 File_name_index;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 File_name_count;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Folder_name_count;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Folder_name_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 File_name_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Test_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Settings_selection_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Main_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Test_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Settings_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Default_graph_view_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Snow_profile_options_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_16 Snow_Profiles_List_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_16 Snow_Profiles_File_List_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_16 Favourites_List_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Date_Folder_name_count;

//This variable is adjusted according to bluetooth status
extern volatile uint_8 ble_transmission_status;

extern char Snow_Profile_Directory[][16];
extern char Snow_Profile_List_Data[][16];
extern char Snow_Profile_File_Name[][16];


//This variable is incremented and decremented according to left or right key in the settings menu 
extern volatile uint_8 Config_settings_selection;
extern uint_8 compass_calibration_screen;
extern uint_8 accelerometer_calibration_screen;
extern volatile uint_8 bluetooth_status;
extern volatile uint_8 multiple_tests_status;
extern volatile uint_8 test_number;

extern volatile uint_16 Favourites_List_count;
extern volatile uint_16 Favourites_file_count;


extern volatile uint_16 Snow_Profiles_List_count;

extern volatile uint_8 Snow_Profiles_File_List_count;

extern uint_8 Battery_adc_stucked_and_shows_zero;
extern char BUSY;
extern uint_8 Calib_status[]; 

extern CalibTable Pressure_Calib_Table[];
extern CalibTable ROS1_Calib_Table[];
extern CalibTable ROS2_Calib_Table[];
extern CalibTable IRDMS_Calib_Table[];
extern CalibTable Accelerometer_Calib_Table[];
extern CalibTable Magnetometer_Calib_Table[];

extern volatile uint_8 IRDMS_Condition_selection;
extern volatile uint_8 Pressure_Condition_selection;
extern volatile uint_8 ROS_Condition_selection;
extern volatile uint_8 ROS_Condition_selection;
extern volatile uint_8 Accelerometer_Condition_selection;
extern volatile uint_8 Magnetometer_Condition_selection;
extern volatile uint_8 Acc_reading_status;

void display_Snow_Profiles_File_List(void);
void display_favourites_menu(void);
void display_about_device(void);
void display_Settings_Main(void);
void display_record_not_found(void);
void display_sd_error_screen(void);
void dispaly_battery_booting(void);
void dispaly_battery_low(void);
void display_error_screen_during_test(void);
void Handle_battery_related_issues(void);
void display_Settings_Password(void);
void display_set_favourite(void);
void display_save_slope(void);
void dummy_function(void);

void sort_favourite_list(void);
void display_Link_Slope_with_Profile(void);
void display_confirm_reboot(void);
void display_New_Snow_profile_discard(void);
void display_New_Snow_profile_complete(void);
void display_Slope_Measurment(void);
void display_New_Snow_profile_processing(void);
void display_New_Snow_profile_processing_Only();
void display_New_Snow_profile_calibrating(void);
void display_New_Snow_profile_collecting(void);
void Update_Collecting_Only();
void display_New_Snow_profile(void);
void display_Sensor_Output(void);
void display_Config_Settings(void);
void display_Settings_Selection(void);
void display_mass_storage_enable(void);
void display_default_graph_view(void);
void display_Bluetooth_main(void);
void display_Bluetooth_file_sending_status(void);
void dispaly_Usb_Connected(void);
void dispaly_Waiting_for_file(void);
void display_Snow_Profiles_Delete(void);
void display_Snow_Profile_Details(void);
void display_Snow_Profiles_Options(void);
void display_Snow_Profiles_List(void);
void display_Testmenu(void);
void display_MultipleTestMenu(void);
void display_Mainmenu(void);
void create_Location_informations(void);
void Create_Header(void);
float battery_Voltage_Check(void);
void Testmenu_Key_down(void);
void Testmenu_Key_up(void);
void Mainmenu_Key_down(void);
void Mainmenu_Key_up(void);
void Favourites_List_Key_up(void);
void Favourites_List_Key_down(void);
void Snow_Profiles_List_Key_down(void);
void Snow_Profiles_List_Key_up(void);
void Snow_Profiles_Options_Key_up(void);
void Snow_Profiles_Options_Key_down(void);
boolean Check_Ui_BackAction(void);
boolean Check_User_Authentication(void);
void Password_key_down(void);
void Password_key_up(void);
void Password_navigate_left(void);
void Password_navigate_right(void);
void clear_all_user_typed_pass(void);
void Config_Settings_navigate_up(void);
void Config_Settings_navigate_down(void);
void Settingsmenu_Key_up(void);
void Settingsmenu_Key_down(void);
void Default_Graph_View_Key_down(void);
void Default_Graph_View_Key_up(void);
void Snow_Profiles_File_List_Key_up(void);
void Snow_Profiles_File_List_Key_down(void);
void Change_Defualt_Gragh_View(void);
extern void Create_Snow_Profiles_File_List_Content(void);
extern uint_8 tempCount;
extern unsigned char Test_disp_ctr;
extern void Add_Item_To_Menu(const char *menu_Text, uint_8 menu_position,uint_16 menu_selection_status);
extern void Update_Fav_List_File_Index_After_File_Deletion(void);

void display_bluetooth_toggle(void);
void display_gps_toggle(void);
void display_multiple_tests_toggle(void);

extern void IRDMS_Condition_Key_up(void);
extern void IRDMS_Condition_Key_down(void);
extern void Pressure_Condition_Key_up(void);
extern void Pressure_Condition_Key_down(void);
extern void ROS_Condition_Key_up(void);
extern void ROS_Condition_Key_down(void);
extern void Accelerometer_Condition_Key_up(void);
extern void Accelerometer_Condition_Key_down(void);


#endif /* UI_DISPLAY_H_ */
/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
