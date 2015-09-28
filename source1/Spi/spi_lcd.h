#ifndef __spi_lcd_h__
#define __spi_lcd_h__
#endif

/******************************************************************************
 *
 * 	Filename:       $spi_lcd.h
 * 	Created on:     $Date: Nov 26, 2013	(11:23:40 AM)
 * 	Revision:       $1
 * 	Author:         $GadgEon
 *
 * 	Description:    $This file describes all spi interface application.
 *
 * 	Copyright (C) 2013 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/



extern uint_8 frame_buff_temp[12500];
extern LWGPIO_STRUCT lcd_cs;
extern MQX_FILE_PTR  spifd;
extern LWGPIO_STRUCT REG_SHDN;

/*Added new*/
#define BSP_SHDN_LCD			(GPIO_PORT_C|GPIO_PIN2)
#define BSP_SHDN_LCD_MUX		(LWGPIO_MUX_C2_GPIO)


extern LWGPIO_STRUCT DISP_ON;
#define BSP_DISP_ON_PIN        (GPIO_PORT_B|GPIO_PIN6)
#define LWGPIO_MUX_B6_GPIO     (1)

#define BSP_BLEN_LCD				(GPIO_PORT_B|GPIO_PIN4)
#define BSP_BLEN_LCD_MUX			(LWGPIO_MUX_B4_GPIO)

extern _mqx_int lcd_clear(void);  //pointer,MQX_FILE_PTR
extern _mqx_int draw_image(uchar_ptr); //pointer,MQX_FILE_PTR ,
//extern uint_8   reverse(uint_8 );
void spi_init(void);
void buff_clear();
void font(uint_16,uint_8);
void Upadate_lcd_settings(uint_32);

void lcd_display_init(void);
void lcd_display_power_pin_init(void);
void lcd_display_power_pin_on(void);
void lcd_display_power_pin_off(void);
void lcd_display_deinit(void );
void lcd_display_on_pin_on(void);
void lcd_display_on_pin_off(void);
void Set_lcd_buad_rate(uint_32 buad_rate);
void Test_backlight(void);


