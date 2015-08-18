/*
 * UI_Display_Time.h
 *
 *  Created on: Sep 29, 2014
 *      Author: Shihab
 */

#ifndef UI_DISPLAY_TIME_H_
#define UI_DISPLAY_TIME_H_

#define SELECTION_MONTH							 0
#define SELECTION_DATE							 1
#define SELECTION_YEAR							 2
#define SELECTION_HOURS						 	 3
#define SELECTION_MINUTES						 4
#define SELECTION_SECONDS					 	 5
#define SELECTION_AM_PM							 6

#define DST_OFF									 0
#define DST_ON									 1
#define TWELVE_HR_FORMAT						 12
#define TWENTYFOUR_HOUR_FORMAT					 24
#define NUM_OF_DATE_TIME_MENU_ITEM 				 5	

#define TIME_AM									 0
#define TIME_PM									 1

#define SET_DATE_TIME_WHILE_BOOTING				 0
#define SET_DATE_TIME_FROM_SETTINGS				 1

#define 	DATE_NOT_SET		0
#define 	DATE_SET			1
#define 	TIME_NOT_SET		0
#define 	TIME_SET			1

typedef struct
{
   
    char Time_Zone_Name[16]; // Name of the timezone
    char UTC_Offset_String[8]; 
    char UTC_DST_Offset_String[8];
    
    int32_t UTC_Offset_Value; 
    int32_t UTC_DST_Offset_Value;
    
} TimeZoneTableEntry;

extern const TimeZoneTableEntry TimeZoneTable[];

extern int32_t Prev_Offset_Value;

void display_Date_Time_Menu(void);
void display_Update_Date_Time_Menu(void);
void Date_Time_menu_Key_up(void);
void Date_Time_menu_Key_down(void);
void Increment_date_time_selection(void);
void Decrement_date_time_selection(void);

void display_Time_Zone_List(void);
void Time_Zone_List_Key_up(void);
void Time_Zone_List_Key_down(void);

void display_time_format_toggle(void);
void display_dst_toggle(void);


//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Date_Time_menu_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_8 Date_Time_update_selection;

//This variable is incremented and decremented according to navigation key 
extern volatile uint_16 Time_Zone_List_selection;

extern volatile uint_8 Time_format;

extern volatile uint_8 Settings_Month_Count;
extern volatile uint_8 Settings_Date_Count;
extern volatile uint_8 Settings_Year_Count;
extern volatile uint_8 Settings_Hours_Count;
extern volatile uint_8 Settings_Minutes_Count;
extern volatile uint_8 Settings_Seconds_Count;
extern volatile uint_8 Settings_AM_PM_Count;

extern uint_8 set_date_time_status;
extern uint_8 set_date_while_booting_count;
extern uint_8 set_time_while_booting_count;


extern char Date_from_settings[];
extern char Time_from_settings[];
//extern char Default_Time_Zone[];

#endif /* UI_DISPLAY_TIME_H_ */
