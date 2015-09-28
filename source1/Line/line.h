/*
 * line.h
 *
 *  Created on: Dec 4, 2013
 *      Author: user
 */

#ifndef LINE_H_
#define LINE_H_

#include <mqx.h>
#include <bsp.h>
#include "spi_config.h"
#include "font.h"
#include "spi_lcd.h"

/*function prototype*/
extern void Draw_Rect(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel);
extern void Rect_Fill(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel);
extern void Draw_Line(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel);
extern void Draw_Dithered_Line(uint_16 X1, uint_16 Y1, uint_16 X2, uint_16 Y2, uint_8 pixel);
void Rect_fill_with_dithered_lines(uint_8 X1, uint_16 Y1, uint_8 X2, uint_16 Y2, uint_8 pixel);

#endif /* LINE_H_ */
