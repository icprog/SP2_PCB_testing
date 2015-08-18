///*
// * tmp006.h
// *
// *  Created on: Dec 26, 2013
// *      Author: GADGEON PC
// */
//
//#ifndef TMP006_H_
//#define TMP006_H_
///**HEADER********************************************************************
//* 
//* Copyright (c) 2008 Freescale Semiconductor;
//* All Rights Reserved                       
//*
//*************************************************************************** 
//*
//* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
//* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
//* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
//* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
//* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
//* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
//* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//* THE POSSIBILITY OF SUCH DAMAGE.
//*
//**************************************************************************
//*
//* $FileName: eeprom.h$
//* $Version : 3.8.6.0$
//* $Date    : Jun-6-2012$
//*
//* Comments:
//*
//*   This file contains definitions for the I2C master example.
//*
//*END************************************************************************/
//
//#if defined(BSP_MPC8308RDB)
//#define I2C_DEVICE_PAGE_SIZE       0x40     /* Maximum bytes per write for M24256 */
//#define I2C_DEVICE_MEMORY_WIDTH    0x02     /* Memory address width in bytes */
//#elif defined(BSP_M53015EVB)
//#define I2C_DEVICE_PAGE_SIZE       0x08     /* Maximum bytes per write for AT24C02B */
//#define I2C_DEVICE_MEMORY_WIDTH    0x01     /* Memory address width in bytes */
//#else
//#define I2C_DEVICE_PAGE_SIZE       0x10     /* Maximum bytes per write for 24LC16 */
//#define I2C_DEVICE_MEMORY_WIDTH    0x01     /* Memory address width in bytes */
//#endif
//
/////* The I2C EEPROM test addresses */
////#define I2C_EEPROM_BUS_ADDRESS     0x32     /* I2C bus address of EEPROM */
////#define I2C_EEPROM_MEMORY_ADDRESS1 0x20     /* Memory test address 1 */
////#define I2C_EEPROM_MEMORY_ADDRESS2 0x23     /* Memory test address 2 */
//
//
//
///*TEMP006 address*/
//#define TMP006_ADDRESS              0x40    
//#define CONFIG_REG_ADDRESS          0x02  
//#define TMP006_V_OBJ_REG            0x00     /**< Base address of V-OBJECT register */
//#define TMP006_T_AMBIENT_REG        0x01     /**< Base address of T-AMBIENT register */
//#define TMP006_CONFIG_REG           0x02     /**< Base address of Configuration register */
//#define TMP006_MANUFACT_ID_REG      0xFE     /**< Base address of Manufacturer ID register */
//#define TMP006_DEVICE_ID_REG        0xFF     /**< Base address of Device ID register */
//
//// Constants for calculating object temperature
//#define TMP006_B0 -0.0000294
//#define TMP006_B1 -0.00000057
//#define TMP006_B2 0.00000000463
//#define TMP006_C2 13.4
//#define TMP006_TREF 298.15
//#define TMP006_A2 -0.00001678
//#define TMP006_A1 0.00175
//
//#define TMP006_S0 6.4  // * 10^-14  
//#define TMP006_ENABLE_POWER_DOWN       0x04     /* Enable power down mode in TMP006 (no conversion) */
//#define TMP006_ENABLE_CONVERSION       0x74     /* Enable continious conversion mode in TMP006      */
//#define TMP006_CONFIG_REG_LSB          0x00     /* Default value of Least Significant Byte(LSB) in Configuration register */
//
//
//
///* Global variables */
//extern LWGPIO_STRUCT  TMP_pin;
//extern LWSEM_STRUCT tmp006_lock;
//extern uchar interrupt_flag,start_temp_flag,stop_temp_flag;
//extern MQX_FILE_PTR       tmp006_fd;
//extern uchar_ptr    tmp006_buffer;
//extern ieee_double Ambient_temperature,Object_temperature,ambient_tmp_start,ambient_tmp_stop,Object_tmp_start,Object_tmp_stop;
//
//extern  char TMP_START_STATUS,TMP_STOP_STATUS;
//
//
//extern uchar CONFIG_CON_MODE[2],CONFIG_POW_DOWN_MODE[2];
//
///* Function prototypes */
//extern void  TMP006_enable_powerdown_mode       (void);
//extern void  TMP006_enable_continuous_conversion(void);
//extern void int_service_routine_tmp006(void *);   /*gpio isr for interrupt read tmp006*/
//extern void init_gpio_isr_tmp006(void);
//extern double get_object_temperature( MQX_FILE_PTR tmp006_fd,uchar_ptr  tmp006_buffer,char flag);
//extern int_16 get_V_object_data( MQX_FILE_PTR tmp006_fd,uchar_ptr  tmp006_buffer);
//extern int_16 get_T_ambient_data( MQX_FILE_PTR tmp006_fd,uchar_ptr  tmp006_buffer);
//extern void i2c_write_polled (MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);
//extern void i2c_read_polled (MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);
////extern void i2c_write_eeprom_interrupt (MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);
////extern void i2c_read_eeprom_interrupt (MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int);
//
//extern void init_tmp006_i2c1(void);
//void Tmp006_Read(char test_time);
//#endif /* TMP006_H_ */


/*
 * tmp006.h
 *
 *  Created on: Dec 26, 2013
 *      Author: GADGEON PC
 */

#ifndef TMP006_H_
#define TMP006_H_
/**HEADER********************************************************************
 * 
 * Copyright (c) 2008 Freescale Semiconductor;
 * All Rights Reserved                       
 *
 *************************************************************************** 
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************
 *
 * $FileName: eeprom.h$
 * $Version : 3.8.6.0$
 * $Date    : Jun-6-2012$
 *
 * Comments:
 *
 *   This file contains definitions for the I2C master example.
 *
 *END************************************************************************/

#if defined(BSP_MPC8308RDB)
#define I2C_DEVICE_PAGE_SIZE        (0x40)     /* Maximum bytes per write for M24256 */
#define I2C_DEVICE_MEMORY_WIDTH     (0x02)     /* Memory address width in bytes */
#elif defined(BSP_M53015EVB)
#define I2C_DEVICE_PAGE_SIZE        (0x08)     /* Maximum bytes per write for AT24C02B */
#define I2C_DEVICE_MEMORY_WIDTH     (0x01)     /* Memory address width in bytes */
#else
#define I2C_DEVICE_PAGE_SIZE        (0x10)     /* Maximum bytes per write for 24LC16 */
#define I2C_DEVICE_MEMORY_WIDTH     (0x01)    /* Memory address width in bytes */
#endif

/*TEMP006 address*/
#define TMP006_ADDRESS              (0x40)    
#define CONFIG_REG_ADDRESS          (0x02)  
#define TMP006_V_OBJ_REG            (0x00)     /**< Base address of V-OBJECT register */
#define TMP006_T_AMBIENT_REG        (0x01)     /**< Base address of T-AMBIENT register */
#define TMP006_CONFIG_REG           (0x02)     /**< Base address of Configuration register */
#define TMP006_MANUFACT_ID_REG      (0xFE)     /**< Base address of Manufacturer ID register */
#define TMP006_DEVICE_ID_REG        (0xFF)     /**< Base address of Device ID register */

// Constants for calculating object temperature
#define TMP006_B0    (-0.0000294)
#define TMP006_B1    (-0.00000057)
#define TMP006_B2    (0.00000000463)
#define TMP006_C2    (13.4)
#define TMP006_TREF  (298.15)
#define TMP006_A2    (-0.00001678)
#define TMP006_A1    (0.00175)
#define TMP006_S0    (6.4)
#define TMP006_ENABLE_POWER_DOWN       (0x04)     /* Enable power down mode in TMP006 (no conversion) */
#define TMP006_ENABLE_CONVERSION       (0x74)     /* Enable continious conversion mode in TMP006      */
#define TMP006_CONFIG_REG_LSB          (0x00)     /* Default value of Least Significant Byte(LSB) in Configuration register */

/* Global variables */
extern LWGPIO_STRUCT TMP_pin;
extern uchar TMP_Data_Ready;
extern MQX_FILE_PTR Tmp006_fd;
extern uchar_ptr Tmp006_buffer;
extern ieee_double Ambient_temperature, Object_temperature;
extern uchar Tmp_continuous_mode_command[], Tmp_powerdown_mode_command[];

/*Fuction prototypes*/
extern uint_8 init_tmp006_i2c1(void);
extern void Tmp006_Read(char test_time);
extern uint_8 Tmp006_deinit(void);
uint_8 Tmp006_Init();

#endif /* TMP006_H_ */


