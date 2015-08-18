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


extern _mqx_int lcd_clear(void);  //pointer,MQX_FILE_PTR
extern _mqx_int draw_image(uchar_ptr); //pointer,MQX_FILE_PTR ,
//extern uint_8   reverse(uint_8 );
extern void     spi_init(void);
extern void     buff_clear();
extern void     font(uint_16,uint_8);
extern void Upadate_lcd_settings(uint_32);

extern void lcd_display_init(void);
extern void lcd_display_power_pin_init(void);
extern void lcd_display_power_pin_on(void);
extern void lcd_display_power_pin_off(void);
extern void lcd_display_deinit(void );
extern void lcd_display_on_pin_on(void);
extern void lcd_display_on_pin_off(void);
extern void Set_lcd_buad_rate(uint_32 buad_rate);


