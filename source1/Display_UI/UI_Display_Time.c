/******************************************************************************
 *
 *   Filename: $UI_Display.c
 *   Created on: $Date: Sept 29, 2014  (4:10:52 PM)
 *   Revision: $1
 *   Author: $Navinar
 *
 *   Description: $This file contains implementation of time related display screens
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
#include "UI_task.h"
#include "UI_Settings.h"
#include "Sd_Card.h"
#include "Data_Processessing_Functions.h"
#include "UI_icon_manipulation.h"
#include "Gps.h"
#include "Sensor_Configuration.h"
#include "UI_Display_Time.h"

void display_Time_Zone_List(void);
static void Create_Time_Zone_List_Content();
void Time_Zone_List_Key_up(void);
void Time_Zone_List_Key_down(void);
static void Add_UTC_Offset_To_Menu(const char *menu_data, uint_8 menu_position);
static void Create_Date_Time_Menu_Content(void);
static void Create_Update_Date_Time_Content(void);

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Date_Time_menu_selection = 0;

//This variable is incremented and decremented according to navigation key 
volatile uint_8 Date_Time_update_selection = 0;

//This variable is either in 12H or 24H format
volatile uint_8 Time_format = TWENTYFOUR_HOUR_FORMAT;

//This variable is incremented and decremented according to navigation key 

volatile uint_8 Settings_Month_Count 	 = 1;
volatile uint_8 Settings_Date_Count	 	 = 1;
volatile uint_8 Settings_Year_Count 	 = 14;
volatile uint_8 Settings_Hours_Count 	 = 0;
volatile uint_8 Settings_Minutes_Count	 = 0;
volatile uint_8 Settings_Seconds_Count   = 0;
volatile uint_8 Settings_AM_PM_Count  	 = TIME_AM;

//char Default_Time_Zone[13]={"US-MOUNTAIN"};
char Date_from_settings[7]={"211014"};
char Time_from_settings[9]={"170521"};

//This variable is incremented and decremented according to navigation key 
volatile uint_16 Time_Zone_List_selection = 0;

//volatile uint_16 Time_Zone_List_count = 0;

uint_8 set_date_time_status = SET_DATE_TIME_FROM_SETTINGS;
uint_8 set_date_while_booting_count = DATE_NOT_SET;
uint_8 set_time_while_booting_count = TIME_NOT_SET;



//Table for displaying Timezone and corresponding values
const TimeZoneTableEntry TimeZoneTable[] = {
		//  display_name,	Display UTC Offset , Display UTC DST Offset     ,UTC Offset,    UTC DST Offset
		{ "ADELAIDE",	 		"+09:30",		 	"+10:30",					+34200,			+37800 		}, 
		{ "ALMATY", 			"+06:00", 			"+06:00", 					+21600,  		+21600		},
		{ "AUCKLAND", 			"+12:00", 			"+13:00", 					+43200,			+46800  	},	
		{ "BANGKOK", 			"+07:00", 			"+07:00", 					+25200,			+25200  	},	
		{ "BOMBAY", 			"+05:30", 			"+05:30", 					+19800,			+19800  	},
		{ "BRASILIA",			"-03:00", 			"-02:00", 					-10800,  		-7200		},
		{ "BRISBANE", 			"+10:00", 			"+10:00", 					+36000,  		+36000		},
		{ "CAIRO", 				"+02:00", 			"+02:00", 					+7200,			+7200		},
		{ "CAPE TOWN", 			"+02:00", 			"+02:00",					+7200,			+7200		},
		{ "CAPE VERDE",			"-01:00", 			"-01:00",					-3600,			-3600		},
		{ "CHITA", 				"+10:00", 			"+10:00", 					+36000,  		+36000		},
		{ "DARWIN", 			"+09:30", 			"+09:30", 					+34200,			+34200		},
		{ "EKATERINBURG", 		"+06:00", 			"+06:00",  					+21600,  		+21600		},
		{ "EU-CENTRAL", 		"+01:00", 			"+02:00",					+3600,			+7200		},
		{ "EU-EASTERN", 		"+02:00", 			"+03:00",					+7200,			+10800		},
		{ "EU-WESTERN", 		"+00:00", 			"+01:00",					+0,				+3600		},
		{ "FIJI", 				"+12:00", 			"+13:00", 					+43200,			+46800  	},	
		{ "HONG KONG", 			"+08:00", 			"+08:00", 					+28800,			+28800		},
		{ "ICELAND", 			"+00:00", 			"+00:00",					+0,				+0			},
		{ "ISLAMABAD", 			"+05:00", 			"+05:00", 					+18000,			+18000  	},
		{ "JAKARTA", 			"+07:00", 			"+07:00", 					+25200,			+25200  	},	
		{ "KABUL", 				"+04:30", 			"+04:30",					+16200,			+16200		},
		{ "KALININGRAD", 		"+03:00", 			"+03:00", 					+10800,			+10800		},
		{ "KATHMANDU", 			"+05:45", 			"+05:45", 					+20700,			+20700  	},
		{ "KRASNOYARSK", 		"+08:00", 			"+08:00", 					+28800,			+28800		},
		{ "LAGOS", 				"+01:00", 			"+01:00", 					+3600,			+3600,		},
		{ "MAGADAN", 			"+12:00", 			"+12:00", 					+43200,			+43200  	},	
		{ "MEX-CENTRAL", 		"-06:00", 			"-05:00", 					-21600,			-18000  	},
		{ "MEX-MOUNTAIN", 		"-07:00", 			"-06:00", 					-25200, 		-21600 		},
		{ "MEX-PACIFIC", 		"-08:00", 			"-07:00", 					-28800,			-25200		},
		{ "MOSCOW", 			"+04:00", 			"+04:00", 					+14400,			+14400		},
		{ "MUSCAT", 			"+04:00", 			"+04:00", 					+14400,			+14400		},
		{ "NEWFOUNDLAND", 		"-03:30", 			"-02:30", 					-12600,			-9000		},
		{ "NOVOSIBIRSK", 		"+07:00", 			"+07:00", 					+25200,			+25200  	},	
		{ "PERTH", 				"+08:00", 			"+08:00", 					+28800,			+28800		},
		{ "PETROPAVLOVSK", 		"+12:00", 			"+12:00", 					+43200,			+43200  	},	
		{ "RIYADH", 			"+03:00", 			"+03:00", 					+10800,			+10800		},
		{ "SAMARA", 			"+04:00", 			"+04:00", 					+14400,			+14400		},
		{ "SAMOA", 				"-11:00", 			"-11:00", 					-39600,			-39600		},
		{ "SYDNEY", 			"+10:00", 			"+11:00", 					+36000,			+39600		},
		{ "TASMANIA", 			"+10:00", 			"+11:00", 					+36000,			+39600		},
		{ "TEHRAN", 			"+03:30", 			"+04:30", 					+12600,			+16200		},
		{ "TOKYO", 				"+09:00", 			"+09:00", 					+32400,			+32400		},
		{ "US-ALASKA", 			"-09:00", 			"-08:00",					-32400,			-28800	    },
		{ "US-ARIZONA", 		"-07:00", 			"-07:00", 					-25200,			-25200  	},	
		{ "US-ATLANTIC", 		"-04:00", 			"-03:00", 					-14400,  		-10800		},
		{ "US-CENTRAL", 		"-06:00", 			"-05:00", 					-21600,  		-18000		},
		{ "US-EASTERN", 		"-05:00", 			"-04:00", 					-18000,			-14400		},
		{ "US-HAWAII", 			"-10:00", 			"-10:00", 					-36000,  		-36000		},
		{ "US-MOUNTAIN", 		"-07:00", 			"-06:00", 					-25200,			-21600  	},	
		{ "US-PACIFIC", 		"-08:00", 			"-07:00", 					-28800,			-25200  	},	
		{ "VLADIVOSTOK", 		"+11:00", 			"+11:00", 					+39600,			+39600		},
		{ "WINDHOEK", 			"+01:00", 			"+02:00", 					+3600,			+7200		}

#define TIME_ZONE_ENTRY_COUNT  (53)	

};



/*-----------------------------------------------------------------------------* 
 * Function:    display_Set_Date_Time_Menu
 * Brief:       This will display date and time screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Date_Time_Menu(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Battery_ADC_Init();
	Create_Header();
	Battery_ADC_Deinit();
	Create_Title("DATE & TIME",strlen("DATE & TIME"));
	Create_Date_Time_Menu_Content();
	if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
	{
		if((set_date_while_booting_count == DATE_SET) && (set_time_while_booting_count == TIME_SET))
		{
			Create_Footer("MENU",strlen("MENU"),"SELECT",strlen("SELECT"));
		}
		else
		{
			Create_Footer("",0,"SELECT",strlen("SELECT"));
			Draw_string_new((240 - Get_string_pixel_length("PLEASE ENTER THE", MEDIUM_FONT))/2 - 10, 280, (uint_8*) "PLEASE ENTER THE", COLOUR_BLACK, MEDIUM_FONT);
			Draw_string_new((240 - Get_string_pixel_length("DATE AND TIME", MEDIUM_FONT))/2 - 10, 305, (uint_8*) "DATE AND TIME", COLOUR_BLACK, MEDIUM_FONT);
		}
	}
	else
	{
		Create_Footer("BACK",strlen("BACK"),"SELECT",strlen("SELECT"));
	}
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Update_Date_Time_Menu
 * Brief:       This will display date and time screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Update_Date_Time_Menu(void)
{
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("DATE & TIME",strlen("DATE & TIME"));
	Create_Update_Date_Time_Content();
	Create_Footer("SAVE",strlen("SAVE"),"NEXT",strlen("NEXT"));
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}

/*-----------------------------------------------------------------------------* 
 * Function:    display_Time_Zone_List
 * Brief:       This will display time zone list screen
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_Time_Zone_List(void)
{
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Header();
	Create_Title("TIME ZONE",strlen("TIME ZONE"));
	Create_Time_Zone_List_Content();
	Create_Footer("CANCEL",strlen("CANCEL"), "SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);

}


/*-----------------------------------------------------------------------------* 
 * Function:    Create_Time_Zone_List_Content
 * Brief:       This will list the Time zones in the LCD buffer.
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Time_Zone_List_Content(void)
{
	uint_8 i=0, Time_Zone_index=0;
	char temp_string[20];
	memset(temp_string,' ',19);
	temp_string[19]=0;
	Time_Zone_index = Time_Zone_List_selection - (Time_Zone_List_selection % NUM_OF_LIST_ITEM_IN_A_VIEW);
	for(i=0; i<NUM_OF_LIST_ITEM_IN_A_VIEW; i++)
	{

		if(i == (Time_Zone_List_selection % NUM_OF_LIST_ITEM_IN_A_VIEW))
		{

			Add_Item_To_Menu(TimeZoneTable[Time_Zone_index].Time_Zone_Name, i, MENU_SELECTED);
		}
		else
		{
			Add_Item_To_Menu(TimeZoneTable[Time_Zone_index].Time_Zone_Name, i, MENU_NOT_SELECTED);
		}

		if(Settings_Data.Dst_status == DST_OFF)
		{
			Add_UTC_Offset_To_Menu(TimeZoneTable[Time_Zone_index].UTC_Offset_String,i);
		}
		else
		{
			Add_UTC_Offset_To_Menu(TimeZoneTable[Time_Zone_index].UTC_DST_Offset_String, i);
		}

		++Time_Zone_index;
		if(Time_Zone_index == TIME_ZONE_ENTRY_COUNT)
		{
			break;
		}

		memset(temp_string,' ', 20);
	}

}



/*-----------------------------------------------------------------------------* 
 * Function:    Time_Zone_List_Key_up
 * Brief:       Decrement Time Zone List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Time_Zone_List_Key_up(void)
{

	if(Time_Zone_List_selection == 0)
	{
		Time_Zone_List_selection = TIME_ZONE_ENTRY_COUNT - 1;
	}
	else
		Time_Zone_List_selection--;

	Time_Zone_List_selection = Time_Zone_List_selection % TIME_ZONE_ENTRY_COUNT;




}

/*-----------------------------------------------------------------------------* 
 * Function:    Time_Zone_List_Key_down
 * Brief:       Increment Time Zone List selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Time_Zone_List_Key_down(void)
{

	if(Time_Zone_List_selection == TIME_ZONE_ENTRY_COUNT)
	{
		Time_Zone_List_selection = 0;
	}
	else
		Time_Zone_List_selection++;

	Time_Zone_List_selection = Time_Zone_List_selection % TIME_ZONE_ENTRY_COUNT;

}



/*-----------------------------------------------------------------------------* 
 * Function:    Add_UTC_Offset_To_Menu
 * Brief:       This will add an item in to the list
 * Parameter:   display name ,position, selection status
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Add_UTC_Offset_To_Menu(const char *menu_data, uint_8 menu_position)
{
	uint_16 y_position_min;
	char menu_Text[8];
	memset(menu_Text,0x00,8);
	strncpy(menu_Text,menu_data,6);
	if(strlen(menu_Text)>6)
	{
		printf("\nADD ITEM TO MENU NAME OF STRING EXCEEDED SIZE LIMIT; SIZE IS %d", strlen(menu_Text));
		menu_Text[6]=0;
	}

	y_position_min = DISPLAY_CONTAINER_Y_MIN + (menu_position * DISPLAY_MENU_WIDTH);
	Draw_string_new((150 + NUM_X_PIXEL_PER_CHAR),
			(y_position_min + (DISPLAY_MENU_WIDTH / 4)), (uint_8 *) menu_Text, COLOUR_WHITE ,MEDIUM_FONT);  

}


/*-----------------------------------------------------------------------------* 
 * Function:    display_dst_toggle
 * Brief:       Turn dst on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_dst_toggle(void)
{
	if(Settings_Data.Dst_status == DST_ON)
	{
		Settings_Data.Dst_status = DST_OFF;
		Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_Value
				- TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_Value);
	}
	else
	{
		Settings_Data.Dst_status = DST_ON;
		Set_Time_Zone(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_Value
				- TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_Value);
	}
	Write_Data_file_information_To_Sdcard(); //Store settings to sd card
	display_Date_Time_Menu();
}

/*-----------------------------------------------------------------------------* 
 * Function:    display_time_format_toggle
 * Brief:       Turn time format on or off
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void display_time_format_toggle(void)
{
//	if(Time_format == TWELVE_HR_FORMAT)
//	{
//		Time_format = TWENTYFOUR_HOUR_FORMAT;
//		if(Settings_AM_PM_Count == TIME_PM)
//		{
//			Settings_Hours_Count= (Settings_Hours_Count +12)%24;
//		}
//	}
//	else if(Time_format == TWENTYFOUR_HOUR_FORMAT)
//	{
//		Time_format = TWELVE_HR_FORMAT;
//		if(Settings_Hours_Count >12)
//		{
//			Settings_Hours_Count = Settings_Hours_Count-12;
//			Settings_AM_PM_Count = TIME_PM;
//		}
//		else
//		{
//			Settings_AM_PM_Count = TIME_AM;
//		}
//	}
	
	if(Time_format == TWELVE_HR_FORMAT)
	{
		Time_format = TWENTYFOUR_HOUR_FORMAT;
	}
	else if(Time_format == TWENTYFOUR_HOUR_FORMAT)
	{
		Time_format = TWELVE_HR_FORMAT;
	}
	
	update_system_date_time_from_rtc();
	display_Date_Time_Menu();
}


/*-----------------------------------------------------------------------------* 
 * Function:    Date_Time_menu_Key_up
 * Brief:       decrement Date_Time menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Date_Time_menu_Key_up(void)
{
	if (Date_Time_menu_selection == 0)
		Date_Time_menu_selection = NUM_OF_DATE_TIME_MENU_ITEM - 1;
	else
		Date_Time_menu_selection--;

	Date_Time_menu_selection = Date_Time_menu_selection % NUM_OF_DATE_TIME_MENU_ITEM;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Date_Time_menu_Key_down
 * Brief:       increment Date_Time menu selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Date_Time_menu_Key_down(void)
{
	Date_Time_menu_selection++;
	Date_Time_menu_selection = Date_Time_menu_selection % NUM_OF_DATE_TIME_MENU_ITEM;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Increment_date_time_selection
 * Brief:       Increment date and time selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Increment_date_time_selection(void)
{
	switch(Date_Time_update_selection)
	{
	case SELECTION_MONTH:
		if(Settings_Month_Count == 12)
		{
			Settings_Month_Count = 1;
		}
		else
		{
			++Settings_Month_Count;
		}
		break;

	case SELECTION_DATE:
		if((Settings_Month_Count == 1) || (Settings_Month_Count == 3) || (Settings_Month_Count == 5) || (Settings_Month_Count == 7) || 
				(Settings_Month_Count == 8) || (Settings_Month_Count == 10) || (Settings_Month_Count == 12))
		{
			if(Settings_Date_Count == 31)
				Settings_Date_Count = 1;
			else
				++Settings_Date_Count;
		}
		else if((Settings_Month_Count == 4) || (Settings_Month_Count == 6) || (Settings_Month_Count == 9) || (Settings_Month_Count == 11))
		{
			if(Settings_Date_Count == 30)
				Settings_Date_Count = 1;
			else
				++Settings_Date_Count;
		}
		else if(Settings_Month_Count == 2)
		{
			if(SELECTION_YEAR % 4 == 0)
			{
				if(Settings_Date_Count == 29)
					Settings_Date_Count = 1;
				else
					++Settings_Date_Count;
			}
			else 
			{
				if(Settings_Date_Count == 28)
					Settings_Date_Count = 1;
				else
					++Settings_Date_Count;
			}

		}
		break;

	case SELECTION_YEAR:
		if(Settings_Year_Count == 99)
		{
			Settings_Year_Count = 0;
		}
		else
		{
			++Settings_Year_Count;
		}
		break;

	case SELECTION_HOURS:
		if(Time_format == TWELVE_HR_FORMAT)
		{
			if(Settings_Hours_Count == 12)
			{
				Settings_Hours_Count = 1;
			}
			else
			{
				++Settings_Hours_Count;
			}
		}
		else
		{
			if(Settings_Hours_Count == 23)
			{
				Settings_Hours_Count = 0;
			}
			else
			{
				++Settings_Hours_Count;
			}
		}

		break;

	case SELECTION_MINUTES:
		if(Settings_Minutes_Count == 59)
		{
			Settings_Minutes_Count = 0;
		}
		else
		{
			++Settings_Minutes_Count;
		}
		break;

	case SELECTION_SECONDS:
		if(Settings_Seconds_Count == 59)
		{
			Settings_Seconds_Count = 0;
		}
		else
		{
			++Settings_Seconds_Count;
		}
		break;

	case SELECTION_AM_PM:
		if(Settings_AM_PM_Count == TIME_AM)
		{
			Settings_AM_PM_Count = TIME_PM;
		}
		else
		{
			Settings_AM_PM_Count=TIME_AM;
		}
		break;

	default:
		break;
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Decrement_date_time_selection
 * Brief:       Decrement date and time selection
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
void Decrement_date_time_selection(void)
{
	switch(Date_Time_update_selection)
	{
	case SELECTION_MONTH:
		if(Settings_Month_Count == 1)
			Settings_Month_Count = 12;
		else
			--Settings_Month_Count;
		break;

	case SELECTION_DATE:
		if((Settings_Month_Count == 1) || (Settings_Month_Count == 3) || (Settings_Month_Count == 5) || (Settings_Month_Count == 7) || 
				(Settings_Month_Count == 8) || (Settings_Month_Count == 10) || (Settings_Month_Count == 12))
		{
			if(Settings_Date_Count == 1)
				Settings_Date_Count = 31;
			else
				--Settings_Date_Count;
		}
		else if((Settings_Month_Count == 4) || (Settings_Month_Count == 6) || (Settings_Month_Count == 9) || (Settings_Month_Count == 11))
		{
			if(Settings_Date_Count == 1)
				Settings_Date_Count = 30;
			else
				--Settings_Date_Count;
		}
		else if(Settings_Month_Count == 2)
		{
			if(SELECTION_YEAR % 4 == 0)
			{
				if(Settings_Date_Count == 1)
					Settings_Date_Count = 29;
				else
					++Settings_Date_Count;
			}
			else 
			{
				if(Settings_Date_Count == 1)
					Settings_Date_Count = 28;
				else
					--Settings_Date_Count;
			}

		}
		break;

	case SELECTION_YEAR:
		if(Settings_Year_Count == 0)
		{
			Settings_Year_Count = 99;
		}
		else
		{
			--Settings_Year_Count;
		}
		break;

	case SELECTION_HOURS:
		if(Time_format == TWELVE_HR_FORMAT)
		{
			if((Settings_Hours_Count == 1) || (Settings_Hours_Count == 0))
			{
				Settings_Hours_Count = 12;
			}
			else
			{
				--Settings_Hours_Count;
			}
		}
		else
		{
			if(Settings_Hours_Count == 0)
			{
				Settings_Hours_Count = 23;
			}
			else
			{
				--Settings_Hours_Count;
			}
		}
		break;

	case SELECTION_MINUTES:
		if(Settings_Minutes_Count == 0)
		{
			Settings_Minutes_Count = 59;
		}
		else
		{
			--Settings_Minutes_Count;
		}
		break;

	case SELECTION_SECONDS:
		if(Settings_Seconds_Count == 0)
		{
			Settings_Seconds_Count = 59;
		}
		else
		{
			--Settings_Seconds_Count;
		}
		break;

	case SELECTION_AM_PM:
		if(Settings_AM_PM_Count == TIME_AM)
		{
			Settings_AM_PM_Count = TIME_PM;
		}
		else
		{
			Settings_AM_PM_Count=TIME_AM;
		}
		break;

	default:
		break;
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Date_Time_Menu_Content
 * Brief:       This will create date and time display content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Date_Time_Menu_Content(void)
{
	for (uint_8 i = 0; i < NUM_OF_DATE_TIME_MENU_ITEM; i++)
	{
		if (i == Date_Time_menu_selection)
		{
			Add_Item_To_Menu(Date_Time_menu_Table[i].Menu_String, i, MENU_SELECTED);
		}
		else
		{
			Add_Item_To_Menu(Date_Time_menu_Table[i].Menu_String, i, MENU_NOT_SELECTED);
		}

	}


	uint_8 x_position, y_position;
	x_position = (DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*5))-25;
	if((x_position < 1) || (x_position >240))
	{
		printf("\n DATE AND TIME SCREEN ADD ITEMS TO MENU LENGTH ERROR.... BUFFER SIZE EXCEEDED!! ");
		while(1)
		{

		}
	}

	y_position = 58;
	Draw_string_new(DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name, MEDIUM_FONT)
			- (strlen(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name)*2)-5,
			y_position+(DISPLAY_MENU_WIDTH*3), TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name, COLOUR_WHITE, MEDIUM_FONT);	

	y_position = 60;

	Draw_favourites_icons(x_position, y_position+(DISPLAY_MENU_WIDTH), GRAY,  COLOUR_WHITE);
	Draw_favourites_icons(x_position, y_position+(DISPLAY_MENU_WIDTH*4), GRAY,  COLOUR_WHITE);

	//Draw icons content
	y_position = y_position - 2;

	if(Settings_Data.Dst_status == DST_OFF)
	{

		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+(DISPLAY_MENU_WIDTH*4), (uint_8 *) "ON", COLOUR_BLACK,MEDIUM_FONT);	
	}
	else
	{

		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+(DISPLAY_MENU_WIDTH*4), (uint_8 *) "OFF", COLOUR_BLACK,MEDIUM_FONT);
	}

	if(Time_format == TWELVE_HR_FORMAT)
	{
		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+DISPLAY_MENU_WIDTH, (uint_8 *) "12H", COLOUR_BLACK,MEDIUM_FONT);
	}
	else if(Time_format == TWENTYFOUR_HOUR_FORMAT)
	{
		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+DISPLAY_MENU_WIDTH, (uint_8 *) "24H", COLOUR_BLACK,MEDIUM_FONT);
	}

	char tempString[16];

	if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
	{
		//DIsplay Date
		if(set_date_while_booting_count == DATE_NOT_SET)
		{
			sprintf(tempString, "--/--/--");
			Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
		}
		else
		{
			sprintf(tempString, "%02d/%02d/%02d", Settings_Month_Count,Settings_Date_Count,Settings_Year_Count);
			Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
		}
		//Display Time
		if(set_time_while_booting_count == TIME_NOT_SET)
		{
			sprintf(tempString, "--:--:--");
		}
		else
		{
			sprintf(tempString, "%02d:%02d:%02d", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
		}
	}
	else
	{
		//Display Date
		sprintf(tempString, "%02d/%02d/%02d", Settings_Month_Count,Settings_Date_Count,Settings_Year_Count);
		Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
		//Display Time
		sprintf(tempString, "%02d:%02d:%02d", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
	}

	if(Time_format == TWELVE_HR_FORMAT)
	{
		if(Settings_AM_PM_Count == TIME_AM)
		{
			strcat(tempString, " AM");
		}
		else
		{
			strcat(tempString, " PM");
		}
		Draw_string_new(115, 138, tempString, COLOUR_WHITE, MEDIUM_FONT);
	}
	else
	{
		Draw_string_new(152, 138, tempString, COLOUR_WHITE, MEDIUM_FONT);
	}

}

/*-----------------------------------------------------------------------------* 
 * Function:    Create_Update_Date_Time_Content
 * Brief:       This will update date and time display content
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Create_Update_Date_Time_Content(void)
{
	char tempString[16];

	for (uint_8 i = 0; i < NUM_OF_DATE_TIME_MENU_ITEM; i++)
	{

		Add_Item_To_Menu(Date_Time_menu_Table[i].Menu_String, i, MENU_NOT_SELECTED);

	}


	uint_8 x_position, y_position;
	x_position = DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*5)-25;
	if((x_position < 1) || (x_position >240))
	{
		printf("\n DATE AND TIME SCREEN ADD ITEMS TO MENU LENGTH ERROR.... BUFFER SIZE EXCEEDED!! ");
		while(1)
		{

		}
	}

	y_position = 58;
	Draw_string_new(DISPLAY_CONTAINER_X_MAX - Get_string_pixel_length(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name, MEDIUM_FONT)
			- (strlen(TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name)*2) - 5,
			y_position+(DISPLAY_MENU_WIDTH*3), TimeZoneTable[Settings_Data.Prev_Zone_List_selection].Time_Zone_Name, COLOUR_WHITE, MEDIUM_FONT);

	y_position = 60;

	Draw_favourites_icons(x_position, y_position+(DISPLAY_MENU_WIDTH), GRAY,  COLOUR_WHITE);
	Draw_favourites_icons(x_position, y_position+(DISPLAY_MENU_WIDTH*4), GRAY,  COLOUR_WHITE);

	//Draw icons content
	y_position = y_position - 2;

	if(Settings_Data.Dst_status == DST_OFF)
	{

		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+(DISPLAY_MENU_WIDTH*4), (uint_8 *) "ON", COLOUR_BLACK,MEDIUM_FONT);	
	}
	else
	{

		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+(DISPLAY_MENU_WIDTH*4), (uint_8 *) "OFF", COLOUR_BLACK,MEDIUM_FONT);
	}

	if(Time_format == TWELVE_HR_FORMAT)
	{
		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+DISPLAY_MENU_WIDTH, (uint_8 *) "12H", COLOUR_BLACK,MEDIUM_FONT);
	}
	else if(Time_format == TWENTYFOUR_HOUR_FORMAT)
	{
		Draw_string_new((DISPLAY_CONTAINER_X_MAX - (NUM_X_PIXEL_PER_CHAR*4)),
				y_position+DISPLAY_MENU_WIDTH, (uint_8 *) "24H", COLOUR_BLACK,MEDIUM_FONT);
	}

	x_position = 132;
	y_position = 60;

	if(State_of_Screen == UI_UPDATE_DATE)
	{
		if(Date_Time_update_selection == 0)
			Rect_fill_with_horizontal_lines(x_position, y_position,x_position+26,y_position + 25, COLOUR_BLACK);
		else if(Date_Time_update_selection == 1)
			Rect_fill_with_horizontal_lines(x_position+40,y_position,x_position+65,y_position + 25, COLOUR_BLACK);
		else if(Date_Time_update_selection ==2)
			Rect_fill_with_horizontal_lines(x_position+78,y_position,x_position+105,y_position + 25, COLOUR_BLACK);

	}

	if(set_date_time_status == SET_DATE_TIME_WHILE_BOOTING)
	{
		//DIsplay Date

		if(State_of_Screen == UI_UPDATE_DATE)
		{
			sprintf(tempString, "%02d/%02d/%02d", Settings_Month_Count,Settings_Date_Count,Settings_Year_Count);
			Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
			if(set_time_while_booting_count == TIME_NOT_SET)
			{
				sprintf(tempString, "--:--:--");
			}
			else
			{
				sprintf(tempString, "%02d:%02d:%02d", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
			}
		}
		else if(State_of_Screen == UI_UPDATE_TIME)
		{
			if(set_date_while_booting_count == DATE_NOT_SET)
			{
				sprintf(tempString, "--/--/--");
				Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
			}
			else
			{
				sprintf(tempString, "%02d/%02d/%02d", Settings_Month_Count,Settings_Date_Count,Settings_Year_Count);
				Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
			}
			sprintf(tempString, "%02d:%02d:%02d", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
		}
		//Display Time
	}
	else
	{
		//Display Date
		sprintf(tempString, "%02d/%02d/%02d", Settings_Month_Count,Settings_Date_Count,Settings_Year_Count);
		Draw_string_new(135, 58, tempString, COLOUR_WHITE, MEDIUM_FONT);
		//Display Time
		sprintf(tempString, "%02d:%02d:%02d", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
	}



	if(Time_format == TWELVE_HR_FORMAT)
	{
		x_position = 114;
	}
	else
	{
		x_position = 150;
	}
	y_position = 140;

	if(State_of_Screen == UI_UPDATE_TIME)
	{
		if(Date_Time_update_selection == 3)
			Rect_fill_with_horizontal_lines(x_position,y_position,x_position+26,y_position+25, COLOUR_BLACK);
		else if(Date_Time_update_selection == 4)
			Rect_fill_with_horizontal_lines(x_position+30,y_position,x_position+56,y_position+25, COLOUR_BLACK);
		else if(Date_Time_update_selection == 5)
			Rect_fill_with_horizontal_lines(x_position+60,y_position,x_position+84,y_position+25, COLOUR_BLACK);
		else if(Date_Time_update_selection == 6)
			Rect_fill_with_horizontal_lines(x_position+95,y_position,x_position+125,y_position+25, COLOUR_BLACK);	
	}

	if(Time_format == TWELVE_HR_FORMAT)
	{
		if(Settings_AM_PM_Count == TIME_AM)
		{
			sprintf(tempString, "%02d:%02d:%02d AM", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
			sprintf(Time_from_settings, "%02d%02d%02d00", Settings_Hours_Count%12, Settings_Minutes_Count, Settings_Seconds_Count);
		}
		else
		{
			sprintf(tempString, "%02d:%02d:%02d PM", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
			if(Settings_Hours_Count != 12)
				sprintf(Time_from_settings, "%02d%02d%02d00", (Settings_Hours_Count+12)%24, Settings_Minutes_Count, Settings_Seconds_Count);
			else
				sprintf(Time_from_settings, "%02d%02d%02d00", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);
		}
		Draw_string_new(x_position + 1, y_position-2, tempString, COLOUR_WHITE, MEDIUM_FONT);
	}
	else
	{
		Draw_string_new(x_position+1, y_position-2, tempString, COLOUR_WHITE, MEDIUM_FONT);
		sprintf(Time_from_settings, "%02d%02d%02d00", Settings_Hours_Count, Settings_Minutes_Count, Settings_Seconds_Count);

	}
	printf("\nTIME: %s", Time_from_settings);

	sprintf(Date_from_settings, "%02d%02d%02d", Settings_Date_Count,Settings_Month_Count,Settings_Year_Count);
	printf("\nDATE: %s", Date_from_settings);
}
