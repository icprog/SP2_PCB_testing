/******************************************************************************
*
*   Filename: $bitmap.h
*   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
*   Revision: $1
*   Author: $GadgEon
*
*   Description: $This file contains definitions for static images.
*
*   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
*
*****************************************************************************/

#ifndef __BITMAP_H__
#define __BITMAP_H__

void  Draw_Image_on_Buffer(uint_8 * buff);
//void  Draw_Icon_on_Buffer(uint_8 * buff)  ;

extern const unsigned char Default_background [];
extern const unsigned char logo_image[];
extern const unsigned char test_button_background[];
extern const unsigned char chart_image[];
extern const unsigned char left_footer_background[];
extern const unsigned char both_footer_background[];
extern const unsigned char base3[];
extern const unsigned char Collecting_Image0 [];
extern const unsigned char Collecting_Image1 [];
extern const unsigned char Collecting_Image2 [];
extern const unsigned char Collecting_Image3 [];
extern const unsigned char Collecting_Image4 [];
extern const unsigned char Processing_Image0 [];
extern const unsigned char Processing_Image1 [];
extern const unsigned char Processing_Image2 [];
extern const unsigned char Processing_Image3 [];
extern const unsigned char Processing_Image4 [];
extern const unsigned char Mass_Storage_Conn [];
extern const unsigned char Battery_low_1 [];
extern const unsigned char Battery_low_2 [];
#define FRAME_BUFFER_BYTE_ARRAY_SIZE (12000)

#endif

/*-----------------------------------------------------------------------------
    **************************  END   ***************************************
-----------------------------------------------------------------------------*/
