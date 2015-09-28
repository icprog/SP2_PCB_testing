/******************************************************************************
 *
 * 	Filename:       $spi_lcd.c
 * 	Created on:     $Date: Nov 26, 2013	(11:23:16 AM)
 * 	Revision:       $1
 * 	Author:         $GadgEon
 *
 * 	Description:    $This file describes all spi interface application.
 *
 * 	Copyright (C) 2013 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include "spi_lcd.h"
#include "spi_config.h"
#include "UI_Timer.h"
#include "UI_Display.h"
#include "bitmap.h"

#define LCD_CLR 		0x20
#define LCD_CMD 		0x80
#define LCD_TRAILER 	0x00

#define LCD_X_MAX		400
#define LCD_Y_MAX		240

#define LCD_LINE_BYTES	LCD_X_MAX/8

uint_8 frame_buff_temp[12500];
MQX_FILE_PTR spifd;
LWGPIO_STRUCT lcd_cs;
LWGPIO_STRUCT REG_SHDN;
LWGPIO_STRUCT DISP_ON;
static LWGPIO_STRUCT 	LCD_BL;
static void lcd_display_backlight_pin_off(void);
static void lcd_display_backlight_pin_on(void);
static void lcd_display_backlight_pin_init(void);


/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_deinit
 *  Brief:        This function deinit lcd GPIO pins
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_deinit(void)
{
    lcd_display_power_pin_off();
    lcd_display_on_pin_off();
    if (MQX_OK == fclose(spifd))
    {
        printf("\nSP2 closed succesfully\n");
    }
    else
    {
        printf("\nSP2 close Error\n");
    }
    
}


/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_power_pin_init
 *  Brief:        This function initialise LCD power pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_power_pin_init(void)
{
    if (!lwgpio_init(&REG_SHDN, BSP_SHDN_LCD, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        
    }
    lwgpio_set_functionality(&REG_SHDN, BSP_SHDN_LCD_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
}


/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_power_pin_on
 *  Brief:        This function Turns on LCD power
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_power_pin_on(void)
{
    
    lwgpio_set_value(&REG_SHDN, LWGPIO_VALUE_HIGH);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_power_pin_off
 *  Brief:        This function Turns Off LCD power
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_power_pin_off(void)
{
    
    lwgpio_set_value(&REG_SHDN, LWGPIO_VALUE_LOW);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_on_pin_init
 *  Brief:        This function inits  Display on pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_on_pin_init(void)
{
    if (!lwgpio_init(&DISP_ON, BSP_DISP_ON_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        
    }
    lwgpio_set_functionality(&DISP_ON, LWGPIO_MUX_B6_GPIO); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
}

/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_on_pin_on
 *  Brief:        This function Turns on Display ON pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_on_pin_on(void)
{
    
    lwgpio_set_value(&DISP_ON, LWGPIO_VALUE_HIGH);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_on_pin_off
 *  Brief:        This function Turns on Display OFF pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_on_pin_off(void)
{
    
    lwgpio_set_value(&DISP_ON, LWGPIO_VALUE_LOW);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_init
 *  Brief:        This function inits LCD GPIO pins
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void lcd_display_init(void)
{
    lcd_display_power_pin_init();
    lcd_display_power_pin_on();
    
    lcd_display_on_pin_init();
    lcd_display_on_pin_on();
    
}


/*-----------------------------------------------------------------------------
 *  Function:     reverse
 *  Brief:        This function reverse a byte to adjust endian
 *  Parameter:    byte to reverse
 *  Return:       reveres byte
 -----------------------------------------------------------------------------*/

uint_8 reverse(uint_8 byte_input)
{
    
    byte_input = ((byte_input >> 1) & 0x55) | ((byte_input & 0x55) << 1);  // swap odd and even bits
    byte_input = ((byte_input >> 2) & 0x33) | ((byte_input & 0x33) << 2);  // swap consecutive pairs
    byte_input = ((byte_input >> 4) & 0x0F) | ((byte_input & 0x0F) << 4);			// swap nibbles
    
    return byte_input;
}


/*-----------------------------------------------------------------------------
 *  Function:     lcd_clear
 *  Brief:        This function clears data on LCD memory
 *  Parameter:    none
 *  Return:       status
 -----------------------------------------------------------------------------*/
_mqx_int lcd_clear(void)			//pointer user_data,MQX_FILE_PTR spifd
{
    
    _mqx_int result = 0;
    uint_8 buffer[10], index = 0;
    //LWGPIO_STRUCT_PTR lcd_cs = (LWGPIO_STRUCT_PTR)user_data;
    
    /*LCD  Clearing command */
    buffer[index++] = LCD_CLR;
    buffer[index++] = LCD_TRAILER;
    
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_HIGH);
    result = fwrite (buffer, 1,2, spifd);
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);
    
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_HIGH);
    result = fwrite (buffer, 1,2, spifd);
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);
    
    if (!result)
    {
        printf("writing error..!");
    }
    return result;
}



/*-----------------------------------------------------------------------------
 *  Function:     Refresh_Lcd_Buffer
 *  Brief:        This function Writes data on LCD from Frame Buffer.
 *  Parameter:    none
 *  Return:       status
 -----------------------------------------------------------------------------*/
_mqx_int Refresh_Lcd_Buffer(uint_8 * buff)  //pointer user_data,MQX_FILE_PTR spifd,
{
//    uint_8 frame_buff_temp[12500];
    _mqx_int result = 0,index = 0,loop;
    uint_8 line_num = 1;
    
    ui_Hwtimer_stop();
    Stop_PDB_Timer();
    
    /* LCD write Command */ 
    frame_buff_temp[index++] = LCD_CMD;                      				 
    for (line_num = 1; line_num <= LCD_Y_MAX; line_num++)
    {
        loop = 0;        
        /* LCD line Number */   
        frame_buff_temp[index++] = reverse(line_num);                      
        while (loop++ < LCD_LINE_BYTES)
        {
            /* Updating Lines */ 
            frame_buff_temp[index++] =~(*buff++);		    				   
        }
        /* Adding trailer */  
        frame_buff_temp[index++] = LCD_TRAILER;									 
    }
    
    /* Adding trailer */ 
    frame_buff_temp[index] = LCD_TRAILER;		    
    /* setting CS High */
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_HIGH);	    
//    _time_delay(500);
//    Time_Delay_Sleep(100);
    printf("\n\nU\n");	
    fflush(spifd);
    result = fwrite (frame_buff_temp, 1,index,spifd); 
    fflush(spifd);    
    printf("\nC=%d\n\n",result);     
    /* setting CS Low */ 
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);     
    
    if (result != index)
    {
        printf("writing error..!");
    }  
    
    ui_Hwtimer_start();
    Start_PDB_Timer();
    
    return result;
}

/*-----------------------------------------------------------------------------
 *  Function:     Refresh_Lcd_Buffer_Loading_Image
 *  Brief:        This function Writes data on LCD from Frame Buffer.
 *  Parameter:    none
 *  Return:       status
 -----------------------------------------------------------------------------*/
_mqx_int Refresh_Lcd_Buffer_Loading_Image(uint_8 * buff)  //pointer user_data,MQX_FILE_PTR spifd,
{
#if 0
    uint_8 frame_buff_temp[12500], loading_buff[7500];
    _mqx_int result = 0,index = 0,loop, buff_start_index=0, buff_end_index=0;
    uint_8 line_num = 1;
    
    /* LCD write Command */ 
    frame_buff_temp[index++] = LCD_CMD;                      				 
    for (line_num = 1; line_num <= LCD_Y_MAX; line_num++)
    {
    	if(line_num == 50)
    	{
    		buff_start_index = index;
    	}
        loop = 0;        
        /* LCD line Number */   
        frame_buff_temp[index++] = reverse(line_num);                      
        while (loop++ < LCD_LINE_BYTES)
        {
            /* Updating Lines */ 
            frame_buff_temp[index++] =~(*buff++);		    				   
        }
        /* Adding trailer */  
        frame_buff_temp[index++] = LCD_TRAILER;	
        if(line_num== 200)
        {
        	buff_end_index = index -1;
        }
    }
    /* Adding trailer */ 
    frame_buff_temp[index] = LCD_TRAILER;	
    index = 0;
    loading_buff[index++] = LCD_CMD;
    while(buff_start_index <= buff_end_index)
    {
    	loading_buff[index++] = frame_buff_temp[buff_start_index++];
    }
    loading_buff[index] = LCD_TRAILER;
	    
    /* setting CS High */
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_HIGH);	    
    /* Updating each line */
    result = fwrite (loading_buff, 1,index,spifd);      
    /* setting CS Low */ 
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);     
    
    if (result != index)
    {
        printf("writing error..!");
    }   
    return result;
#else

    uint_8 frame_buff_temp[8000];
    _mqx_int result = 0,index = 0,loop;
    uint_8 line_num = 60;
    int ctr;
    ctr=line_num*50;
    /* LCD write Command */ 
    frame_buff_temp[index++] = LCD_CMD;                      				 
    for (; line_num <= 175; line_num++)
    {
        loop = 0;        
        /* LCD line Number */   
        frame_buff_temp[index++] = reverse(line_num);                      
        while (loop++ < LCD_LINE_BYTES)
        {
            /* Updating Lines */ 
            frame_buff_temp[index++] =~(buff[ctr++]);		    				   
        }
        /* Adding trailer */  
        frame_buff_temp[index++] = LCD_TRAILER;									 
    }
    
    /* Adding trailer */ 
    frame_buff_temp[index] = LCD_TRAILER;		    
    /* setting CS High */
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_HIGH);
    
    printf("\n\nU\n");	
    fflush(spifd);
    result = fwrite (frame_buff_temp, 1,index,spifd); 
    fflush(spifd);    
    printf("\nC=%d\n\n",result);   
    
    /* setting CS Low */ 
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);     
    
    if (result != index)
    {
        printf("writing error..!");
    }   
    return result;

#endif
}
/*-----------------------------------------------------------------------------
 *  Function:     spi_init
 *  Brief:        This function Initialise spi configuration for LCD.
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void spi_init(void)
{
    uint_32 param;
    
#if BSP_SPI_LCD_CHANNEL == 0
    
#if ! BSPCFG_ENABLE_SPI0
#error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL "spi0:"
#endif
    
#elif BSP_SPI_LCD_CHANNEL == 1
    
#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL "spi1:"
#endif
    
#elif BSP_SPI_LCD_CHANNEL == 2
    
#if ! BSPCFG_ENABLE_SPI2
#error This application requires BSPCFG_ENABLE_SPI2 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL  "spi2:"
#endif
    
#else
    
#error Unsupported SPI channel number. Please check settings of BSP_SPI_MEMORY_CHANNEL in BSP.
    
#endif
    
    /******************************************************************************/
    /*      			 LCD Chip Select   										  */
    /******************************************************************************/
    
#if BSP_CS_LCD	
    
    if (!lwgpio_init(&lcd_cs, BSP_CS_LCD, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        Time_Delay_Sleep(200L);
        _task_block();
    }
    lwgpio_set_functionality(&lcd_cs, BSP_CS_LCD_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);
#endif  
    
    /*Init LCD Power*/
    lcd_display_init();
    
    /*Open the SPI driver */
    spifd = fopen(TEST_CHANNEL, NULL );
    if (NULL == spifd)
    {
        printf("Error opening SPI driver!\n");
        Time_Delay_Sleep(200L);
        _task_block();
    }
    
    /* Display baud rate */
    /* Set a baudrate to 4MHz (default :9600000 ) */
    /******************************************************************************/
    
    param = SPI_CLK_FREQ;
//    printf("Changing the baud rate to %d Hz ... ", param);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_BAUD, &param))
    {
        printf("OK\n");
    }
    else
    {
        printf("ERROR\n");
    }
    
//    /* Display baud rate */printf("Current baud rate ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
//        printf("%d Hz\n", param);
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set clock mode */
    /******************************************************************************/
    param = SPI_CLK_POL_PHA_MODE0;
//    printf("Setting clock mode to %s ... ", clock_mode[param]);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get clock mode */printf("Getting clock mode ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_MODE, &param))
    {
//        printf("%s\n", clock_mode[param]);
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set endian */
    /******************************************************************************/
    param = SPI_DEVICE_BIG_ENDIAN;
//    printf("Setting endian to %s ... ",
//            param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_ENDIAN, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get endian */printf("Getting endian ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_ENDIAN, &param))
    {
//        printf("%s\n",
//                param == SPI_DEVICE_BIG_ENDIAN ?
//                        "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set transfer mode */
    /******************************************************************************/
    param = SPI_DEVICE_MASTER_MODE;
//    printf("Setting transfer mode to %s ... ", device_mode[param]);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get transfer mode */printf("Getting transfer mode ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &param))
    {
//        printf("%s\n", device_mode[param]);
    }
    else
    {
//        printf("ERROR\n");
    }
    
}


void Upadate_lcd_settings(uint_32 baud_rate)
{
    uint_32 param;
    
#if BSP_SPI_LCD_CHANNEL == 0
    
#if ! BSPCFG_ENABLE_SPI0
#error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL "spi0:"
#endif
    
#elif BSP_SPI_LCD_CHANNEL == 1
    
#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL "spi1:"
#endif
    
#elif BSP_SPI_LCD_CHANNEL == 2
    
#if ! BSPCFG_ENABLE_SPI2
#error This application requires BSPCFG_ENABLE_SPI2 defined non-zero in user_config.h. Please recompile kernel with this option.
#else
#define TEST_CHANNEL  "spi2:"
#endif
    
#else
    
#error Unsupported SPI channel number. Please check settings of BSP_SPI_MEMORY_CHANNEL in BSP.
    
#endif
    
    /******************************************************************************/
    /*      			 LCD Chip Select   										  */
    /******************************************************************************/
    
#if BSP_CS_LCD	
    
    if (!lwgpio_init(&lcd_cs, BSP_CS_LCD, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        Time_Delay_Sleep(200L);
        _task_block();
    }
    lwgpio_set_functionality(&lcd_cs, BSP_CS_LCD_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    lwgpio_set_value(&lcd_cs, LWGPIO_VALUE_LOW);
#endif  
    
    /*Init LCD Power*/
    lcd_display_init();
    
    /*Open the SPI driver */
    spifd = fopen(TEST_CHANNEL, NULL );
    if (NULL == spifd)
    {
        printf("Error opening SPI driver!\n");
        Time_Delay_Sleep(200L);
        _task_block();
    }
    
    /* Display baud rate */
    /* Set a baudrate to 4MHz (default :9600000 ) */
    /******************************************************************************/

//    printf("Changing the baud rate to %d Hz ... ", param);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_BAUD, &baud_rate))
    {
        printf("OK\n");
    }
    else
    {
        printf("ERROR\n");
    }
    
//    /* Display baud rate */printf("Current baud rate ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
//        printf("%d Hz\n", param);
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set clock mode */
    /******************************************************************************/
    param = SPI_CLK_POL_PHA_MODE0;
//    printf("Setting clock mode to %s ... ", clock_mode[param]);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get clock mode */printf("Getting clock mode ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_MODE, &param))
    {
//        printf("%s\n", clock_mode[param]);
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set endian */
    /******************************************************************************/
    param = SPI_DEVICE_BIG_ENDIAN;
//    printf("Setting endian to %s ... ",
//            param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_ENDIAN, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get endian */printf("Getting endian ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_ENDIAN, &param))
    {
//        printf("%s\n",
//                param == SPI_DEVICE_BIG_ENDIAN ?
//                        "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else
    {
//        printf("ERROR\n");
    }
    
    /* Set transfer mode */
    /******************************************************************************/
    param = SPI_DEVICE_MASTER_MODE;
//    printf("Setting transfer mode to %s ... ", device_mode[param]);
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
//        printf("OK\n");
    }
    else
    {
//        printf("ERROR\n");
    }
    
//    /* Get transfer mode */printf("Getting transfer mode ... ");
    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &param))
    {
//        printf("%s\n", device_mode[param]);
    }
    else
    {
//        printf("ERROR\n");
    }
    
}

/*-----------------------------------------------------------------------------
 *  Function:     buff_clear
 *  Brief:        This function clears data on frame buffer.
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void buff_clear()
{
    uint_16 index = 0;
    
    for (index = 0; index < 12000; index++)
    {
        frame_buff[index] = 0x00;
    }
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Set_lcd_buad_rate
 *  Brief:        This function clears data on frame buffer.
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Set_lcd_buad_rate(uint_32 buad_rate)
{
	
	
	//  printf("Changing the baud rate to %d Hz ... ", param);
	    if (SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_BAUD, &buad_rate))
	    {
	        printf("OK\n");
	    }
	    else
	    {
	        printf("Freq Changing ERROR\n");
	    }

}
/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_backlight_pin_init
 *  Brief:        This function initialise LCD backlight pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void lcd_display_backlight_pin_init(void)
{
    if (!lwgpio_init(&LCD_BL, BSP_BLEN_LCD, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        
    }
    lwgpio_set_functionality(&LCD_BL, BSP_BLEN_LCD_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    lwgpio_set_value(&LCD_BL, LWGPIO_VALUE_HIGH);
}


/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_backlight_pin_on
 *  Brief:        This function Turns on LCD backlight
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void lcd_display_backlight_pin_on(void)
{
    
    lwgpio_set_value(&LCD_BL, LWGPIO_VALUE_HIGH);
    
}


/*-----------------------------------------------------------------------------
 *  Function:     lcd_display_backlight_pin_off
 *  Brief:        This function Turns off LCD backlight
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void lcd_display_backlight_pin_off(void)
{
    
    lwgpio_set_value(&LCD_BL, LWGPIO_VALUE_LOW);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     Test_backlight
 *  Brief:        This function Turns off LCD backlight
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Test_backlight(void)
{
	printf("\n Testing LCD Back light...\n");
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : ON",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_init();	
	_time_delay(1000);
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : OFF",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_off();
	_time_delay(1000);
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : ON",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_on();
	_time_delay(1000);
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : OFF",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_off();
	_time_delay(1000);
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : ON",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_on();
	_time_delay(1000);
	
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BACKLIGHT TEST",strlen("BACKLIGHT TEST"));
	Draw_string_new(10,150,(uint_8*)"BACKLIGHT IS : OFF",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	lcd_display_backlight_pin_off();
	_time_delay(1000);
	printf("\n LCD Back light Completed\n");
	
}

