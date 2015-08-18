/*
 * font.h
 *
 *  Created on: Dec 2, 2013
 *      Author: GADGEON-03
 */

#ifndef FONT_H_
#define FONT_H_

//extern void Draw_string(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour);

extern void Draw_string(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour);
extern void Draw_string_new(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour, uint_8 font_size);
extern void Draw_string_medium(uint_16 X, uint_16 Y, uint_8 * data, uint_8 pixel_colour, uint_8 font_size);
extern void Draw_header_icons(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour);
extern void Draw_loading_image(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour);
extern void Draw_image(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour);
extern void Draw_favourites_icons(uint_16 X, uint_16 Y, uint_8 data, uint_8 pixel_colour);
extern uint_8 Get_string_pixel_length(uint_8 *data, uint_8 font_size);

extern const uint_16 fonts[];
extern const uint_32 medium_fonts[];
extern const uint_64 large_fonts[];
extern const uint_8 large_fonts_size[];
extern const uint_8 medium_fonts_size[];





#endif /* FONT_H_ */

