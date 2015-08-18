/*
 * Sd_Card.h
 *
 *  Created on: Dec 30, 2013
 *      Author: GADGEON-03
 */

#ifndef SD_CARD_H_
#define SD_CARD_H_


#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include <fio.h>

#define QUEUE_SIZE 900
#define QUEUE_NOS  12
#define OUTPUT_FILE_FORMAT		'X'
#define RTC_FILE_NAME	"a:__RTC72.bin"
#define FILE_MANIFEST	"a:file_manifest.bin"
#define ALL_FILE		"a:_ALL"
#define ALL_FILE_PATH	"a:_ALL_PATH"
#define PAST_WEEK_FILE	"a:_PAST_WEEK"
#define TODAY_FILE		"a:_TODAY"
#define FAVOURITE_FILE	"a:__FAVOURITES_V72.bin"

/* Memory buffer declaration */
//struct Memory_Buffer
//{
//    uint_8	 buffer_queue[QUEUE_SIZE]; 
//    uint_8   Ready_For_SD;
//    uint_16	 write_ptr;
//    uint_16  Queue_size;
//};
//
//
//struct sdCardBuffer
//{
//	uint_8 currentWriteBuff;
//	uint_8 currentReadBuff;
//	struct Memory_Buffer BlockDataBuff[QUEUE_NOS];
//};


/* Settings Data file Declaration */
//#pragma pack(push, 1) // exact fit - no padding
#define MAX_LSM_SAMPLE_COUNT  (10)

typedef struct Gps_Cordinates
{
	char Latitude[16];
	char Longitude[16];

}Gps_Cordinates_Data;

typedef struct TimeStamp
{
	char Time[16];
	char Date[16];

}TimeStamp;

typedef struct LSM_Data
{
	int_16 AX_data;
	int_16 AY_data;
	int_16 AZ_data;
	int_16 MX_data;
	int_16 MY_data;
	int_16 MZ_data;
	uint_32 Tick_Count;

}LSM_Data;

typedef struct temperature
{	
	float Ambient_Temperature_Before_Test;	//4
	float Ambient_Temperature_After_Test;	//4
	float Object_Temperature_Before_Test;	//4
	float Object_Temperature_After_Test;	//4

}Temperature_Data;

#if 0
typedef struct Sd_Card_Data
{
	uint_32 Tick_Count;       //4
	char Device_ID[16];    //16
	TimeStamp Time_stamp;   //32
	Gps_Cordinates_Data Gps_data; //32
	float Battery_Voltage;   //4
	Temperature_Data Temperature;   //16
	int_16 Slope_Angle;    //2
	char Aspect[4];     //4
	uint_8 Sample_Rate;    //1
	float Optical_Trigger_Threshold;//4
	uint_8 Test_Time_Setting;  //1
	float Force_Sensor_Calibration; //4 
	float Calibration_Time;   //4
	uint_8 Compass_Samples;   //1
	LSM_Data LSM_MeterData[10];     //16*10 
	char User_ID[10];    
	char Software_version[10];
	char Test_ID[16];
	uint_32 Device_memory_status;
	//Total bytes=258+23

} Sd_Card_TempData;

#else
typedef struct Sd_Card_Data
{
	char Device_ID[16];    //16
	char Aspect[4];     //4
	char User_ID[10];    
	char Software_version[10];
	char Test_ID[16];
	uint_8 Sample_Rate;    //1
	uint_8 Test_Time_Setting;  //1
	uint_8 Compass_Samples;   //1
	uint_8 dummy;   //1
	TimeStamp Time_stamp;   //32
	Gps_Cordinates_Data Gps_data; //32
	int_16 Slope_Angle;    //2
	int_16 dummy1;    //2
	LSM_Data LSM_MeterData[10];     //16*10 
	uint_32 Tick_Count;       //4
	float Battery_Voltage;   //4
	Temperature_Data Temperature;   //16
	float Optical_Trigger_Threshold;//4
	float Force_Sensor_Calibration; //4 
	float Calibration_Time;   //4
	uint_32 Device_memory_status;
	//Total bytes=258+23

} Sd_Card_TempData;
#endif


typedef struct File_Header
{
	Gps_Cordinates_Data Gps_data; //32
	TimeStamp Time_stamp;   // 32
	char File_Type_ID[6];  // 6  bytes
	char Device_ID[14];      // 14
	char Software_version[10]; // 10
	char File_Process_Type;  // 1
	char Time_Zone[8];   // 8
	char Local_Time[16]; // 16 
	char Time_Delay;   // 1
	char Tip_Condition[2];  // 2
	char Aspect[4];     //4
	char Three_Test_Average;	//1
	char Warped_Average_Profile;	//1
	char Three_Test_First[8];	//8
	char Three_Test_Second[8];	//8
	char Three_Test_Third[8];	//8
	char compass_sample_count; //1
	char Favorite;			// 1
	uint16_t Date_Number;    // 2
	int_16 Slope_Angle;    //2
	int_16 dummy1;		   //2
	uint32_t Test_ID;    // 4
	float Sample_Rate;    //4
	float Battery_Voltage;    //4
	float Device_Pts[4];	  // 16
	float Irdms_Device_Pts[4];	  // 16
	Temperature_Data Temperature;   //16
} File_Header_information;


//#pragma pack(pop) //back to whatever the previous packing mode was

extern MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partition_handle,fd_ptr1,fd_ptr3,fd_ptr;
//This Globa;l variable is updated when reading data from sdcard
extern Sd_Card_TempData SD_Card_Read_Data,Data_Write_file;
extern File_Header_information Data_header;//SD_Data_header;
extern uint_16 date_word, time_word;
extern volatile unsigned char Test_Err;
//extern uint_32 sd_card_task_id;
extern char Raw_Data_file_name[];
extern uint_16 Data_File_Count;
extern uint_32 	 SD_Write_Cnt;
extern volatile MQX_FILE_PTR 	fd_Adc;
extern struct sdCardBuffer SDCardBuffer;
extern void sdcard_task(uint_32 temp);
extern char BUSY;
extern volatile uint_8 Sd_card_error_detected;
//extern uint_32 Adc_Conversion_Fail_Ctr,Buffer_Left_To_Write;
extern LWGPIO_STRUCT SD_EN;
extern void SD_Buffer_Init();
extern void Write_Data_file_information_To_Sdcard(void);
extern void Write_Favourite_file_information_To_Sdcard(void);
extern void Read_Favourite_file_information_From_Sdcard(void);
extern void Read_Test_count_Ble_Folder_name(void);
extern void Update_BLE_file_manifest(void);
extern void Read_Data_file_information_From_Sdcard(void);
extern int Read_RTC_information_From_Sdcard(void);
extern void Write_RTC_file_information_To_Sdcard(void);
extern uint_8 Sd_Card_deinit(void);
extern void Sd_Card_Folder_List(void);
extern void Delete_File(uint_8 folder_count, uint_8 file_count);
extern void Delete_All_Sd_Card_Data(void);
extern void format_sd_card(void);
extern void List_File_Attributes(void);
extern void Sd_Card_File_List(uint_8);
extern uint_8 Create_subdir(char date[12]);
extern uint_8 Sd_card_folder_count;
extern uint_16 Sd_card_file_count;
extern void Create_ble_manifest_after_startup_and_delete(void);
extern void Sort_Sd_Card_Folder_List_in_Descending_Order(void);
extern uint_32 date_time_to_seconds(DATE_STRUCT_PTR);
extern void Uninstall_FS_for_MSD(void);
extern void Read_mag_calib_data_from_file();
extern void Write_SDcard_DFU();

#endif /* SD_CARD_H_ */

