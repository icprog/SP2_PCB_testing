/*
 * Firmware_Update.c
 *
 *  Created on: Nov 19, 2014
 *      Author: Rahul
 */


#include "common_headers.h"

extern uint8_t Check_Firmware_Update();
extern void Display_Firmware_Update_Available();
extern void Display_Firmware_Updating();

uint8_t Check_Firmware_Update()
{
	MQX_FILE_PTR fd_ptr; 
	uint_16 date_word=0, time_word=0;
	MFS_DATE_TIME_PARAM date;
	date.DATE_PTR = &date_word;
	date.TIME_PTR = &time_word;
	
	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:image.bin", "r");
	if (fd_ptr == NULL)
	{
		printf("No image file found\n");
		return 0;
	}
	ioctl(fd_ptr, IO_IOCTL_GET_DATE_TIME,(uint_32_ptr) &date);
	
	printf ("%02lu-%02lu-%04lu \n",
	(uint_32)(date_word & MFS_MASK_MONTH) >> MFS_SHIFT_MONTH,
	(uint_32)(date_word & MFS_MASK_DAY) >> MFS_SHIFT_DAY,
	(uint_32)((date_word & MFS_MASK_YEAR) >> MFS_SHIFT_YEAR)
	+ 1980);
	
	if((date_word == img_date_word)&&(time_word == img_time_word))
	{
		printf("No Update available\n");
		fclose(fd_ptr);
		return 0;
	}
	else
	{
		printf("Update available\n");
		fclose(fd_ptr);
		return 1;
	}	
}

void Display_Firmware_Update_Available()
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
	Create_Header();
	Create_Title("FIRMWARE UPDATE",strlen("FIRMWARE UPDATE"));

	Draw_string_new(45, 110, (uint_8 *) "NEW FIRMWARE", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(55, 145, (uint_8 *) "  DETECTED", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(50, 210, (uint_8 *) "PRESS SELECT", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(50, 245,(uint_8 *)  "  TO REBOOT", COLOUR_BLACK, MEDIUM_FONT);

	Create_Footer("",0,"SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

