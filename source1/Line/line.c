/******************************************************************************
 *
 *     Filename:       $line.c
 *     Created on:     $Date: Mar 25, 2014    (6:00:07 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contains functions to to draw graphics on lcd
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "line.h"
volatile uint_8 Flip_Status = 1;

extern void draw_pixel(uint_16, uint_16, uint_8);
extern void draw_line_vertical(uint_8, uint_16, uint_16, uint_8);
void draw_line_horizontal(uint_8 X1, uint_8 X2, uint_16 Y, uint_8 pixel);
void draw_dithered_line_horizontal(uint_8 X1, uint_8 X2, uint_16 Y, uint_8 pixel);
void Rect_fill_with_horizontal_lines(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel);
//void Rect_fill_with_dithered_lines(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel);

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Pixel
 *  Brief:        Draw a pixel on specified X Y coordinate of  Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void draw_pixel(uint_16 X, uint_16 Y, uint_8 pixel)
{
    uint_16 byte_index = 0;
    uint_8 buffer_data = 0, pixel_offset = 0;
    
    if ((X < X_MIN) || (X > X_MAX) || (Y < Y_MIN) || (Y > Y_MAX))
    {
        
        return;
    }
    
    /*Byte location in the global LCD buffer*/
    if (Flip_Status == 0)
    {
        byte_index = ((X - 1) * LINE_BYTES) + ((Y_MAX - (Y - 1)) >> 3);
        /*Pixel offset in the Byte */
        pixel_offset = ((Y - 1) & 0x07);
        if (Y && !pixel_offset)
            byte_index--;
    }
    else
    {
        byte_index = ((X_MAX - X) * LINE_BYTES) + ((Y - 1) >> 3);
        /*Pixel offset in the Byte */
        pixel_offset = (8 - ((Y - 1) & 0x07)) & 0x07;
        
        if (Y && !pixel_offset)
            byte_index--;
        
    }
    
    /*Extracting the byte from frame buffer */
    buffer_data = frame_buff[byte_index];
    
    /*Clearing and adding new pixel to byte */
    buffer_data &= ~(0x01 << pixel_offset);
    buffer_data |= (pixel << pixel_offset);
    
    /*Updating new byte to frame buffer*/
    frame_buff[byte_index] = buffer_data;
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Line_V
 *  Brief:        Draw vertical line on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void draw_line_vertical(uint_8 X, uint_16 Y1, uint_16 Y2, uint_8 pixel)
{
    uint_8 pixel_mask1, pixel_mask2, buffer_data;
    uint_16 Y1_address, Y2_address, byte_index;
    
    if (Flip_Status == 1)
    {
        uint_16 temp = Y1;
        if((Y2>Y_MAX)&&(Y1>Y_MAX)&&(X>X_MAX))
        {
        	printf("draw_line_vertical Error...\n");
        	while(1)
        	{
        		
        	}
        }
        Y1 = (Y_MAX+1) - Y2;
        Y2 = (Y_MAX+1) - temp;
        
        X = (X_MAX+1) - X;
        
    }
    pixel_mask1 = 0xFF >> (0x07 - ((Y2 - 1) & 0x07));
    pixel_mask2 = (0xFF << ((Y1 - 1) & 0x07));
    Y1_address = ((Y_MAX - (Y1)) >> 3); //Y1>>3;
    Y2_address = ((Y_MAX - (Y2)) >> 3); //Y2>>3; 
    byte_index = (X - 1) * LINE_BYTES + Y2_address;
    buffer_data = frame_buff[byte_index];
    
    if (Y1_address != Y2_address)
    {
        if (!pixel)
//            buffer_data ^= pixel_mask1;
        	buffer_data &=pixel_mask1;          
        else
            buffer_data |= pixel_mask1;
        
        frame_buff[byte_index++] = buffer_data;
        Y2_address++;
        
        while (Y1_address > Y2_address)
        {
            if (pixel)
                frame_buff[byte_index++] = 0xFF;
            else
                frame_buff[byte_index++] = 0x00;
            
            Y2_address++;
        }
        
        buffer_data = frame_buff[byte_index];
        
        if (!pixel)
            buffer_data &= pixel_mask2;
        else
            buffer_data |= pixel_mask2;
        
        frame_buff[byte_index] = buffer_data;
    }
    
    else
    {
        pixel_mask1 &= pixel_mask2;
        if (!pixel)
            buffer_data &= pixel_mask1;
        else
            buffer_data |= pixel_mask1;
        
        frame_buff[byte_index] = buffer_data;
        
    }
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Line_H
 *  Brief:        Draw Horizontal line on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/

void draw_line_horizontal(uint_8 X1, uint_8 X2, uint_16 Y, uint_8 pixel)
{
    
    uint_16 x_index;
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    
    /* Draw continues pixel to make a line*/
    for (x_index = X1; x_index <= X2; x_index++)
        draw_pixel(x_index, Y, pixel);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     draw_dithered_line_horizontal
 *  Brief:        Draw Dithered Horizontal line on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/

void draw_dithered_line_horizontal(uint_8 X1, uint_8 X2, uint_16 Y, uint_8 pixel)
{
    
    uint_16 x_index;
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    
    /* Draw continues pixel to make a line*/
    for (x_index = X1; x_index <= X2; x_index++)
    {
    	draw_pixel(x_index, Y, pixel);
    	pixel ^= 1;
    }
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Rect_fill_with_horizontal_lines
 *  Brief:        Fill a rectangle with dithered lines on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/

void Rect_fill_with_horizontal_lines(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel)
{
    if (Y1 > Y2)
    {
        Y1 = Y1 + Y2;
        Y2 = Y1 - Y2;
        Y1 = Y1 - Y2;
    }

		while(Y1<=Y2)
		{
			draw_line_horizontal(X1,X2,Y1,pixel);
			++Y1;
		}

}

void Rect_fill_with_dithered_lines(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel)
{
    if (Y1 > Y2)
    {
        Y1 = Y1 + Y2;
        Y2 = Y1 - Y2;
        Y1 = Y1 - Y2;
    }

		while(Y1<=Y2)
		{
			draw_dithered_line_horizontal(X1,X2,Y1,pixel);
			++Y1;
			pixel ^= 1;
		}

}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Rect
 *  Brief:        Draw a Rectangle on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Draw_Rect(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel)
{
    
    /*Swap the Y coordinates if Y1 is greater than Y2   */
    if (Y1 > Y2)
    {
        Y1 = Y1 + Y2;
        Y2 = Y1 - Y2;
        Y1 = Y1 - Y2;
    }
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    /* Draw Horizontal Top line*/
    draw_line_horizontal(X1, X2, Y1, pixel);
    
    /* Draw Horizontal Bottom line*/
    draw_line_horizontal(X1, X2, Y2, pixel);
    
    /* Draw Vertical  Left line*/
    draw_line_vertical(X1, Y1, Y2, pixel);
    
    /* Draw Vertical  Right line*/
    draw_line_vertical(X2, Y1, Y2, pixel);
}

/*-----------------------------------------------------------------------------
 *  Function:     Rect_Fill
 *  Brief:        Fill the rectangle with specified colour
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Rect_Fill(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel)
{
    /*Swap the Y coordinates if Y1 is greater than Y2   */
    if (Y1 > Y2)
    {
        Y1 = Y1 + Y2;
        Y2 = Y1 - Y2;
        Y1 = Y1 - Y2;
    }
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    /*Fill with Vertical Lines*/
    do
    {
        //  Draw_Line_H(X1,X2,Y1,pixel);
        draw_line_vertical(X1, Y1, Y2, pixel);
        //  Draw_Line(X1,Y1,X2,Y2,pixel);   
    } while (X1++ < X2);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Dithered_Line
 *  Brief:        Draw a Dithered Line on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Draw_Dithered_Line(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel)
{
    uint_16 x, y;
    int_16 dx, dy;
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    dx = X2 - X1;
    dy = Y2 - Y1;
    
    for (x = X1; x <= X2; x++)
    {
        y = Y1 + (dy) * (x - X1) / (dx);
        draw_pixel(x, y, pixel);
    	pixel ^= 1;
    }
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Draw_Line
 *  Brief:        Draw a Line on Frame buffer
 *  Parameter:    X,Y coordinate and pixel colour
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Draw_Line(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel)
{
    uint_16 x, y;
    int_16 dx, dy;
    
    /*Swap the X coordinates if X1 is greater than X2   */
    if (X1 > X2)
    {
        X1 = X1 + X2;
        X2 = X1 - X2;
        X1 = X1 - X2;
    }
    dx = X2 - X1;
    dy = Y2 - Y1;
    
    for (x = X1; x <= X2; x++)
    {
        y = Y1 + (dy) * (x - X1) / (dx);
        draw_pixel(x, y, pixel);
    }
    
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
