/*
 * Data_Processessing_Functions.h
 *
 *  Created on: Mar 3, 2014
 *      Author: user
 */

#ifndef DATA_PROCESSESSING_FUNCTIONS_H_

#define DATA_PROCESSESSING_FUNCTIONS_H_

#define BUFFER_BLOCK_SIZE	       (600)
#define DEPTH_RAW_COUNT            (1500)
#define GRAPH_RAW_PIXEL_COUNT      (300)
#define GRAPH_COLUMN_PIXEL_COUNT   (200)
#define RAW_AVERAGE_COUNT          (DEPTH_RAW_COUNT/GRAPH_RAW_PIXEL_COUNT)



#define ENABLE_PRINTF_SMOOTHING_PRESSURE 0
#define ENABLE_PRINTF_SMOOTH_QRD 0
#define ENABLE_PRINTF_IRDMS_MOVING_MIN 0
#define ENABLE_PRINTF_IRDMS_RUNNING_AVG 0
#define PRESSURE_DEPTH_CALCULATION_LOG_ENABLE 0
#define ENABLE_PRINTF_LOG 0
#define ENABLE_PRINTF_CALIBEREATE_PRESSURE_SENSOR 0
#define DISTANCE_CALCULATION_LOG 0
#define VIRDMS_MODE_PRITNF_FLAG 0

#define LOG_BASE_FIVE  1
//#define BUFFER_BLOCK_SIZE	1020
#define SETTINGS_MENU_ITEM 10

#define log5f(X) (logf(X)/logf(5))			//Logarithm base change rule
uint_8 Read_pressure_pixel_values(const char *Name_of_pressure_pixel_file);
uint_8 Write_pixel_files(const char *File_Name,uint_16 * buffer);
void Clear_Global_Buffers(void);
//void graph_to_bitmap(uint_8 *data);
//extern void Pixelated_Means(uint_8 *data);
//uint_8 Calculate_Pressure(uint_16 tempData, float *Vpressure_Difference, float *pressure);
//uint_8 Calculate_Distance_from_raw_Data(uint_8 * tempBuffer, float *depth);
uint_8 Process_Data();
uint_8 Take_SD_Raw();
uint_8 Read_Header_Data_From_File(MQX_FILE_PTR fd_ptr);
uint_8 Link_Snow_Profile_With_Test(int_16 SlopeAngle, char *Aspect);
uint_8 Link_Favourite_With_Test(void);
void Cancel_New_Test(void);
void Discard_New_Test_Data(void);
void delete_snow_profle_selected(void);
extern void Blocky_representation(uint_8 *data);
uint_8 Write_raw_files(MQX_FILE_PTR fd_ptr,volatile uint_8 *temp_buff);
uint_8 Write_pixelated_files(const char *File_Name,float *buffer);
//extern uint_16 pressurePixels_buff[];
//extern uint_16 pressureBlockyPixels_buff[];
extern uint16_t Pixels_buffer[]; 

extern uint32_t process_index;

extern float Kforce_5; //= 0.17;
extern float Kforce_3; //= 0.3 * (3 / 5.0);
extern uint8_t processing_status;

typedef struct Data_File_Names
{
	char Processed_Data_file_name[32];
	char Name_of_pressure_pixel_file[32];
	char Name_of_RawDataFile[32] ;
	char test_folder_name[32];
	char test_file_name[32];

}File_Directory;
extern File_Directory Test_file_directory;

#endif /* DATA_PROCESSESSING_FUNCTIONS_H_ */

/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
