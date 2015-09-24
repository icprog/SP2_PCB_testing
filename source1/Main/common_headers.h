/*
 * common_headers.h
 *
 *  Created on: Jul 6, 2014
 *      Author: Rahul TT
 */

#ifndef COMMON_HEADERS_H_
#define COMMON_HEADERS_H_

#include <stdlib.h> 
#include "string.h"
#include "HAL_CC4000.h"
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <nvic.h>
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include "bitmap.h"
#include "spi_lcd.h"
#include "config.h"
#include "UI_Display.h"
#include "Button_Layout.h"
#include "math.h"
#include "data_acquisition.h"
#include "Hw_Timer.h"
#include "adc_k70.h"
#include "Power_mode.h"
#include "tmp006.h"
#include "lsm303.h"
#include "vector.h"
#include "Hw_Init.h"
#include "HAL_CC4000.h"
#include "Gps.h"
#include "Sd_Card.h"
#include "Hw_Timer.h"
#include "Bulk_transfer.h"
#include "Ble_uart.h"
#include "UI_Timer.h"
#include "Bulk_transfer.h"
#include "lsm303.h"
#include "tmp006.h"
#include "disk.h"
#include "mqx_prv_new.h"
#include "Hw_Timer.h"
#include "lsm303.h"
#include "config.h"
#include "Sd_Card.h"
#include "tmp006.h"
#include "data_acquisition.h"
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include <fio.h>
#include "Data_Processessing_Functions.h"
#include "bitmap.h"
#include "spi_lcd.h"
#include "config.h"
#include "spi_config.h"
#include "UI_Display.h"
#include "UI_task.h"
#include "UI_Display_Time.h"
#include "Button_Layout.h"
#include "UI_Timer.h"
#include "Buzzer_Functions.h"
#include "lsm303.h"
#include "UI_Settings.h"
#include "Sd_Card.h"
#include "data_acquisition.h"
#include "disk.h"
#include "Sensor_Configuration.h"
#include "Power_mode/Power_mode.h"
#include "lsm303.h"
#include "Mqx_rtc.h"
#include "adc_k70.h"
#include "calib.h"
#include "algorithm.h"
#include "common_headers.h"
#include "calibration.h"

typedef union
{
	uint32_t Total_Test_Ct;
	uint8_t Total_Test_Ctr_Buff[4];
}TEST_COUNT_FLASH;

typedef union
{
	float var_float;
	char var_char_float[4];
}FLOAT_TO_CHAR;

typedef struct
{
	uint8_t Update_Status;
	uint8_t CRC_Status;
}IMAGE_PARA;
extern IMAGE_PARA SP2_IMG,CALIB_IMG;

extern TEST_COUNT_FLASH Test_Count_Flash;


#if MQX_ROM_VECTORS
#define FLASH_NAME 			"flashx:bank3"
#else
#define FLASH_NAME 			"flashx:bank0"
#endif


#define DFU_ADDR_LOC		0x24B70
#define TEST_ID_ADDR_LOC	0x24B75
#define IRDMS_ADDR_LOC		0x24B7A
#define PRESS_ADDR_LOC		0x24B8F
#define ROS_ADDR_LOC		0x24BA4
#define ACC_ADDR_LOC		0x24BB9
#define MAG_ADDR_LOC		0x24BCE
#define CALIB_SECTOR		0x20000
#define SERIAL_ADDR_LOC 	0x21000
#define TID_BAT				0x22000
#define ACC_CALIB_SECTOR	0x23000
#define DFU_SECTOR			0x24000
#define STRING_SECTOR		0x25000

//#define TEST_LOW_POWER
#define AUTOMATE_TEST 0
#define ALGO_TEST 1
#define GPS_OFF_DURING_TEST 1
#define FLASH_ADC_TEST 0
#define DYNAMIC_UPDATE_TIME	1

#define LAST_STATE_IS_FAVOURITE	1
#define LAST_STATE_IS_FILE_LIST 0

//#define TEST_MENU_BACK_BUTTON_MAIN_MENU		1
//#define TEST_MENU_BACK_BUTTON_DETAILS_LIST	0

/* Task Index */

#define BLE_PROCESS_TASK (6)
#define TEST_TASK 		1
#define UI_TASK         3
#define SW_EVENT_MASK          (1 << 0)
#define PWR_RAIL_EN_PIN			(GPIO_PORT_C|GPIO_PIN9)
#define PWR_RAIL_EN_PIN_MUX		(LWGPIO_MUX_C9_GPIO)
#define KILL_EN_PIN			(GPIO_PORT_A|GPIO_PIN14)
#define KILL_EN_PIN_MUX		(LWGPIO_MUX_A14_GPIO)
extern CalibTable IRDMS_Calib_Table[];
extern CalibTable Pressure_Calib_Table[];
extern CalibTable ROS_Calib_Table[];
extern void Test_task (uint_32);
extern MQX_FILE_PTR   flash_file;
void Lsm_Task(uint_32);
extern void UI_Task (uint_32);
extern void power_rail_disable(void);
extern void power_rail_enable(void);
extern void power_rail_init(void);
extern char Serial_Numbr[15];
void flash_adc_loop_test();
extern uint_16 img_date_word, img_time_word;
extern uint_16 calib_date_word, calib_time_word;
extern MFS_DATE_TIME_PARAM Image_File_Attr;
extern unsigned char LCD_Updating_Flag,LSM_Updating_Flag;
extern unsigned char Processing_Display;
extern LWEVENT_STRUCT                  app_event,Lsm_Event;
extern uint8_t Calib_Flash_Buf[120];
void Time_Delay_Sleep(unsigned int delay_numbr);
#endif /* COMMON_HEADERS_H_ */
