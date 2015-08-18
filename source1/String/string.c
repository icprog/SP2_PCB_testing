/******************************************************************************
 *
 *     Filename:       $string.c
 *     Created on:     $Date: Mar 25, 2014    (6:31:32 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contain functions to write sting to lcd
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "spi_config.h"
#include "font.h"
#include "spi_lcd.h"
#include "UI_Display.h"
#include "UI_icon_manipulation.h"

extern volatile uint_8 Flip_Status;

void Draw_loading_image(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour)			//16x16
{
	uint_16 x_temp = 0, ytemp = 0, buff_index = 0, font = 0,pix_height;
	uint_16 pix=0,pix_width=0, font_index = 0, y_temp_adjustments = 0, y_temp=0;
	uint_8  count=0;


	X = X_MAX + 1 - X;
	pix_height=8;

	y_temp = (Y - 1) / 8;	//Current Byte position
	//    y_temp_adjustments = ((pix_height + (Y - 1))) % 8;
	y_temp_adjustments = 0;

	pix_width= 2592;

	x_temp = X-1-144;

	for (font_index = 0; font_index < pix_width; )
	{
		if (pixel_colour == COLOUR_BLACK)
		{
			buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
			if (y_temp_adjustments == 0)
			{
				switch(data)
				{
				case LOADING_IMAGE_NO_BLACK:                     
					for(pix=0;pix<18;pix++)
					{
						frame_buff[buff_index++] = loading_image_none[font_index++];
					}
					break;
				case LOADING_IMAGE_ONE_BLACK:
					for(pix=0;pix<18;pix++)
					{
						frame_buff[buff_index++] = loading_image_one[font_index++];
					}
					break;
				case LOADING_IMAGE_TWO_BLACK:
					for(pix=0;pix<18;pix++)
					{
						frame_buff[buff_index++] = loading_image_two[font_index++];
					}
					break;
				case LOADING_IMAGE_THREE_BLACK:
					for(pix=0;pix<18;pix++)
					{
						frame_buff[buff_index++] = loading_image_three[font_index++];
					}
					break;

				case LOADING_IMAGE_ALL_BLACK:
					for(pix=0;pix<18;pix++)
					{
						frame_buff[buff_index++] = loading_image_all[font_index++];
					}
					break;
				default: 
					break;

				}
			}
		}
	}
}

void Draw_image(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour)			//16x16
{
	uint_16 x_temp = 0, ytemp = 0, buff_index = 0, font = 0,pix_height;
	uint_16 pix=0,pix_width=0, font_index = 0, y_temp_adjustments = 0, y_temp=0;
	uint_8 count=0;


	X = X_MAX + 1 - X;

	y_temp = (Y - 1) / 8;	//Current Byte position
	//    y_temp_adjustments = ((pix_height + (Y - 1))) % 8;
	y_temp_adjustments = 0;


	if(data==COMPASS_ALL_ICON_NUM)
	{
		pix_height= NUM_Y_COMPASS_ALL_ICON/8 ;
		pix_width = NUM_X_COMPASS_ALL_ICON;
		x_temp = X-1 - ((NUM_X_COMPASS_ALL_ICON / NUM_Y_COMPASS_ALL_ICON)* 8);
	}
	else if(data==COMPASS_ROLL_ICON_NUM)
	{
		pix_height= NUM_Y_COMPASS_ROLL_ICON/8;
		pix_width = NUM_X_COMPASS_ROLL_ICON;
		x_temp = X-1 - ((NUM_X_COMPASS_ROLL_ICON / NUM_Y_COMPASS_ROLL_ICON)* 8);
	}
	else if(data==COMPASS_SPIN_ICON_NUM)
	{
		pix_height= NUM_Y_COMPASS_SPIN_ICON/8;
		pix_width = NUM_X_COMPASS_SPIN_ICON;
		x_temp = X-1 - ((NUM_X_COMPASS_SPIN_ICON / NUM_Y_COMPASS_SPIN_ICON)* 8);
	}
	else if(data==COMPASS_FLIP_ICON_NUM)
	{
		pix_height = NUM_Y_COMPASS_FLIP_ICON/8;
		pix_width = NUM_X_COMPASS_FLIP_ICON;
		x_temp = X-1 - ((NUM_X_COMPASS_FLIP_ICON / NUM_Y_COMPASS_FLIP_ICON)* 8);
	}

	for (font_index = 0; font_index < pix_width; )
	{
		buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
		switch(data)
		{
		case COMPASS_ALL_ICON_NUM:                     
			for(pix=0;pix<pix_height;pix++)
			{
				frame_buff[buff_index++] = compass_all_icon[font_index++];
			}
			break;

		case COMPASS_ROLL_ICON_NUM: 
			for(pix=0;pix<pix_height;pix++)
			{
				frame_buff[buff_index++] = compass_roll_icon[font_index++];
			}
			break;

		case COMPASS_SPIN_ICON_NUM: 
			for(pix=0;pix<pix_height;pix++)
			{
				frame_buff[buff_index++] = compass_spin_icon[font_index++];
			}
			break;

		case COMPASS_FLIP_ICON_NUM: 
			for(pix=0;pix<pix_height;pix++)
			{
				frame_buff[buff_index++] = compass_flip_icon[font_index++];
			}
			break;

		default: 
			break;
		}
	}
}

void Draw_header_icons(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour)			//16x16
{
	uint_16 x_temp = 0, ytemp = 0, buff_index = 0, font = 0,pix_height;
	uint_16 pix=0,pix_width=0, font_index = 0, y_temp_adjustments = 0, y_temp=0;
	uint_8 tempData[3], count=0;


	X = X_MAX + 1 - X;
	pix_height=16;

	y_temp = (Y - 1) / 8;	//Current Byte position
	y_temp_adjustments = ((pix_height + (Y - 1))) % 8;

	if(data==BATTERY_FULL_ICON)
	{
		pix_width= NUM_X_PIXEL_BATTERY_ICON * 2;
	}
	else if(data==BATTERY_HALF_ICON)
	{
		pix_width= NUM_X_PIXEL_BATTERY_ICON * 2;
	}
	else if(data==BATTERY_EMPTY_ICON)
	{
		pix_width= NUM_X_PIXEL_BATTERY_ICON * 2;
	}
	else if(data==BLUETOOTH_CONNECTED_ICON)
	{
		pix_width= NUM_X_PIXEL_BLUETOOTH_ICON * 2;
	}
	else if(data==GPS_LINKED_ICON)
	{
		pix_width= NUM_X_PIXEL_GPS_ICON * 2;
	} 


	x_temp = X-1 - (pix_width/2);

	for (font_index = 0; font_index < pix_width; )
	{
		if (pixel_colour == COLOUR_BLACK)
		{
			buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
			if (y_temp_adjustments == 0)
			{
				switch(data)
				{
				case BATTERY_HALF_ICON:                     
					frame_buff[buff_index++] |= battery_icon[font_index];
					frame_buff[buff_index++] |= battery_icon[++font_index];
					break;
				case BATTERY_FULL_ICON:                     
					frame_buff[buff_index++] |= battery_full_icon[font_index];
					frame_buff[buff_index++] |= battery_full_icon[++font_index];
					break;
				case BATTERY_EMPTY_ICON:                     
					frame_buff[buff_index++] |= battery_empty_icon[font_index];
					frame_buff[buff_index++] |= battery_empty_icon[++font_index];
					break;
				case BLUETOOTH_CONNECTED_ICON:
					frame_buff[buff_index++] |= bluetooth_icon[font_index];
					frame_buff[buff_index++] |= bluetooth_icon[++font_index];
					break;
				case GPS_LINKED_ICON:
					frame_buff[buff_index++] |= gps_icon[font_index];
					frame_buff[buff_index++] |= gps_icon[++font_index];
					break;
				default:	break;

				}


				++font_index;
			}
			else
			{


				switch(data)
				{
				case BATTERY_HALF_ICON:                     
					tempData[0]  = battery_icon[font_index];
					tempData[1]  = battery_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BATTERY_FULL_ICON:                     
					tempData[0]  = battery_full_icon[font_index];
					tempData[1]  = battery_full_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BATTERY_EMPTY_ICON:           
					tempData[0]  = battery_empty_icon[font_index];
					tempData[1]  = battery_empty_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BLUETOOTH_CONNECTED_ICON:
					tempData[0]  = bluetooth_icon[font_index];
					tempData[1]  = bluetooth_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				case GPS_LINKED_ICON:
					tempData[0]  = gps_icon[font_index];
					tempData[1]  = gps_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				default:	break;

				}
				font_index++;       

			}
		}
		else
		{

			buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
			if (y_temp_adjustments == 0)
			{
				switch(data)
				{
				case BATTERY_HALF_ICON:                     
					frame_buff[buff_index++] ^= battery_icon[font_index];
					frame_buff[buff_index++] ^= battery_icon[++font_index];
					break;
				case BATTERY_FULL_ICON:                     
					frame_buff[buff_index++] ^= battery_full_icon[font_index];
					frame_buff[buff_index++] ^= battery_full_icon[++font_index];
					break;
				case BATTERY_EMPTY_ICON:                     
					frame_buff[buff_index++] ^= battery_empty_icon[font_index];
					frame_buff[buff_index++] ^= battery_empty_icon[++font_index];
					break;
				case BLUETOOTH_CONNECTED_ICON:
					frame_buff[buff_index++] ^= bluetooth_icon[font_index];
					frame_buff[buff_index++] ^= bluetooth_icon[++font_index];
					break;
				case GPS_LINKED_ICON:
					frame_buff[buff_index++] ^= gps_icon[font_index];
					frame_buff[buff_index++] ^= gps_icon[++font_index];
					break;
				default:	break;

				}


				++font_index;
			}
			else
			{


				switch(data)
				{
				case BATTERY_HALF_ICON:                     
					tempData[0]  = battery_icon[font_index];
					tempData[1]  = battery_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BATTERY_FULL_ICON:                     
					tempData[0]  = battery_full_icon[font_index];
					tempData[1]  = battery_full_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BATTERY_EMPTY_ICON:           
					tempData[0]  = battery_empty_icon[font_index];
					tempData[1]  = battery_empty_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				case BLUETOOTH_CONNECTED_ICON:
					tempData[0]  = bluetooth_icon[font_index];
					tempData[1]  = bluetooth_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				case GPS_LINKED_ICON:
					tempData[0]  = gps_icon[font_index];
					tempData[1]  = gps_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				default:	break;

				}
				font_index++;       

			}

		}
	}
}    

void Draw_favourites_icons(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour)			//16x16
{
	uint_16 x_temp = 0, ytemp = 0, buff_index = 0, font = 0,pix_height=0;
	uint_16 pix=0,pix_width=0, font_index = 0, y_temp_adjustments = 0, y_temp=0;   
	uint_8 tempData[5], count=0;


	X = X_MAX + 1 - X;

	y_temp = (Y - 1) / 8;	//Current Byte position
	y_temp_adjustments = ((pix_height + (Y - 1))) % 8;

	if(data==FAV_ICON)
	{
		pix_height=NUM_Y_PIXEL_FAV_ICON;
		pix_width= NUM_X_PIXEL_FAV_ICON * 2;
	}
	else if(data==ANGLE_ICON)
	{
		pix_height=NUM_Y_PIXEL_ANGLE_ICON;
		pix_width= NUM_X_PIXEL_ANGLE_ICON * 2;
	} 
	else if(data==ASPECT_ICON)
	{
		pix_height=NUM_Y_PIXEL_ASPECT_ICON;
		pix_width= NUM_X_PIXEL_ASPECT_ICON * 2;
	} 
	else if(data==LOCATION_ICON)
	{
		pix_height=NUM_Y_PIXEL_LOCATION_ICON;
		pix_width= NUM_X_PIXEL_LOCATION_ICON * 3;
	}
	else if(data==VIEW_ICON)
	{
		pix_height=NUM_Y_PIXEL_VIEW_ICON;
		pix_width= NUM_X_PIXEL_VIEW_ICON * 2;
	} 
	else if(data==DELETE_ICON)
	{
		pix_height=NUM_Y_PIXEL_DELETE_ICON;
		pix_width= NUM_X_PIXEL_DELETE_ICON * 2;
	} 
	else if(data==GRAY)
	{
		pix_height=NUM_Y_GRAY;
		pix_width= NUM_X_GRAY * 3;
	} 
	else if(data==CIRCLE_GREY)
	{
		pix_height=NUM_Y_CIRCLE_GREY;
		pix_width= NUM_X_CIRCLE_GREY * 4;
	} 
	else if(data==CIRCLE_BLACK)
	{
		pix_height=NUM_Y_CIRCLE_BLACK;
		pix_width= NUM_X_CIRCLE_BLACK * 4;
	} 
	else if((data==TOP_RECT_WHITE)||(data==TOP_RECT_BLACK)||(data==BOTTOM_RECT_WHITE)||(data==BOTTOM_RECT_BLACK))
	{
		pix_height=NUM_Y_TOP_RECT;
		pix_width= NUM_X_TOP_RECT * 4;
	} 


	x_temp = X-1 - (pix_width/2);

	for (font_index = 0; font_index < pix_width; )
	{
		if (pixel_colour == COLOUR_BLACK)
		{
			buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
			if (y_temp_adjustments == 0)
			{
				switch(data)
				{
				case FAV_ICON:                     
					frame_buff[buff_index++] |= fav_icon[font_index];
					frame_buff[buff_index++] |= fav_icon[++font_index];
					break;
				case LOCATION_ICON:
					frame_buff[buff_index++] |= location_icon[font_index];
					frame_buff[buff_index++] |= location_icon[++font_index];
					frame_buff[buff_index++] |= location_icon[++font_index];
					break;
				case ANGLE_ICON:
					frame_buff[buff_index++] |= angle_icon[font_index];
					frame_buff[buff_index++] |= angle_icon[++font_index];
					break;
				case ASPECT_ICON:
					frame_buff[buff_index++] |= aspect_icon[font_index];
					frame_buff[buff_index++] |= aspect_icon[++font_index];
					break;
				case VIEW_ICON:
					frame_buff[buff_index++] |= view_icon[font_index];
					frame_buff[buff_index++] |= view_icon[++font_index];
					break;
				case DELETE_ICON:
					frame_buff[buff_index++] |= delete_icon[font_index];
					frame_buff[buff_index++] |= delete_icon[++font_index];
					break;
				case GRAY:
					frame_buff[buff_index++] |= gray[font_index];
					frame_buff[buff_index++] |= gray[++font_index];
					frame_buff[buff_index++] |= gray[++font_index];
					break;
				case CIRCLE_GREY:
					frame_buff[buff_index++] |= circle_grey[font_index];
					frame_buff[buff_index++] |= circle_grey[++font_index];
					frame_buff[buff_index++] |= circle_grey[++font_index];
					frame_buff[buff_index++] |= circle_grey[++font_index];
					break;
				case CIRCLE_BLACK:
					frame_buff[buff_index++] |= circle_black[font_index];
					frame_buff[buff_index++] |= circle_black[++font_index];
					frame_buff[buff_index++] |= circle_black[++font_index];
					frame_buff[buff_index++] |= circle_black[++font_index];
					break;
				case TOP_RECT_WHITE:
					frame_buff[buff_index++] |= toprectwhite[font_index];
					frame_buff[buff_index++] |= toprectwhite[++font_index];
					frame_buff[buff_index++] |= toprectwhite[++font_index];
					frame_buff[buff_index++] |= toprectwhite[++font_index];
					break;
				case TOP_RECT_BLACK:
					frame_buff[buff_index++] |= toprectblack[font_index];
					frame_buff[buff_index++] |= toprectblack[++font_index];
					frame_buff[buff_index++] |= toprectblack[++font_index];
					frame_buff[buff_index++] |= toprectblack[++font_index];

					break;
				case BOTTOM_RECT_WHITE:
					frame_buff[buff_index++] |= bottomrectwhite[font_index];
					frame_buff[buff_index++] |= bottomrectwhite[++font_index];
					frame_buff[buff_index++] |= bottomrectwhite[++font_index];
					frame_buff[buff_index++] |= bottomrectwhite[++font_index];
					break;
				case BOTTOM_RECT_BLACK:
					frame_buff[buff_index++] |= bottomrectblack[font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];
					break;

				default:	break;

				}
				++font_index;
			}
			else
			{

				switch(data)
				{
				case FAV_ICON:                     
					tempData[0]  = fav_icon[font_index];
					tempData[1]  = fav_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;

				case LOCATION_ICON:
					tempData[0]  = location_icon[font_index];
					tempData[1]  = location_icon[++font_index];
					tempData[2]  = location_icon[++font_index]; 

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);

					break;

				case ANGLE_ICON:
					tempData[0]  = angle_icon[font_index];
					tempData[1]  = angle_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;

				case ASPECT_ICON:
					tempData[0]  = aspect_icon[font_index];
					tempData[1]  = aspect_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;

				case VIEW_ICON:
					tempData[0]  = view_icon[font_index];
					tempData[1]  = view_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;
				case DELETE_ICON:
					tempData[0]  = delete_icon[font_index];
					tempData[1]  = delete_icon[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					break;

				case GRAY:
					tempData[0]  = gray[font_index];
					tempData[1]  = gray[++font_index];
					tempData[2]  = gray[++font_index]; 

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);

					break;

				case CIRCLE_GREY:
					tempData[0]  = circle_grey[font_index];
					tempData[1]  = circle_grey[++font_index];
					tempData[2]  = circle_grey[++font_index]; 
					tempData[3]  = circle_grey[++font_index]; 


					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);

					break;

				case CIRCLE_BLACK:
					tempData[0]  = circle_black[font_index];
					tempData[1]  = circle_black[++font_index];
					tempData[2]  = circle_black[++font_index]; 
					tempData[3]  = circle_black[++font_index]; 

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);

					break;

				case TOP_RECT_WHITE:

					tempData[0]  = toprectwhite[font_index];
					tempData[1]  = toprectwhite[++font_index];
					tempData[2]  = toprectwhite[++font_index]; 
					tempData[3]  = toprectwhite[++font_index]; 


					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);
					break;

				case TOP_RECT_BLACK:

					tempData[0]  = toprectblack[font_index];
					tempData[1]  = toprectblack[++font_index];
					tempData[2]  = toprectblack[++font_index]; 
					tempData[3]  = toprectblack[++font_index]; 

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);
					break;

				case BOTTOM_RECT_WHITE:
					tempData[0]  = bottomrectwhite[font_index];
					tempData[1]  = bottomrectwhite[++font_index];
					tempData[2]  = bottomrectwhite[++font_index]; 
					tempData[3]  = bottomrectwhite[++font_index]; 

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);
					break;

				case BOTTOM_RECT_BLACK:
					frame_buff[buff_index++] |= bottomrectblack[font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];
					frame_buff[buff_index++] |= bottomrectblack[++font_index];

					frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);
					break;

				default:	break;

				}

				font_index++;       

			}
		}
		else
		{

			buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
			if (y_temp_adjustments == 0)
			{
				switch(data)
				{
				case FAV_ICON:                     
					frame_buff[buff_index++] ^= fav_icon[font_index];
					frame_buff[buff_index++] ^= fav_icon[++font_index];
					break;
				case LOCATION_ICON:
					frame_buff[buff_index++] ^= location_icon[font_index];
					frame_buff[buff_index++] ^= location_icon[++font_index];
					break;
				case ANGLE_ICON:
					frame_buff[buff_index++] ^= angle_icon[font_index];
					frame_buff[buff_index++] ^= angle_icon[++font_index];
					break;
				case ASPECT_ICON:
					frame_buff[buff_index++] ^= aspect_icon[font_index];
					frame_buff[buff_index++] ^= aspect_icon[++font_index];
					break;
				case VIEW_ICON:
					frame_buff[buff_index++] ^= view_icon[font_index];
					frame_buff[buff_index++] ^= view_icon[++font_index];
					break;
				case DELETE_ICON:
					frame_buff[buff_index++] ^= delete_icon[font_index];
					frame_buff[buff_index++] ^= delete_icon[++font_index];
					break;
				case GRAY:
					frame_buff[buff_index++] ^= gray[font_index];
					frame_buff[buff_index++] ^= gray[++font_index];
					frame_buff[buff_index++] ^= gray[++font_index];
					break;

				case CIRCLE_GREY:
					frame_buff[buff_index++] ^= circle_grey[font_index];
					frame_buff[buff_index++] ^= circle_grey[++font_index];
					frame_buff[buff_index++] ^= circle_grey[++font_index];
					frame_buff[buff_index++] ^= circle_grey[++font_index];
					break;
				case CIRCLE_BLACK:
					frame_buff[buff_index++] ^= circle_black[font_index];
					frame_buff[buff_index++] ^= circle_black[++font_index];
					frame_buff[buff_index++] ^= circle_black[++font_index];
					frame_buff[buff_index++] ^= circle_black[++font_index];
					break;

				case TOP_RECT_WHITE:
					frame_buff[buff_index++] ^= toprectwhite[font_index];
					frame_buff[buff_index++] ^= toprectwhite[++font_index];
					frame_buff[buff_index++] ^= toprectwhite[++font_index];
					frame_buff[buff_index++] ^= toprectwhite[++font_index];

					break;
				case TOP_RECT_BLACK:
					frame_buff[buff_index++] ^= toprectblack[font_index];
					frame_buff[buff_index++] ^= toprectblack[++font_index];
					frame_buff[buff_index++] ^= toprectblack[++font_index];
					frame_buff[buff_index++] ^= toprectblack[++font_index];

					break;

				case BOTTOM_RECT_WHITE:
					frame_buff[buff_index++] ^= bottomrectwhite[font_index];
					frame_buff[buff_index++] ^= bottomrectwhite[++font_index];
					frame_buff[buff_index++] ^= bottomrectwhite[++font_index];
					frame_buff[buff_index++] ^= bottomrectwhite[++font_index];
					break;

				case BOTTOM_RECT_BLACK:
					frame_buff[buff_index++] ^= bottomrectblack[font_index];
					frame_buff[buff_index++] ^= bottomrectblack[++font_index];
					frame_buff[buff_index++] ^= bottomrectblack[++font_index];
					frame_buff[buff_index++] ^= bottomrectblack[++font_index];
					break;

				default:	break;

				}

				++font_index;
			}
			else
			{

				switch(data)
				{
				case FAV_ICON:                     
					tempData[0]  = fav_icon[font_index];
					tempData[1]  = fav_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;

				case LOCATION_ICON:
					tempData[0]  = location_icon[font_index];
					tempData[1]  = location_icon[++font_index];
					tempData[2]  = location_icon[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);

					break;

				case ANGLE_ICON:
					tempData[0]  = angle_icon[font_index];
					tempData[1]  = angle_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;

				case ASPECT_ICON:
					tempData[0]  = aspect_icon[font_index];
					tempData[1]  = aspect_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;

				case VIEW_ICON:
					tempData[0]  = view_icon[font_index];
					tempData[1]  = view_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;
				case DELETE_ICON:
					tempData[0]  = delete_icon[font_index];
					tempData[1]  = delete_icon[++font_index];

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					break;

				case GRAY:
					tempData[0]  = gray[font_index];
					tempData[1]  = gray[++font_index];
					tempData[2]  = gray[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);

					break;

				case CIRCLE_GREY:
					tempData[0]  = circle_grey[font_index];
					tempData[1]  = circle_grey[++font_index];
					tempData[2]  = circle_grey[++font_index]; 
					tempData[3]  = circle_grey[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);

					break;

				case CIRCLE_BLACK:
					tempData[0]  = circle_black[font_index];
					tempData[1]  = circle_black[++font_index];
					tempData[2]  = circle_black[++font_index]; 
					tempData[3]  = circle_black[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);

					break;

				case TOP_RECT_WHITE:

					tempData[0]  = toprectwhite[font_index];
					tempData[1]  = toprectwhite[++font_index];
					tempData[2]  = toprectwhite[++font_index]; 
					tempData[3]  = toprectwhite[++font_index]; 


					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);
					break;

				case TOP_RECT_BLACK:

					tempData[0]  = toprectblack[font_index];
					tempData[1]  = toprectblack[++font_index];
					tempData[2]  = toprectblack[++font_index]; 
					tempData[3]  = toprectblack[++font_index]; 


					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);
					break;

				case BOTTOM_RECT_WHITE:
					tempData[0]  = bottomrectwhite[font_index];
					tempData[1]  = bottomrectwhite[++font_index];
					tempData[2]  = bottomrectwhite[++font_index]; 
					tempData[3]  = bottomrectwhite[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);
					break;

				case BOTTOM_RECT_BLACK:
					tempData[0]  = bottomrectblack[font_index];
					tempData[1]  = bottomrectblack[++font_index];
					tempData[2]  = bottomrectblack[++font_index]; 
					tempData[3]  = bottomrectblack[++font_index]; 

					frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
					frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);    
					frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
					frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
					frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
					frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
					frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);
					break;

				default:	break;

				}

				font_index++;       

			}

		}
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_string
 *  Brief:        prints a string on LCD
 *  Parameter:    Starting of string(X,Y coordinate),beginning address of data
 *                and pixel colour.
 *  Return:       None
 -----------------------------------------------------------------------------*/

//void Draw_string(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour);

void Draw_string_new(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour, uint_8 font_size)
{
	uint_16 x_temp = 0, y_temp = 0, buff_index = 0, font_index = 0;
	uint_8  tempData[5];
	uint_16 pix_height=0,pix_width=0,font_index_count=0, index = 0, y_temp_adjustments = 0;

	if((X<1) || (X>240) || (Y<1) || (Y>400))
	{
		
		printf("\nDRAW STRING NEW EXCEEDED THE LIMIT; BUFFER OVERFLOWWWWWWW 1");
		printf("\n X= %d\tY= %d\n",X,Y);
		//    	while(1)
		//    	{
		//    		
		//    	}
		return;
	}

	if(font_size==LARGE_FONT)		//LARGE FONT
	{
		pix_height=PIXEL_HIGHT_LARGE;
	}
	else if(font_size==MEDIUM_FONT)
	{
		pix_height=PIXEL_HIGHT_MEDIUM;
	}
	else if(font_size==SMALL_FONT)
	{
		Draw_string(X,Y, data,pixel_colour);
		return;

	}

	if (Flip_Status == 1)
	{


		X = ((X_MAX - X) + 1);

		//  y_temp=((pix_height +( Y-1)))/8;
		y_temp = (Y - 1) / 8;	//Current Byte position
		y_temp_adjustments = ((pix_height + (Y - 1))) % 8;

		x_temp = X-1;
		if(x_temp > 240)
		{
			printf("\nDRAW STRING NEW EXCEEDED THE LIMIT; BUFFER OVERFLOWWWWWWW 2");
			printf("\n X= %d\tY= %d\n",X,Y);
			//        	while(1)
			//        	{
			//        		
			//        	}
			return;
		}

		while (*data)
		{
			if (*data >= 'A' && *data <= 'Z')
				font_index = (*data - 'A');
			else if (*data >= 'a' && *data <= 'z')		//NOT AVAILABLE IN BIG FONTS
				font_index = (*data - 'a') + 26;
			else if (*data >= '1' && *data <= '9')
				font_index = (*data - '1') + 26;
			else
				switch (*data)							//UPDATED VALUES FOR NEW DATA
				{

				case '0':  font_index = 35;    break;
				case '!':  font_index = 36;    break;
				case '?':  font_index = 37;    break;
				case '@':  font_index = 38;    break;
				case '#':  font_index = 39;    break;
				case '$':  font_index = 40;    break;
				case '%':  font_index = 41;    break;
				case '&':  font_index = 42;    break;
				case '*':  font_index = 43;    break;
				case '(':  font_index = 44;    break;
				case ')':  font_index = 45;    break;
				case '-':  font_index = 46;    break;
				case '+':  font_index = 47;    break;
				case '=':  font_index = 48;    break;
				case '/':  font_index = 49;    break;
				case '<':  font_index = 50;    break;
				case '>':  font_index = 51;    break;
				case '.':  font_index = 52;    break;
				case ',':  font_index = 53;    break;
				case ':':  font_index = 54;    break;
				case ';':  font_index = 55;    break;
				case '[':  font_index = 56;    break;
				case ']':  font_index = 57;    break;
				case '\'':  font_index = 58;    break;
				case '{':  font_index = 59;    break;
				case '}':  font_index = 60;    break;
				case '|':  font_index = 61;    break;
				case '°':  font_index = 62;    break;       
				case ' ':  font_index = 63;    break;     

				default:  font_index = 63;     break;
				}


			if(font_size==LARGE_FONT)		//LARGE FONT
			{
				pix_width=large_fonts_size[font_index];
				font_index_count=font_index-1;
				font_index=0;

				while(font_index_count>=0 && font_index_count<=100)		//Unsigned integer so 2 check conditions for signed condition..else infinite loop
				{
					font_index += large_fonts_size[font_index_count];	
					--font_index_count;		
				}
			}
			else if(font_size==MEDIUM_FONT)
			{
				pix_width=medium_fonts_size[font_index];
				font_index_count=font_index-1;
				font_index=0;

				while(font_index_count>=0 && font_index_count<=100)		//Unsigned integer so 2 check conditions for signed condition..else infinite loop
				{
					font_index += medium_fonts_size[font_index_count];	
					--font_index_count;		
				}
			}




			//           printf("Size of %d is %d",font_index,pix_width);

			for (index = 0; index < pix_width; index++)
			{
				if (pixel_colour == COLOUR_BLACK)
				{
					buff_index = ((x_temp-- - 1) * LINE_BYTES) + y_temp;                
					if(buff_index >= 12000)
					{
						printf("\n DRAW STRING NEW Buffer index size exceeded!!!!");
						//                    	while(1)
						//                    	{
						//                    		
						//                    	}
						buff_index = 11999;
						return;
					}
					if (y_temp_adjustments == 0)
					{
						if(font_size==LARGE_FONT)		//LARGE FONT
						{
							frame_buff[buff_index++] |= ((large_fonts[font_index] & 0x0FF00000000) >> 32);
							frame_buff[buff_index++] |= ((large_fonts[font_index] & 0x0FF000000) >> 24);
							frame_buff[buff_index++] |= ((large_fonts[font_index] & 0x0FF0000) >> 16);
							frame_buff[buff_index++] |= ((large_fonts[font_index] & 0x0FF00) >> 8);
							frame_buff[buff_index]   |= (large_fonts[font_index] & 0x0FF);


						}
						else if(font_size==MEDIUM_FONT)
						{

							frame_buff[buff_index++] |= ((medium_fonts[font_index] & 0x0FF0000) >> 16);
							frame_buff[buff_index++] |= ((medium_fonts[font_index] & 0x0FF00) >> 8);
							frame_buff[buff_index]   |= (medium_fonts[font_index] & 0x0FF);


						}

						font_index++;
					}
					else
					{

						if(font_size==LARGE_FONT)		//LARGE FONT
						{

							tempData[0]  = ((large_fonts[font_index] & 0x0FF00000000) >> 32);
							tempData[1]  = ((large_fonts[font_index] & 0x0FF000000) >> 24);
							tempData[2]  = ((large_fonts[font_index] & 0x0FF0000) >> 16);
							tempData[3]  = ((large_fonts[font_index] & 0x0FF00) >> 8);
							tempData[4]  = (large_fonts[font_index] & 0x0FF);

							frame_buff[buff_index] |= tempData[0] >> y_temp_adjustments;
							frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
							frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
							frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
							frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 3] |= tempData[3] >> y_temp_adjustments;
							frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 4] |= tempData[4] >> y_temp_adjustments;
							frame_buff[buff_index + 4] |= tempData[3] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 5] |= tempData[4] << (8 - y_temp_adjustments);


						}
						else if(font_size==MEDIUM_FONT)
						{
							tempData[0]  = ((medium_fonts[font_index] & 0x0FF0000) >> 16);
							tempData[1]  = ((medium_fonts[font_index] & 0x0FF00) >> 8);
							tempData[2]  = (medium_fonts[font_index] & 0x0FF);

							frame_buff[buff_index]     |= tempData[0] >> y_temp_adjustments;
							frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
							frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);                        
							frame_buff[buff_index + 2] |= tempData[2] >> y_temp_adjustments;
							frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 3] |= tempData[2] << (8 - y_temp_adjustments);



						}

						font_index++;       

					}
				}
				else
				{

					buff_index = ((x_temp-- - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{
						if(font_size==LARGE_FONT)		//LARGE FONT
								{
							frame_buff[buff_index++] ^= ((large_fonts[font_index] & 0x0FF00000000) >> 32);
							frame_buff[buff_index++] ^= ((large_fonts[font_index] & 0x0FF000000) >> 24);
							frame_buff[buff_index++] ^= ((large_fonts[font_index] & 0x0FF0000) >> 16);
							frame_buff[buff_index++] ^= ((large_fonts[font_index] & 0x0FF00) >> 8);
							frame_buff[buff_index]   ^= (large_fonts[font_index] & 0x0FF);


								}
						else if(font_size==MEDIUM_FONT)
						{

							frame_buff[buff_index++] ^= ((medium_fonts[font_index] & 0x0FF0000) >> 16);
							frame_buff[buff_index++] ^= ((medium_fonts[font_index] & 0x0FF00) >> 8);
							frame_buff[buff_index]   ^= (medium_fonts[font_index] & 0x0FF);


						}

						font_index++;

					}
					else
					{
						if(font_size==LARGE_FONT)		//LARGE FONT
						{
							tempData[0]  = ((large_fonts[font_index] & 0x0FF00000000) >> 32);
							tempData[1]  = ((large_fonts[font_index] & 0x0FF000000) >> 24);
							tempData[2]  = ((large_fonts[font_index] & 0x0FF0000) >> 16);
							tempData[3]  = ((large_fonts[font_index] & 0x0FF00) >> 8);
							tempData[4]  = (large_fonts[font_index] & 0x0FF);

							frame_buff[buff_index] ^= tempData[0] >> y_temp_adjustments;
							frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
							frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                     
							frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
							frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 3] ^= tempData[3] >> y_temp_adjustments;
							frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 4] ^= tempData[4] >> y_temp_adjustments;
							frame_buff[buff_index + 4] ^= tempData[3] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 5] ^= tempData[4] << (8 - y_temp_adjustments);
						}
						else if(font_size==MEDIUM_FONT)
						{
							tempData[0]  = ((medium_fonts[font_index] & 0x0FF0000) >> 16);
							tempData[1]  = ((medium_fonts[font_index] & 0x0FF00) >> 8);
							tempData[2]  = (medium_fonts[font_index] & 0x0FF);

							frame_buff[buff_index]     ^= tempData[0] >> y_temp_adjustments;
							frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
							frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);                        
							frame_buff[buff_index + 2] ^= tempData[2] >> y_temp_adjustments;
							frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
							frame_buff[buff_index + 3] ^= tempData[2] << (8 - y_temp_adjustments);

						}

						font_index++;

					}

				}
			}
			x_temp-=2;
			data++;
		}

	}
	else
	{
		y_temp = (Y_MAX - (pix_height + (Y - 1))) / 8;
		y_temp_adjustments = (Y_MAX - (pix_height + (Y - 1))) % 8;

		x_temp = X;
		while (*data)
		{
			if (*data >= 'A' && *data <= 'Z')
				font_index = (*data - 'A');
			else if (*data >= 'a' && *data <= 'z')
				font_index = (*data - 'a') + 26;
			else if (*data >= '0' && *data <= '9')
				font_index = (*data - '0') + 52;
			else
				switch (*data)
				{
				case '.':
				font_index = 62;
				break;
				case ' ':
					font_index = 63;
					break;
				case '?':
					font_index = 64;
					break;
				case '#':
					font_index = 65;
					break;
				case '°':
					font_index = 66;
					break;
				case '%':
					font_index = 67;
					break;
				case ',':
					font_index = 68;
					break;
				case ':':
					font_index = 69;
					break;
				case ';':
					font_index = 70;
					break;
				case '(':
					font_index = 71;
					break;
				case ')':
					font_index = 72;
					break;
				case '-':
					font_index = 73;
					break;
				default:
					break;
				}
			font_index *= pix_width;
			for (index = 0; index < pix_width; index++)
			{
				if (pixel_colour == COLOUR_BLACK)
				{
					buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] |= reverse(large_fonts[font_index] & 0x0FF);
						frame_buff[buff_index] |= reverse((large_fonts[font_index] & 0xFF00) >> 8);
						font_index++;
					}
					else
					{
						tempData[0] = reverse(large_fonts[font_index] & 0x0FF);
						tempData[1] = reverse((large_fonts[font_index] & 0xFF00) >> 8);

						frame_buff[buff_index] |= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}
				}
				else
				{

					buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] ^= (reverse(large_fonts[font_index] & 0x0FF));
						frame_buff[buff_index] ^= (reverse((large_fonts[font_index] & 0xFF00) >> 8));
						font_index++;
					}
					else
					{
						tempData[0] = (reverse(large_fonts[font_index] & 0x0FF));
						tempData[1] = (reverse((large_fonts[font_index] & 0xFF00) >> 8));

						frame_buff[buff_index] ^= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}

				}
			}
			x_temp++;
			data++;
		}
	}
}



/*-----------------------------------------------------------------------------
 *  Function:     Draw_string
 *  Brief:        prints a string on LCD
 *  Parameter:    Starting of string(X,Y coordinate),beginning address of data
 *                and pixel colour.
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Draw_string(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour)
{
	Draw_string_new(X,Y,data,pixel_colour,MEDIUM_FONT);
#if 0
	uint_16 x_temp = 0, y_temp = 0, buff_index = 0, font_index = 0;
	uint_8 index = 0, y_temp_adjustments = 0, tempData[3];

	if (Flip_Status == 1)
	{

		X = X_MAX + 1 - X;

		//  y_temp=((PIXEL_HIGHT +( Y-1)))/8;
		y_temp = (Y - 1) / 8;
		y_temp_adjustments = ((PIXEL_HIGHT + (Y - 1))) % 8;

		x_temp = X;
		while (*data)
		{
			if (*data >= 'A' && *data <= 'Z')
				font_index = (*data - 'A');
			else if (*data >= 'a' && *data <= 'z')
				font_index = (*data - 'a') + 26;
			else if (*data >= '0' && *data <= '9')
				font_index = (*data - '0') + 52;
			else
				switch (*data)
				{
				case '.':
				font_index = 62;
				break;
				case ' ':
					font_index = 63;
					break;
				case '?':
					font_index = 64;
					break;
				case '#':
					font_index = 65;
					break;
				case '°':
					font_index = 66;
					break;
				case '%':
					font_index = 67;
					break;
				case ',':
					font_index = 68;
					break;
				case ':':
					font_index = 69;
					break;
				case ';':
					font_index = 70;
					break;
				case '(':
					font_index = 71;
					break;
				case ')':
					font_index = 72;
					break;
				case '-':
					font_index = 73;
					break;
				default:
					break;
				}
			font_index *= 10;
			for (index = 0; index < PIXEL_WIDTH; index++)
			{
				if (pixel_colour == COLOUR_BLACK)
				{
					buff_index = ((x_temp-- - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] |= ((fonts[font_index] & 0xFF00) >> 8);
						frame_buff[buff_index] |= (fonts[font_index] & 0x0FF);
						font_index++;
					}
					else
					{
						tempData[0] = ((fonts[font_index] & 0xFF00) >> 8);
						tempData[1] = (fonts[font_index] & 0x0FF);

						frame_buff[buff_index] |= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}
				}
				else
				{

					buff_index = ((x_temp-- - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] ^= (((fonts[font_index] & 0xFF00) >> 8));
						frame_buff[buff_index] ^= ((fonts[font_index] & 0x0FF));
						font_index++;
					}
					else
					{
						tempData[0] = (((fonts[font_index] & 0xFF00) >> 8));
						tempData[1] = ((fonts[font_index] & 0x0FF));

						frame_buff[buff_index] ^= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}

				}
			}
			x_temp--;
			data++;
		}

	}
	else
	{
		y_temp = (Y_MAX - (PIXEL_HIGHT + (Y - 1))) / 8;
		y_temp_adjustments = (Y_MAX - (PIXEL_HIGHT + (Y - 1))) % 8;

		x_temp = X;
		while (*data)
		{
			if (*data >= 'A' && *data <= 'Z')
				font_index = (*data - 'A');
			else if (*data >= 'a' && *data <= 'z')
				font_index = (*data - 'a') + 26;
			else if (*data >= '0' && *data <= '9')
				font_index = (*data - '0') + 52;
			else
				switch (*data)
				{
				case '.':
				font_index = 62;
				break;
				case ' ':
					font_index = 63;
					break;
				case '?':
					font_index = 64;
					break;
				case '#':
					font_index = 65;
					break;
				case '°':
					font_index = 66;
					break;
				case '%':
					font_index = 67;
					break;
				case ',':
					font_index = 68;
					break;
				case ':':
					font_index = 69;
					break;
				case ';':
					font_index = 70;
					break;
				case '(':
					font_index = 71;
					break;
				case ')':
					font_index = 72;
					break;
				case '-':
					font_index = 73;
					break;
				default:
					break;
				}
			font_index *= 10;
			for (index = 0; index < PIXEL_WIDTH; index++)
			{
				if (pixel_colour == COLOUR_BLACK)
				{
					buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] |= reverse(fonts[font_index] & 0x0FF);
						frame_buff[buff_index] |= reverse((fonts[font_index] & 0xFF00) >> 8);
						font_index++;
					}
					else
					{
						tempData[0] = reverse(fonts[font_index] & 0x0FF);
						tempData[1] = reverse((fonts[font_index] & 0xFF00) >> 8);

						frame_buff[buff_index] |= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] |= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] |= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}
				}
				else
				{

					buff_index = ((x_temp++ - 1) * LINE_BYTES) + y_temp;
					if (y_temp_adjustments == 0)
					{

						frame_buff[buff_index++] ^= (reverse(fonts[font_index] & 0x0FF));
						frame_buff[buff_index] ^= (reverse((fonts[font_index] & 0xFF00) >> 8));
						font_index++;
					}
					else
					{
						tempData[0] = (reverse(fonts[font_index] & 0x0FF));
						tempData[1] = (reverse((fonts[font_index] & 0xFF00) >> 8));

						frame_buff[buff_index] ^= tempData[0] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[1] >> y_temp_adjustments;
						frame_buff[buff_index + 1] ^= tempData[0] << (8 - y_temp_adjustments);
						frame_buff[buff_index + 2] ^= tempData[1] << (8 - y_temp_adjustments);
						font_index++;

					}

				}
			}
			x_temp++;
			data++;
		}
	}
#endif
}

uint_8 Get_string_pixel_length(uint_8 *data, uint_8 font_size)
{
	uint_16 font_index = 0, pix_width=0, count = 0;

	while (*data)
	{
		if (*data >= 'A' && *data <= 'Z')
			font_index = (*data - 'A');
		else if (*data >= 'a' && *data <= 'z')		//NOT AVAILABLE IN BIG FONTS
			font_index = (*data - 'a') + 26;
		else if (*data >= '1' && *data <= '9')
			font_index = (*data - '1') + 26;
		else
			switch (*data)							//UPDATED VALUES FOR NEW DATA
			{

			case '0':  font_index = 35;    break;
			case '!':  font_index = 36;    break;
			case '?':  font_index = 37;    break;
			case '@':  font_index = 38;    break;
			case '#':  font_index = 39;    break;
			case '$':  font_index = 40;    break;
			case '%':  font_index = 41;    break;
			case '&':  font_index = 42;    break;
			case '*':  font_index = 43;    break;
			case '(':  font_index = 44;    break;
			case ')':  font_index = 45;    break;
			case '-':  font_index = 46;    break;
			case '+':  font_index = 47;    break;
			case '=':  font_index = 48;    break;
			case '/':  font_index = 49;    break;
			case '<':  font_index = 50;    break;
			case '>':  font_index = 51;    break;
			case '.':  font_index = 52;    break;
			case ',':  font_index = 53;    break;
			case ':':  font_index = 54;    break;
			case ';':  font_index = 55;    break;
			case '[':  font_index = 56;    break;
			case ']':  font_index = 57;    break;
			case '\'':  font_index = 58;    break;
			case '{':  font_index = 59;    break;
			case '}':  font_index = 60;    break;
			case '|':  font_index = 61;    break;
			case '°':  font_index = 62;    break;       
			case ' ':  font_index = 63;    break;     

			default:  font_index = 63;     break;
			}


		if(font_size==LARGE_FONT)		//LARGE FONT
		{
			pix_width += large_fonts_size[font_index];
			font_index=0;
		}
		else if(font_size==MEDIUM_FONT)
		{
			pix_width += medium_fonts_size[font_index];
		}
		//        printf("\nPix width added for character %c is : %d", *data, medium_fonts_size[font_index]);
		data++;	count++;
	}
	//    pix_width += count;
	//    printf("Total pix width calculated is : %d Count to be added is: %d Final pix width is: %d", pix_width-count, count, pix_width);
	return pix_width;
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
