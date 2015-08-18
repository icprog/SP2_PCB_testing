/*
 * spi_config.h
 *
 *  Created on: Jan 27, 2014
 *      Author: GADGEON-03
 */

#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

/**************************************************************************************/
/*                              Defining LCD GPIO PINS                                */
/**************************************************************************************/
#define	BSP_LCD_VCOM				(GPIO_PORT_B | GPIO_PIN5)
#define	BSP_LCD_VCOM_MUX			(LWGPIO_MUX_E5_GPIO)

#define BSP_CS_LCD					(GPIO_PORT_B | GPIO_PIN20)
#define BSP_CS_LCD_MUX				(LWGPIO_MUX_B20_GPIO)

/* Defining SPI Channel*/
#define BSP_SPI_LCD_CHANNEL			2    /* (spi 2) */

/*SPI Clock Frequency*/
#define SPI_CLK_FREQ				4000000


/**************************************************************************************/
/*                              Defining DISPLAY CONFIGURATIONS                                */
/**************************************************************************************/
//#define X_FONT_LIMIT 		384
//#define Y_FONT_LIMIT 		230

#define PIXEL_WIDTH			10
#define PIXEL_HIGHT			16


#define PIXEL_HIGHT_LARGE			40

#define PIXEL_HIGHT_MEDIUM			32



#define X_MAX				240
#define Y_MAX				400

#define X_MIN				1
#define Y_MIN				1

#define LINE_BYTES			50
/**************************************************************************************/

extern const char_ptr device_mode[];
extern const char_ptr clock_mode[];
extern uint_8 frame_buff[];

/*-----------------------------------------------------------------------------
 *************************		END		*******************************
-----------------------------------------------------------------------------*/

#endif /* SPI_CONFIG_H_ */
