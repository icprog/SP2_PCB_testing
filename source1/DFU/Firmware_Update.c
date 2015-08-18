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
extern void Display_Validating_Firmware();
extern void Display_Firmware_Corrupted();
extern uint8_t File_CRC16();

#define CRC_POLY 0x1021 //CRC polynomiyal
#define BLOCK_SIZE			4096

uint8_t Read_Buff[4100];

uint8_t File_CRC16(char Image_File_Name[12])
{
	uint16_t i;
	uint32_t num=0,ctr=0;
	uint32_t crc = 0xFFFF;//starting CRC
	uint16_t file_crc=0;
	uint_32 File_Size=0;
	MQX_FILE_PTR fd_ptr; 
	/* Opening settings file from SD card*/
	fd_ptr = fopen(Image_File_Name, "r");
	if (fd_ptr == NULL)
	{
		printf("No image file found\n");
		return 0;
	}
	fseek(fd_ptr, -2, IO_SEEK_END);
	File_Size = ftell(fd_ptr);
	read(fd_ptr, Read_Buff, 2);
	file_crc = ((Read_Buff[0]<<8)|(Read_Buff[1]));
	// seek to the beginning of the file
	fseek(fd_ptr, 0, IO_SEEK_SET);

	while(1)
	{
		if(File_Size >= BLOCK_SIZE)
		{
			// setting number of bytes to read to a block size.
			File_Size = File_Size - BLOCK_SIZE;
			num = BLOCK_SIZE;
		}
		else
		{
			// saving remaing bytes to read
			num = File_Size;
		}
		if(read(fd_ptr, Read_Buff, num)!=num)
		{
			fclose(fd_ptr);
			printf("read error\n");
			return 0;	
		}
		for (ctr=0 ; ctr<num ; ctr++)               /* Step through bytes in memory */
		{
			//		printf("%02X\n",Read_Buff[0]);
			crc = (crc ^ (Read_Buff[ctr] << 8));      /* Fetch byte from memory, XOR into CRC top byte*/
			for (i = 0; i < 8; i++)              /* Prepare to rotate 8 bits */
			{
				crc = (crc << 1);                /* rotate */
				if (crc & 0x10000)             /* bit 15 was set (now bit 16)... */
					crc = ((crc ^ CRC_POLY) & 0xFFFF); /* XOR with XMODEM polynomic */
				/* and ensure CRC remains 16-bit value */
			}                              /* Loop for 8 bits */
		}                                /* Loop until num=0 */
		if(num<BLOCK_SIZE)
		{
			break;
		}
	}
	if(crc == file_crc)
	{
		printf("CRC Pass\n");
		fclose(fd_ptr);
		return 1;
	}
	else
	{
		printf("CRC Fail\nFile CRC = %02X %02X\nCalculated CRC = %02X %02X\n",((file_crc&0xFF00)>>8),
				(file_crc&0xFF),((crc&0xFF00)>>8),(crc&0xFF));
		fclose(fd_ptr);
		if(_io_ioctl(filesystem_handle,	IO_IOCTL_DELETE_FILE,(uint_32_ptr)Image_File_Name)==MQX_OK)
		{
			printf("File Deleted\n");
		}
		else
		{
			printf("File Not Deleted\n");
		}
		return 0;	
	}
}

uint8_t Find_CRC(void)
{
	if(SP1_IMG.Update_Status == YES)
	{
		SP1_IMG.CRC_Status   = File_CRC16("a:image.bin");
	}
	if(CALIB_IMG.Update_Status == YES)
	{
		CALIB_IMG.CRC_Status = File_CRC16("a:calib.bin");
	}

	
	if((SP1_IMG.CRC_Status == YES) || (CALIB_IMG.CRC_Status == YES))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t Check_Firmware_Update()
{
	MQX_FILE_PTR fd_ptr; 
	uint_16 date_word=0, time_word=0;
	MFS_DATE_TIME_PARAM date;
	date.DATE_PTR = &date_word;
	date.TIME_PTR = &time_word;
	
	SP1_IMG.CRC_Status = 0;SP1_IMG.Update_Status=0;
	CALIB_IMG.CRC_Status = 0;CALIB_IMG.Update_Status=0;	

	/* Checking for SP1 image.bin*/
	fd_ptr = fopen("a:image.bin", "r");
	if (fd_ptr == NULL)
	{
		printf("image.bin file not found\n");
		//		return 0;
	}
	else
	{
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
			SP1_IMG.Update_Status = 0;
		}
		else
		{
			printf("Update available\n");
			fclose(fd_ptr);
			SP1_IMG.Update_Status = 1;
		}	
	}

	date_word=0, time_word=0;
	/* Checking for Calib image.bin*/
	fd_ptr = fopen("a:calib.bin", "r");
	if (fd_ptr == NULL)
	{
		printf("calib.bin file not found\n");
	}
	else
	{
		ioctl(fd_ptr, IO_IOCTL_GET_DATE_TIME,(uint_32_ptr) &date);

		printf ("%02lu-%02lu-%04lu \n",
				(uint_32)(date_word & MFS_MASK_MONTH) >> MFS_SHIFT_MONTH,
				(uint_32)(date_word & MFS_MASK_DAY) >> MFS_SHIFT_DAY,
				(uint_32)((date_word & MFS_MASK_YEAR) >> MFS_SHIFT_YEAR)
				+ 1980);

		if((date_word == calib_date_word)&&(time_word == calib_time_word))
		{
			printf("No Update available\n");
			fclose(fd_ptr);
			CALIB_IMG.Update_Status = 0;
		}
		else
		{
			printf("Update available\n");
			fclose(fd_ptr);
			CALIB_IMG.Update_Status = 1;
		}	
	}

	if((SP1_IMG.Update_Status == 1) || (CALIB_IMG.Update_Status == 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void Display_Firmware_Update_Available()
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) Default_background);
//	Create_Header();
	Create_Title("FIRMWARE UPDATE",strlen("FIRMWARE UPDATE"));

	Draw_string_new(45, 110, (uint_8 *) "NEW FIRMWARE", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(55, 145, (uint_8 *) "  DETECTED", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(50, 210, (uint_8 *) "PRESS SELECT", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(50, 245,(uint_8 *)  "  TO REBOOT", COLOUR_BLACK, MEDIUM_FONT);

	Create_Footer("",0,"SELECT",strlen("SELECT"));

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

void Display_Firmware_Corrupted()
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) left_footer_background);
//	Create_Header();
	Create_Title("FIRMWARE UPDATE",strlen("FIRMWARE UPDATE"));

	Draw_string_new(35, 110, (uint_8 *) "FIRMWARE UPDATE", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(60, 145, (uint_8 *) "  FAILED", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(60, 205, (uint_8 *) "  PLEASE", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(35, 240, (uint_8 *) "RECONNECT TO PC", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(35, 275, (uint_8 *) " AND TRY AGAIN", COLOUR_BLACK, MEDIUM_FONT);
	
	Create_Footer("BACK",strlen("BACK"),"",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}

void Display_Validating_Firmware()
{

	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
//	Create_Header();
	Create_Title("FIRMWARE UPDATE",strlen("FIRMWARE UPDATE"));

	Draw_string_new(55, 145, (uint_8 *) "  VALIDATING", COLOUR_BLACK, MEDIUM_FONT);
	Draw_string_new(45, 180, (uint_8 *) "NEW FIRMWARE", COLOUR_BLACK, MEDIUM_FONT);

	Create_Footer("",0,"",0);

	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
}
