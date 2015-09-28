

#if 0
///*I2C 1
// * PTC10  SCL1 ON A75 -  PTC11 SDA 1 on B71 Interrupt pin 9 data ready on twr14 pinptb0*/
//
//#include <mqx.h>
//#include <bsp.h>
//#include <i2c.h>
//#include "tmp006.h"
//#include <math.h>
//
//
//
//#define TEMP_I2C_POLLED    "i2c1:"
//#define I2C_DEVICE_INTERRUPT "ii2c1:"
//#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C1
//
//#if !BSPCFG_ENABLE_I2C1
//#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
//#endif
//
//
//
//
//#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
//#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
//#endif
//
//
//#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
//#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
//#endif
//
///*PTB0-A14*/
//
//#define BSP_TMP_PIN  (GPIO_PORT_B | GPIO_PIN0)
//#define LWGPIO_MUX_B0_GPIO (1)
//
///*variables for gpio isr for tmp006*/
///* Global variables */
//LWSEM_STRUCT lwsem;
//LWGPIO_STRUCT  TMP_pin;
//uchar interrupt_flag=0,start_temp_flag=0,stop_temp_flag=0;
//int_16 Tdie_temp=0,Vobj_temp=0;
///*
//SET TMP_START_STATUS to flase 
//*/
//char TMP_START_STATUS=TRUE,TMP_STOP_STATUS=FALSE;
//
//#define BUFFER_SIZE_TMP006 6
///*Variables and data TMP006 */
//uchar CONFIG_CON_MODE[]      = {0x75,0x00};/*config bits for continues operation  */
//
//uchar CONFIG_POW_DOWN_MODE[] = {0x00,0x00};/*config bits for power down mode  */
//
//ieee_double Object_temperature=0,Ambient_temperature=0;
//
//ieee_double ambient_tmp_start=0,ambient_tmp_stop=0,Object_tmp_start=0,Object_tmp_stop=0;
//
//LWSEM_STRUCT        tmp006_lock;
//
//MQX_FILE_PTR        tmp006_fd;
//
//uchar_ptr    tmp006_buffer;
//
//
//void init_tmp006_i2c1(void)
//
//
//{
//	 uint_32                        i, j;
//	 _mqx_int                     param, result;
//	 I2C_STATISTICS_STRUCT        stats;
//	 interrupt_flag=0;
//	   /* Allocate receive tmp006_buffer */
//	   tmp006_buffer = _mem_alloc_zero (BUFFER_SIZE_TMP006);
//	   if (tmp006_buffer == NULL) 
//	   {
//	      printf ("ERROR getting receive tmp006_buffer!\n");
//	      _task_block ();
//	   }   
//	   
//	   /* Open the I2C driver */         
//	   tmp006_fd = fopen (TEMP_I2C_POLLED, NULL);
//	   if (tmp006_fd == NULL)    {
//
//	      _task_block ();
//	   }
//
//	   /* Test ioctl commands 100000*/
//		param = 100000;
//		ioctl (tmp006_fd, IO_IOCTL_I2C_SET_BAUD, &param);
//		ioctl (tmp006_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
//		
//		param = 0x60;
//		ioctl (tmp006_fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param);
//		ioctl (tmp006_fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
//		param =TMP006_ADDRESS;
//		ioctl (tmp006_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
//	   
//	/*Configure isr to read TMP interrupt */
//	//   init_gpio_isr_tmp006();
//	/***********************************************************************************/   
//	/*Configuring TMP006
//	 * Interrupt disabled, continuous conversion,sample/sec 1,average of 4 samples */
//	/***********************************************************************************/  
//	   TMP006_enable_continuous_conversion();
//	/***********************************************************************************/ 
//	    i2c_read_polled (tmp006_fd, CONFIG_REG_ADDRESS, tmp006_buffer, 2); 
//	    
//	    if((tmp006_buffer[0]==0x75)&&(tmp006_buffer[1]==0x00))
//	    {
//	    	printf("Temperature Test Success\n");
//	    }
//		else
//		{
//			printf("%02x %02x",tmp006_buffer[0],tmp006_buffer[1]);
//			printf("Temperature Test Fail\n");
//		}
//		
//		printf("\n\n***End of Testing Temperature Sensor***\n\n");
//}
//
//
///**************************************************
// * function to enable power down mode
// * no operation
// * ************************************************
// */
//
//
//
// void  TMP006_enable_powerdown_mode  (void)
// 
// {
//	 i2c_write_polled (tmp006_fd, CONFIG_REG_ADDRESS, (uchar_ptr)CONFIG_POW_DOWN_MODE,2); 
// }
// 
// 
// /**************************************************
//  * function to enable continuous conversion mode
//  * no operation
//  * ************************************************
//  */
// void  TMP006_enable_continuous_conversion(void)
// {
//	   i2c_write_polled (tmp006_fd, CONFIG_REG_ADDRESS, (uchar_ptr)CONFIG_CON_MODE,2); 
//	 
// }
//
//
///*ISR*-----------------------------------------------------
//*
//* Task Name    : int_service_routine
//* Comments     :
//* Button press ISR
//*END*-----------------------------------------------------*/
//void int_service_routine_tmp006(void *pin)
//{  
//    lwgpio_int_enable(&TMP_pin, FALSE);
//    
//	lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
//	
//	Tdie_temp=0;	
//	
//    Vobj_temp=0;
//    
//	Tdie_temp=get_T_ambient_data(tmp006_fd,tmp006_buffer);  /*Temperature Raw data in 2's compliment form*/
//	
//	Vobj_temp=get_V_object_data(tmp006_fd,tmp006_buffer);   /*Target voltage Raw data in 2's compliment form*/
//	
//	interrupt_flag=1;
//
//}
//
//
//void init_gpio_isr_tmp006(void){
//	
//	
//	/*******************************************************************************
//	Opening the pin (PTE28) for input, initialize interrupt on it and set interrupt handler.
//	*******************************************************************************/
//	    /* opening pins for input */
//	    if (!lwgpio_init(&TMP_pin, BSP_TMP_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
//	    {
//	        _task_block();
//	    }
//	    /* Some platforms require to setup MUX to IRQ functionality,
//	    for the rest just set MUXto GPIO functionality */
//	    lwgpio_set_functionality(&TMP_pin, LWGPIO_MUX_B0_GPIO);
//	    lwgpio_set_attribute(&TMP_pin, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
//
//	    /* enable gpio functionality for given pin, react on falling edge */
//	    if (!lwgpio_int_init(&TMP_pin, LWGPIO_INT_MODE_FALLING))
//	    {
//	        _task_block();
//	    }
//	    /* install gpio interrupt service routine */
//	    _int_install_isr(lwgpio_int_get_vector(&TMP_pin), int_service_routine_tmp006, (void *) &TMP_pin);
//	    /* set the interrupt level, and unmask the interrupt in interrupt controller*/
//	    _bsp_int_init(lwgpio_int_get_vector(&TMP_pin), 3, 0, TRUE);
//	    /* enable interrupt on GPIO peripheral module*/
//	    lwgpio_int_enable(&TMP_pin, TRUE);
////	    /* disable interrupt on GPIO peripheral module*/
////	    lwgpio_int_enable(&btn1, FALSE);
//}
//
//
//
///*To read Object temperature raw data from tmp006*/
//int_16 get_V_object_data   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR tmp006_fd,
//
//      /* [IN] The array of characters to be written into */
//      uchar_ptr  tmp006_buffer)
//{   
//	int_16 temp_data=0;
//	
//    tmp006_buffer[0] = 0x00;   
//    
//    tmp006_buffer[1] = 0x00;
//    
//    i2c_read_polled (tmp006_fd, TMP006_V_OBJ_REG, tmp006_buffer, 2);
//    
//    temp_data=(int_16)(((uint_16)tmp006_buffer[0] << 8) |(uint_16) tmp006_buffer[1]);	
//    
//	return temp_data;	
//}
//
//
///*To read Ambient  temperature raw data from tmp006*/
//
//int_16 get_T_ambient_data   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR tmp006_fd,
//
//      /* [IN] The array of characters to be written into */
//      uchar_ptr  tmp006_buffer)
//{   
//	int_16 temp_data=0;
//	
//    tmp006_buffer[0] = 0x00;   
//
//    tmp006_buffer[1] = 0x00;
//    
//    i2c_read_polled (tmp006_fd, TMP006_T_AMBIENT_REG, tmp006_buffer, 2);
//    
//	temp_data=(int_16)(((uint_16)tmp006_buffer[0] << 8) |(uint_16) tmp006_buffer[1]);
//	
//	temp_data=(temp_data>>2);
//	
//	if((temp_data&0x20)==0X20)
//		
//		temp_data|=0xC0;
//	
//	return temp_data;	
//}
//
//
///*To calculate  Object temperature and ambient temperature  from tmp006 raw data*/
//ieee_double get_object_temperature   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR tmp006_fd,
//
//      /* [IN] The array of characters to be written into */
//      uchar_ptr  tmp006_buffer,char flag)
//{   
//
//	volatile ieee_double Tdie,Vobj,tdie_tref,S,fVobj,Tobj,Vos;	
//	
//	Tdie =(ieee_double)Tdie_temp*0.03125;                   /* convert to celsius*/	
//
//	Ambient_temperature=Tdie;
//	
//	if(flag==0)
//		
//		ambient_tmp_start=Ambient_temperature;
//	
//	else if(flag==1)
//		
//		ambient_tmp_stop=Ambient_temperature;	
//	
//	Vobj =(ieee_double)Vobj_temp* 156.25;                    /*156.25 nV per LSB*/ 
//	
//	Vobj /= 1000000000; // nV -> V
//
//	Tdie += 273.15; // convert to kelvin
//
//
//	// Equations for calculating temperature found in section 5.1 in the user guide
//	tdie_tref = Tdie - TMP006_TREF;
//	
//	S = (1 + (TMP006_A1*tdie_tref) + (TMP006_A2*tdie_tref*tdie_tref));
//	
//	S *= TMP006_S0;
//	
//	S /= 10000000;
//	
//	S /= 10000000;	
//	
//	Vos = (TMP006_B0 + (TMP006_B1*tdie_tref) +(TMP006_B2*tdie_tref*tdie_tref));
//	
//	fVobj = (Vobj - Vos) + (TMP006_C2*(Vobj-Vos)*(Vobj-Vos));
//	
//	Tobj = sqrt(sqrt((Tdie * Tdie * Tdie * Tdie) + (fVobj/S)));
//	
//	Tobj -= 273.15; // Kelvin -> *C
//	
//	
//	if(flag==0)
//		
//		Object_tmp_start=Tobj;
//	
//	else if(flag==1)
//		
//		Object_tmp_stop=Tobj;	
//	
//    return Tobj;
//
//}
//
//
///*FUNCTION****************************************************************
//* 
//* Function Name    : i2c_write_polled
//* Returned Value   : void
//* Comments         : 
//*   Writes the provided data tmp006_buffer at address in I2C device
//*
//*END*********************************************************************/
//
//void i2c_write_polled
//   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR tmp006_fd,
//
//      /* [IN] The address in  to write to  i2c device*/
//      uint_32    addr,
//
//      /* [IN] The array of characters are to be written in i2c device */
//      uchar_ptr  tmp006_buffer,
//
//      /* [IN] Number of bytes in that tmp006_buffer */
//      _mqx_int   n      
//   )
//{ /* Body */
//
//   uint_32       param;
//   _mqx_int    length;
//   _mqx_int    result;
//   uint_8        mem[I2C_DEVICE_MEMORY_WIDTH];
//
//   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&tmp006_lock);
//   do
//   {
//      /* I2C bus address also contains memory block index */
//      param = TMP006_ADDRESS;
//      ioctl (tmp006_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
//
//      length = (_mqx_int)(I2C_DEVICE_PAGE_SIZE - (addr & (I2C_DEVICE_PAGE_SIZE - 1)));
//      if (length > n) length = n;
//
//      /* Initiate start and send I2C bus address */
//      fwrite (mem, 1, 0, tmp006_fd);
//
//      /* Check ack (device exists) */
//      if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
//      {
//
//          if (param) 
//          {
//             /* Stop I2C transfer */
//            
//             if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
//             {
//
//             } else {
//
//             }
//             break;
//          }
//       } else {
//     	  
//       }
//
//      /* Write address within memory block */
//#if I2C_DEVICE_MEMORY_WIDTH == 2
//      mem[0] = (uint_8)(addr >> 8);
//      mem[1] = (uint_8)addr;
//      result = fwrite (mem, 1, 2, tmp006_fd);
//
//#else
//      mem[0] = (uint_8)addr;
//
//      result = fwrite (mem, 1, 1, tmp006_fd);
//
//#endif
//
//      /* Page write of data */
//      result = fwrite (tmp006_buffer, 1, length, tmp006_fd);
//     
//      /* Wait for completion */
//
//      result = fflush (tmp006_fd);
//      /* Stop I2C transfer - initiate i2c device write cycle */
//      ioctl (tmp006_fd, IO_IOCTL_I2C_STOP, NULL);
//      /* Wait for i2c device write cycle finish - acknowledge */
//      result = 0;
//      do 
//      {  /* Sends just I2C bus address, returns acknowledge bit and stops */
//          fwrite (&result, 1,0, tmp006_fd);
//          
//          if (I2C_OK != ioctl (tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
//          {
//
//          }
//          
//          if (I2C_OK != ioctl (tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
//          {
//
//          }
//          result++;
//      } while ((param & 1) || (result <= 1));
//      
//
//      /* Next round */
//      tmp006_buffer += length;
//      addr += length;
//      n -= length;
//        
//   } while (n);
//
//   /* Release the transaction tmp006_lock */
//   _lwsem_post (&tmp006_lock);
//} /* Endbody */
//
//  
//
///*FUNCTION****************************************************************
//* 
//* Function Name    : i2c_read_polled
//* Returned Value   : void
//* Comments         : 
//*   Reads into the provided data tmp006_buffer from address in I2C device
//*
//*END*********************************************************************/
//
//void i2c_read_polled
//   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR tmp006_fd,
//
//      /* [IN] The address in i2c device to read from */
//      uint_32    addr,
//
//      /* [IN] The array of characters to be written into */
//      uchar_ptr  tmp006_buffer,
//
//      /* [IN] Number of bytes to read */
//      _mqx_int   n      
//   )
//{ /* Body */
//   _mqx_int    param;
//   _mqx_int    result;
//   uint_8        mem[I2C_DEVICE_MEMORY_WIDTH];
//
//
//   if (0 == n) 
//   {
//
//      return;
//   }
//
//   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&tmp006_lock);
//   
//   /* I2C bus address also contains memory block index */
//   param = TMP006_ADDRESS;
//
//   if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
//   {
//
//   } else {
//
//   }
//
//   /* Initiate start and send I2C bus address */
//
//   fwrite (mem, 1, 0, tmp006_fd);
//
//   /* Check ack (device exists) */
//   if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
//   {
//
//      if (param) 
//      {
//         /* Stop I2C transfer */
//
//         if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
//         {
//
//         } else {
//
//         }
//         
//         /* Release the transaction tmp006_lock */
//         _lwsem_post (&tmp006_lock);
//         
//         return;
//      }
//   } else {
//
//   }
//
//   /* Write address within memory block */
//#if I2C_DEVICE_MEMORY_WIDTH == 2
//      mem[0] = (uint_8)(addr >> 8);
//      mem[1] = (uint_8)addr;
//
//      result = fwrite (mem, 1, 2, tmp006_fd);
//
//#else
//      mem[0] = (uint_8)addr;
//
//      result = fwrite (mem, 1, 1, tmp006_fd);
//
//#endif
//
//   /* Wait for completion */
//
//   result = fflush (tmp006_fd);
//   if (MQX_OK == result)
//   {
//
//   } else {
//
//   }
//
//   /* Restart I2C transfer for reading */
//
//   if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_REPEATED_START, NULL))
//   {
//
//   } else {
//
//   }
//
//   /* Set read request */
//   param = n;
//
//   if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
//   {
//
//   } else {
//
//   }
//
//   /* Read all data */
//
//   result = fread (tmp006_buffer, 1, n, tmp006_fd);
//   if (result == n)
//   {
//
//   } else {
//
//   }
//      
//   /* Stop I2C transfer - initiate i2c device write cycle */
//
//   if (I2C_OK == ioctl (tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
//   {
//
//   } else {
//
//   }
//   
//   /* Release the transaction tmp006_lock */
//   _lwsem_post (&tmp006_lock);
//} /* Endbody */
#else
/******************************************************************************
 *
 *     Filename:       $tmp006_polled.c
 *     Created on:     $Date: Mar 24, 2014    (6:12:13 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contains functions to configure tmp006
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "tmp006.h"
#include <math.h>
//#include "lsm303.h"
#include "Sd_Card.h"
#include "config.h"
#include "UI_Display.h"
#include "spi_lcd.h"
#include "bitmap.h"
#include "spi_config.h"

#define TEMP_I2C_POLLED    "i2c1:"
#define I2C_DEVICE_INTERRUPT "ii2c1:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C1
#if !BSPCFG_ENABLE_I2C1
#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#define BUFFER_SIZE_TMP006       (6)
#define BSP_TMP006_PIN           (GPIO_PORT_B | GPIO_PIN0)
#define LWGPIO_MUX_B0_GPIO       (1)

/* Global variables */
LWGPIO_STRUCT TMP_pin;
MQX_FILE_PTR Tmp006_fd;
uchar TMP_Data_Ready = 0, start_temp_flag = 0, stop_temp_flag = 0;
int_16 Tdie_temp = 0, Vobj_temp = 0;
uchar_ptr Tmp006_buffer;
uchar Tmp006_Data_buffer[BUFFER_SIZE_TMP006];
uchar Tmp_continuous_mode_command[2] = { 0x71, 0x00 };
uchar Tmp_powerdown_mode_command[2]  = { 0x00, 0x00 };

/* Function prototypes */
static void TMP006_enable_powerdown_mode(void);
static void TMP006_enable_continuous_conversion(void);
static double get_object_temperature( char flag);
static int_16 get_V_object_data(MQX_FILE_PTR tmp006_fd, uchar_ptr tmp006_buffer);
static int_16 get_T_ambient_data(MQX_FILE_PTR tmp006_fd, uchar_ptr tmp006_buffer);
static void i2c_write_polled(MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);
static void i2c_read_polled(MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);

/*-----------------------------------------------------------------------------
 *  Function:     Tmp006_deinit
 *  Brief:        Deinitialise tmp006 and i2c1
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
uint_8 Tmp006_deinit(void)
{
	if(Sensor_status.TMP_status == NOT_INITIALISED)
	{
		printf("TMP already un-initialised..!");
		return 1;
	}
	//    TMP006_enable_powerdown_mode();
	if (MQX_OK != fclose(Tmp006_fd))
	{
		printf("\ni2c1 close Error\n");
		return 1;
	}
	
	printf("TMP  un-initialised..!");
	Sensor_status.TMP_status = NOT_INITIALISED;
	return 0;
}


uint_8 Tmp006_Init()
{
	if(Sensor_status.TMP_status == INITIALISED)
	{
		printf("TMP already initialised..!");
		return 1;
	}
    _mqx_int param;
    
    TMP_Data_Ready = 0;
    /* Allocate receive tmp006_buffer */
    Tmp006_buffer = Tmp006_Data_buffer;
    /* Open the I2C driver */
    Tmp006_fd = fopen(TEMP_I2C_POLLED, NULL);
    if (Tmp006_fd == NULL)
    {
        printf("ERROR tmp006_fd == NULL!\n");
        return 1;
    }
    else
    {
		/* Test ioctl commands 100000*/
    	printf("TMP Init Success!\n");
		param = 100000;
		ioctl(Tmp006_fd, IO_IOCTL_I2C_SET_BAUD, &param);
		ioctl(Tmp006_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
		Sensor_status.TMP_status = INITIALISED;
		return 0;
    }
}

/*-----------------------------------------------------------------------------
 *  Function:     init_tmp006_i2c1
 *  Brief:        initialise tmp006 and i2c1
 *  Parameter:    None
 *  Return:       Status 0=Success,1=Error
 -----------------------------------------------------------------------------*/
uint_8 init_tmp006_i2c1(void)
{
    _mqx_int param;
    uchar tmp006_buffer[3];
    
    TMP_Data_Ready = 0;
    /* Allocate receive tmp006_buffer */
    Tmp006_buffer = Tmp006_Data_buffer;
    /* Open the I2C driver */
    Tmp006_fd = fopen(TEMP_I2C_POLLED, NULL);
    if (Tmp006_fd == NULL)
    {
        printf("ERROR tmp006_fd == NULL!\n");
        return 1;
    }
    /* Test ioctl commands 100000*/
    param = 100000;
    ioctl(Tmp006_fd, IO_IOCTL_I2C_SET_BAUD, &param);
    ioctl(Tmp006_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
    TMP006_enable_continuous_conversion();
    
    i2c_read_polled (Tmp006_fd, CONFIG_REG_ADDRESS, tmp006_buffer, 2); 
    
    if((tmp006_buffer[0]==Tmp_continuous_mode_command[0])&&
    		(tmp006_buffer[1]==Tmp_continuous_mode_command[1]))
    {
    	printf("Temperature Test Success\n");
    	Draw_string_new(10,200, (uint_8 *)"TMP006 TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
    }
	else
	{
		printf("Temperature Test Fail\n");
    	Draw_string_new(10,200, (uint_8 *)"TMP006 TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
	}
	
	printf("\n\n***End of Testing Temperature Sensor***\n\n");
	
    TMP006_enable_powerdown_mode();
    return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     TMP006_enable_powerdown_mode
 *  Brief:        function to enable power down mode in tmp006
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void TMP006_enable_powerdown_mode(void)

{
    i2c_write_polled(Tmp006_fd, CONFIG_REG_ADDRESS, (uchar_ptr) Tmp_powerdown_mode_command, 2);
}

/*-----------------------------------------------------------------------------
 *  Function:     TMP006_enable_continuous_conversion
 *  Brief:        function to enable continuous conversion mode in tmp006
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void TMP006_enable_continuous_conversion(void)
{
    
    i2c_write_polled(Tmp006_fd, CONFIG_REG_ADDRESS, (uchar_ptr) Tmp_continuous_mode_command, 2);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     get_V_object_data
 *  Brief:        To read Object temperature raw data from tmp006
 *  Parameter:    The file pointer for the I2C channel ,The array of characters to be written into
 *  Return:       Object temperature raw data
 -----------------------------------------------------------------------------*/
static int_16 get_V_object_data(
/* [IN] The file pointer for the I2C channel */
MQX_FILE_PTR tmp006_fd,
/* [IN] The array of characters to be written into */
uchar_ptr tmp006_buffer)
{
    int_16 temp_data = 0;
    tmp006_buffer[0] = 0x00;
    tmp006_buffer[1] = 0x00;
    i2c_read_polled(tmp006_fd, TMP006_V_OBJ_REG, tmp006_buffer, 2);
    temp_data = (int_16) (((uint_16) tmp006_buffer[0] << 8) | (uint_16) tmp006_buffer[1]);
    return temp_data;
}

/*-----------------------------------------------------------------------------
 *  Function:     get_T_ambient_data
 *  Brief:        To read Ambient temperature raw data from tmp006
 *  Parameter:    The file pointer for the I2C channel ,The array of characters to be written into
 *  Return:       Ambient temperature raw data
 -----------------------------------------------------------------------------*/
static int_16 get_T_ambient_data(
/* [IN] The file pointer for the I2C channel */
MQX_FILE_PTR tmp006_fd,
/* [IN] The array of characters to be written into */
uchar_ptr tmp006_buffer)
{
    int_16 temp_data = 0;
    tmp006_buffer[0] = 0x00;
    tmp006_buffer[1] = 0x00;
    i2c_read_polled(tmp006_fd, TMP006_T_AMBIENT_REG, tmp006_buffer, 2);
    temp_data = (int_16) (((uint_16) tmp006_buffer[0] << 8) | (uint_16) tmp006_buffer[1]);
    temp_data = (temp_data >> 2);
    if ((temp_data & 0x20) == 0X20)
        temp_data |= 0xC0;
    return temp_data;
}

/*-----------------------------------------------------------------------------
 *  Function:     get_object_temperature
 *  Brief:        To calculate  Object temperature and ambient temperature  from tmp006 
 *                raw data 
 *  Parameter:    Flag to check the time of function call(beginning or end of test)
 *  Return:       Object temperature
 -----------------------------------------------------------------------------*/
ieee_double get_object_temperature   (
char flag)
{
    volatile ieee_double Tdie, Vobj, tdie_tref, S, fVobj, Tobj, Vos;
    Tdie = (ieee_double) Tdie_temp * 0.03125; /* convert to celsius*/
    if (flag == 0)
    {
    	Data_header.Temperature.Ambient_Temperature_Before_Test = Tdie;
    }
    else if (flag == 1)
    {
    	Data_header.Temperature.Ambient_Temperature_After_Test = Tdie;
    }
    
    Vobj = (ieee_double) Vobj_temp * 156.25; /*156.25 nV per LSB*/
    Vobj /= 1000000000; // nV -> V
    Tdie += 273.15;     // convert to kelvin
    // Equations for calculating temperature 
    tdie_tref = Tdie - TMP006_TREF;
    S = (1 + (TMP006_A1 * tdie_tref) + (TMP006_A2 * tdie_tref * tdie_tref));
    S *= TMP006_S0;
    S /= 10000000;
    S /= 10000000;
    Vos = (TMP006_B0 + (TMP006_B1 * tdie_tref) + (TMP006_B2 * tdie_tref * tdie_tref));
    fVobj = (Vobj - Vos) + (TMP006_C2 * (Vobj - Vos) * (Vobj - Vos));
    Tobj = sqrt(sqrt((Tdie * Tdie * Tdie * Tdie) + (fVobj / S)));
    Tobj -= 273.15; // Kelvin -> *C
    if (flag == 0)
    {
    	Data_header.Temperature.Object_Temperature_Before_Test = Tobj;
    }
    else if (flag == 1)
    {
    	Data_header.Temperature.Object_Temperature_After_Test = Tobj;
    }
    return Tobj;
    
}

/*-----------------------------------------------------------------------------
 *  Function:     i2c_write_polled
 *  Brief:        Writes the provided data tmp006_buffer at address in I2C device
 *  Parameter:    The file pointer for the I2C channel,The address in  to write to  i2c device
 *                The array of characters are to be written in i2c device,Number of bytes in that
 *                tmp006_buffer
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void i2c_write_polled(
/* [IN] The file pointer for the I2C channel */
MQX_FILE_PTR tmp006_fd,

/* [IN] The address in  to write to  i2c device*/
uint_32 addr,

/* [IN] The array of characters are to be written in i2c device */
uchar_ptr tmp006_buffer,

/* [IN] Number of bytes in that tmp006_buffer */
_mqx_int n)
{ /* Body */
    uint_32 param;
    _mqx_int length;
    _mqx_int result;
    uint_8 mem[I2C_DEVICE_MEMORY_WIDTH];
    do
    {
        /* I2C bus address also contains memory block index */
        param = TMP006_ADDRESS;
        ioctl(tmp006_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
        length = (_mqx_int) (I2C_DEVICE_PAGE_SIZE - (addr & (I2C_DEVICE_PAGE_SIZE - 1)));
        if (length > n)
            length = n;
        /* Initiate start and send I2C bus address */
        fwrite(mem, 1, 0, tmp006_fd);
        /* Check ack (device exists) */
        if (I2C_OK == ioctl(tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
        {
            if (param)
            {
                /* Stop I2C transfer */

                if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
                {
                    printf("i2c stop ERROR\n");
                    
                }
                break;
            }
        }
        else
        {
            printf("i2c flush output ERROR\n");
            
        }
        
        /* Write address within memory block */
#if I2C_DEVICE_MEMORY_WIDTH == 2
        mem[0] = (uint_8)(addr >> 8);
        mem[1] = (uint_8)addr;
        result = fwrite (mem, 1, 2, tmp006_fd);

#else
        mem[0] = (uint_8) addr;
        result = fwrite (mem, 1, 1, tmp006_fd);
        
#endif
        
        /* Page write of data */
        result = fwrite (tmp006_buffer, 1, length, tmp006_fd);
        if (result != length)
        {
            printf("fwrite ERROR\n");
        }
        /* Wait for completion */
        result = fflush(tmp006_fd);
        if (MQX_OK != result)
        {
            printf("fflush ERROR\n");
        }
        /* Stop I2C transfer - initiate i2c device write cycle */
        if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
        {
            printf("i2c stop ERROR\n");
            
        }
        /* Wait for i2c device write cycle finish - acknowledge */
        result = 0;
        do
        { /* Sends just I2C bus address, returns acknowledge bit and stops */
            fwrite(&result, 1, 0, tmp006_fd);
            if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
            {
                printf("  ERROR during wait (flush)\n");
                
            }
            if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
            {
                printf("  ERROR during wait (stop)\n");
                
            }
            result++;
        } while ((param & 1) || (result <= 1));
        /* Next round */
        tmp006_buffer += length;
        addr += length;
        n -= length;
        
    } while (n);
    
} /* Endbody */

/*-----------------------------------------------------------------------------
 *  Function:     i2c_read_polled
 *  Brief:        Reads into the provided data buffer from address in I2C
 *  Parameter:    The file pointer for the I2C channel,he address in i2c device to read from 
 *                The array of characters to be written into ,Number of bytes to read 
 *  Return:       Data from i2c device
 -----------------------------------------------------------------------------*/

static void i2c_read_polled(
/* [IN] The file pointer for the I2C channel */
MQX_FILE_PTR tmp006_fd,

/* [IN] The address in i2c device to read from */
uint_32 addr,

/* [IN] The array of characters to be written into */
uchar_ptr tmp006_buffer,

/* [IN] Number of bytes to read */
_mqx_int n)
{ /* Body */
    _mqx_int param;
    _mqx_int result;
    uint_8 mem[I2C_DEVICE_MEMORY_WIDTH];
    
    if (0 == n)
    {
        return;
    }
    
    /* I2C bus address also contains memory block index */
    param = TMP006_ADDRESS;
    ioctl(tmp006_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param);
    /* Initiate start and send I2C bus address */
    fwrite(mem, 1, 0, tmp006_fd);
    /* Check ack (device exists) */
    if (I2C_OK == ioctl(tmp006_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
    {
        
        if (param)
        {
            /* Stop I2C transfer */
            if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
            {
                printf("i2c stop ERROR\n");
            }
            
            return;
        }
    }
    else
    {
        printf("i2c flush output ERROR\n");
        
    }
    /* Write address within memory block */
#if I2C_DEVICE_MEMORY_WIDTH == 2
    mem[0] = (uint_8)(addr >> 8);
    mem[1] = (uint_8)addr;

    result = fwrite (mem, 1, 2, tmp006_fd);

#else
    mem[0] = (uint_8) addr;
    result = fwrite (mem, 1, 1, tmp006_fd);
#endif
    
    /* Wait for completion */
    result = fflush(tmp006_fd);
    if (MQX_OK != result)
    {
        printf("fflush ERROR\n");
    }
    /* Restart I2C transfer for reading */
    if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        printf("i2c repeated start ERROR\n");
        
    }
    /* Set read request */
    param = n;
    if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        printf("i2c set rx request ERROR\n");
    }
    /* Read all data */
    result = fread (tmp006_buffer, 1, n, tmp006_fd);
    if (result != n)
    {
        printf("i2c fread ERROR\n");
    }
    /* Stop I2C transfer - initiate i2c device write cycle */
    if (I2C_OK != ioctl(tmp006_fd, IO_IOCTL_I2C_STOP, NULL))
    {
        printf("i2c stop ERROR\n");
    }
}

/*-----------------------------------------------------------------------------
 *  Function:     Tmp006_Read
 *  Brief:        Reads raw data from tmp006
 *  Parameter:    Test time to know the whether it is start or end of test
 *  Return:       void
 -----------------------------------------------------------------------------*/
void Tmp006_Read(char test_time)
{
    TMP_Data_Ready = 0; /*TMP006 interrupt flag*/
    TMP006_enable_continuous_conversion();
    Time_Delay_Sleep(500);
    Tdie_temp = get_T_ambient_data(Tmp006_fd, Tmp006_buffer); /*Temperature Raw data in 2's compliment form*/
    Vobj_temp = get_V_object_data(Tmp006_fd, Tmp006_buffer);
    printf("ambient = %d obj = %d",Tdie_temp,Vobj_temp);
    TMP_Data_Ready = 0;
    TMP006_enable_powerdown_mode();
    get_object_temperature(test_time);
}

void Test_Tmp006(void)
{
	printf("\n********STARTING TMP006 TEST**********\n");
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("TMP006 TEST",strlen("TMP006 TEST"));
	Draw_string_new(15,80, (uint_8 *)"STARTING TMP TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(2000);
	init_tmp006_i2c1();
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(2000);
	printf("\n********TEMPERATURE TEST COMPLETED********\n");
	
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
#endif
