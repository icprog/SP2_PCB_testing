/******************************************************************************
 *
 *   Filename: $Data_Processessing_Functions.c
 *   Created on: $Date: Oct 17, 2013  (4:10:52 PM)
 *   Revision: $1
 *   Author: $GadgEon
 *
 *   Description: $This file contains implementation of data processing algorithms
 *   Copyright (C) 2014 GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include <fio.h>
#include <timer.h>
#include <math.h>

#include <mfs.h>
#include <sdcard.h>
#include <spi.h>
#include <part_mgr.h>
#include <stdlib.h>
#include "UI_Display.h"
#include "config.h"
#include "Sd_Card.h"
#include "Sensor_Configuration.h"
#include "common_headers.h"
#include "Data_Processessing_Functions.h"
#include "algorithm.h"

File_Directory Test_file_directory;


uint32_t process_index=0;
float  SA = SA_VALUE;
float Kforce_5 = 0.17;
float Kforce_3 = 0.17 * (3 / 5.0);
float  Vpressure0 = V_PRESSURE_0;
uint8_t processing_status =0;

//uint16_t pressureBlockyPixels_buff[GRAPH_RAW_PIXEL_COUNT];
//uint16_t pressurePixels_buff[GRAPH_RAW_PIXEL_COUNT];

uint16_t Pixels_buffer[(GRAPH_RAW_PIXEL_COUNT*5)+1]; //EndDEpth will be stored in the  GRAPH_RAW_PIXEL_COUNT*5 location

static uint_16 Processing_Data_Write();
static void Update_File_Header(void);

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Pressure
 * Brief:       The goal of this step is to translate the voltage from the pressure sensor into an actual measurement of pressure.
 * Parameter:   Raw data, pointer to hold pressure difference and a pointer to hold final pressure data
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Calculate_Pressure_from_raw(uint_16 tempData, float *Vpressure_Difference, float *pressure)
{

	*Vpressure_Difference = (float) tempData * (RAW_DATA_TO_VOLTAGE_MULTIPLIER);
	K_factor = (1000 * Kforce_3 * SA);
	*pressure = *Vpressure_Difference / K_factor;

	return 0;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Calculate_Distance_from_raw_Data
 * Brief:       Find depth from raw voltages
 * Parameter:   raw voltage and pointer to hold depth.
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Calculate_Distance_from_raw_Data(uint_8 * tempBuffer, float *depth)
{
	uint_16 tempData;

	tempData = (tempBuffer[0] * 0x0100) + tempBuffer[1];

	*depth = BIRDMS
			+ powf(((float) tempData * (RAW_DATA_TO_VOLTAGE_MULTIPLIER) - DIRDMS) / AIRDMS,
					(1 / CIRDMS));

	return 0;
}



static uint_16 Processing_Data_Write(void)
{
	MQX_FILE_PTR file_name_handle;
	//uint_32 num_of_bytes = process_index * 4;
//	uint_32 i=0;

	/* Writing Processed file to SD card..*/ 
	Data_header.File_Process_Type = 'P';
	file_name_handle= fopen(Test_file_directory.Processed_Data_file_name,"w");
	if(file_name_handle != NULL)
	{

		if (write(file_name_handle, &Data_header, sizeof(Data_header))
				!= sizeof(Data_header))
		{
			printf("\n Error Writing Global Data Buffer..!");
			fclose(file_name_handle);
			return 1;
		}


		if (write(file_name_handle,(void *)AlgorithmData->compressedPressure, (DEPTH_RAW_COUNT*sizeof(uint8_t))) != (DEPTH_RAW_COUNT*sizeof(uint8_t)))
		{
			printf("Writing data to file  Failed.\n");
			fclose(file_name_handle);
			return 1;
		}


	}
	else
	{		
		printf("\nError in opening File %s",Test_file_directory.Processed_Data_file_name);
	}
	fclose(file_name_handle);

	return 0;
}


uint8_t Take_SD_Raw(void)
{
	MQX_FILE_PTR fd_ptr_raw_data_9;
	uint32_t loop=0;	
	uint8_t  d_buff[6]={0};

	if(ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\")==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO a:\\ in function Take_SD_Raw");
	}
	else
	{
		printf("\nCHANGE DIRECTORY TO a:\\ FAILED in function Take_SD_Raw");
	}
	//	fd_ptr_raw_data_9 = fopen("RAW00019.hex","r");  
	fd_ptr_raw_data_9 = fopen("a:RA.HEX","r");      

	if (fd_ptr_raw_data_9 == NULL)
	{
		printf("File error while opening file in read mode...!\n");
		printf("Reading RAW00019NOPRANJAL.HEX file from SD CARD Failed.\n");
		return 1;
	}

	memset(( uint16_t *)AlgorithmData->force_buff,0x00,MAX_PROCESSING_DATA_COUNT);
	memset(( uint16_t *)AlgorithmData->distanceBuff,0x00,MAX_PROCESSING_DATA_COUNT);
	memset(( uint16_t *)AlgorithmData->qrdBuff,0x00,MAX_PROCESSING_DATA_COUNT);


	while(fread(d_buff,6,1,fd_ptr_raw_data_9)> 0)
	{

		AlgorithmData->force_buff[loop]   = ((d_buff[0] << 8) + d_buff[1]);
		AlgorithmData->distanceBuff[loop] = ((d_buff[2] << 8) + d_buff[3]);
		AlgorithmData->qrdBuff[loop++] = ((d_buff[4] << 8) + d_buff[5]);
	}   

	//printf("\r\n%d",loop);
	Acq_Data_Count = loop;
	fclose(fd_ptr_raw_data_9);
	return 0;


}
uint8_t Take_SD_Raw__()
{

	MQX_FILE_PTR fd_ptr_raw_data_9;
	Sd_Card_TempData Read_SD_Card_TempData;

	if(ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\")==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO a:\\ in function Take_SD_Raw");
	}
	else
	{
		printf("\nCHANGE DIRECTORY TO a:\\ FAILED in function Take_SD_Raw");
	}
	fd_ptr_raw_data_9 = fopen("a:R033527","r");    

	if (fd_ptr_raw_data_9 == NULL)
	{
		printf("\nFile error while opening file in read mode in function Take_SD_Raw...!\n");
		printf("Reading R033527 file from SD CARD Failed in function Take_SD_Raw.\n");
		return 1;
	}
	// reading 16 byte dev id
	memset((Sd_Card_TempData *)&Read_SD_Card_TempData,0x00,sizeof(Sd_Card_TempData));
	fread(&Read_SD_Card_TempData,1,sizeof(Sd_Card_TempData),fd_ptr_raw_data_9);

	memset(AlgorithmData,0x00,sizeof(Algorithm_Data));

	if(fread (( uint16_t *)AlgorithmData->force_buff,2,Read_SD_Card_TempData.Tick_Count,fd_ptr_raw_data_9)!=Read_SD_Card_TempData.Tick_Count)
	{
		printf("File read overflow\n");
		fclose(fd_ptr_raw_data_9);
		return 1;
	}

	if(fread (( uint16_t *)AlgorithmData->distanceBuff,2,Read_SD_Card_TempData.Tick_Count,fd_ptr_raw_data_9)!=Read_SD_Card_TempData.Tick_Count)
	{
		printf("File read overflow\n");
		fclose(fd_ptr_raw_data_9);
		return 1;
	}
	if(fread (( uint16_t *)AlgorithmData->qrdBuff,2,Read_SD_Card_TempData.Tick_Count,fd_ptr_raw_data_9)!=Read_SD_Card_TempData.Tick_Count)
	{
		printf("File read overflow\n");
		fclose(fd_ptr_raw_data_9);
		return 1;
	}


	Acq_Data_Count = Read_SD_Card_TempData.Tick_Count;
	fclose(fd_ptr_raw_data_9);
	return 0;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Write_test_files_to_sd_card
 * Brief:       This function will store raw data, processed data and pixel data to sd card
 * Parameter:   None
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint8_t Write_test_files_to_sd_card(void)
{
	MQX_FILE_PTR file_name_handle;
	Data_header.File_Process_Type = 'R';
	file_name_handle= fopen(Test_file_directory.Name_of_RawDataFile,"w");
	if(file_name_handle != NULL)
	{

		if (write(file_name_handle, &Data_header, sizeof(Data_header))
				!= sizeof(Data_header))
		{
			printf("\n Error Writing Raw data..!");
			fclose(file_name_handle);
			return 1;
		}

		//		printf("\nSize of Header written :%d", sizeof(Data_Write_file));



		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->force_buff)!=0)
		{
			printf("\nError in while writing Force_buff");	
			fclose(file_name_handle);
			return 1;
		}

		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->distanceBuff)!=0)
		{
			printf("\nError in while writing Distance_buff");
			fclose(file_name_handle);
			return 1;
		}

		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->qrdBuff)!=0)
		{
			printf("\nError in while writing Qrd_buff");

			fclose(file_name_handle);
			return 1;
		}	

		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->AX)!=0)
		{
			printf("\nError in while writing AX");

			fclose(file_name_handle);
			return 1;
		}	
		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->AY)!=0)
		{
			printf("\nError in while writing AY");

			fclose(file_name_handle);
			return 1;
		}	
		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->AZ)!=0)
		{
			printf("\nError in while writing AZ");

			fclose(file_name_handle);
			return 1;
		}	

		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->MX)!=0)
		{
			printf("\nError in while writing MX");

			fclose(file_name_handle);
			return 1;
		}	
		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->MY)!=0)
		{
			printf("\nError in while writing MY");

			fclose(file_name_handle);
			return 1;
		}	
		if(Write_raw_files(file_name_handle,(uint_8 *)AlgorithmData->MZ)!=0)
		{
			printf("\nError in while writing MZ");

			fclose(file_name_handle);
			return 1;
		}	

	}
	else
	{		
		printf("\nError in opening File %s",Test_file_directory.Processed_Data_file_name);
		return 1;
	}
	fclose(file_name_handle);
	
	if(processing_status==0) //Write processed file only when test is success  
	{
		Processing_Data_Write(); //Write compressed pressure file
		/* Create Pressure Pixel File*/
		// File_Process_Type;  // 1	
		Data_header.File_Process_Type = 'X';
		if (Write_pixel_files(Test_file_directory.Name_of_pressure_pixel_file,Pixels_buffer) != 0)
		{
			printf("Writing pressure pixel values to sd card failed\n");
			return 1;
		}
	};
	return 0;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Process_Data
 * Brief:       This function will convert raw data to processed data
 * Parameter:   name of raw data file,name of processed data file and name pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/
void Writefiles_after_processing()
{
	Create_subdir(Test_file_directory.test_folder_name);
	Update_BLE_file_manifest();
	Update_File_Header();
	Write_test_files_to_sd_card();
}
/*-----------------------------------------------------------------------------* 
 * Function:    Read_Header_Data_From_File
 * Brief:       This function will read header data of processed data file
 * Parameter:   file pointer indicating opened file
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Read_Header_Data_From_File(MQX_FILE_PTR fd_ptr)
{

	if (read(fd_ptr, (char *) &Data_header, sizeof(Data_header))
			!= sizeof(Data_header))
	{
		return 1;
	}
	printf("\nSize of Header:%d", sizeof(Data_header));

	return 0;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Link_Favourite_With_Test
 * Brief:       This function will store favourite to processed data file and pressure pixel file
 * Parameter:   SlopeAngle and Aspect
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Link_Favourite_With_Test(void)
{
	MQX_FILE_PTR fd_ptr;

	char Name_of_Pixelated_means_file[32];
	char Name_of_compressed_pressure_file[32];


	sprintf(Name_of_Pixelated_means_file, "a:\\%s\\X%s", Snow_Profile_Directory[Folder_name_index], Snow_Profile_File_Name[File_name_index]+1);
	sprintf(Name_of_compressed_pressure_file, "a:\\%s\\P%s", Snow_Profile_Directory[Folder_name_index], Snow_Profile_File_Name[File_name_index]+1);

	printf("\nName_of_Pixelated_means_file:%s", Name_of_Pixelated_means_file);
	printf("\nName of compressed pressure file:%s", Name_of_compressed_pressure_file);


	fd_ptr = fopen(Name_of_Pixelated_means_file, "r");
	if (fd_ptr == NULL)
	{
		printf("File error while opening file %s in read/ mode...!\n", Name_of_Pixelated_means_file);
		return 1;

	}

	//Read Header information from file

	if (Read_Header_Data_From_File(fd_ptr) == 1)
	{
		printf("File error while reading Header data from file %s...!\n", Name_of_Pixelated_means_file);
		fclose(fd_ptr);
		return 1;

	}

	if (read(fd_ptr, Pixels_buffer, ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t))) != ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t)))
	{
		printf("reading data from file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}
	fclose(fd_ptr);
	
	
	fd_ptr = fopen(Name_of_compressed_pressure_file, "r");
	
	if (fd_ptr == NULL)
	{
		printf("File error while opening file %s in read/ mode...!\n", Name_of_compressed_pressure_file);
		return 1;

	}

	if (Read_Header_Data_From_File(fd_ptr) == 1)
	{
		printf("File error while reading Header data from file %s...!\n", Name_of_compressed_pressure_file);
		fclose(fd_ptr);
		return 1;

	}

	if (read(fd_ptr,(void *)AlgorithmData->compressedPressure, (DEPTH_RAW_COUNT*sizeof(uint8_t))) != (DEPTH_RAW_COUNT*sizeof(uint8_t)))
	{
		printf("reading data from file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}

	fclose(fd_ptr);
	

	printf("\n Old Favourite status =%d", Data_header.Favorite);
	
	if(Data_header.Favorite == NO)
	{
		Data_header.Favorite = YES;
	}
	else
	{
		Data_header.Favorite = NO;
	}
	
	
	printf("\n New Favourite status =%d", Data_header.Favorite);

	if (Write_pixel_files(Name_of_Pixelated_means_file,Pixels_buffer) != 0)
	{
		printf("Writing pressure pixel values to sd card failed\n");
		return 1;
	}
	
	memcpy(Test_file_directory.Processed_Data_file_name,Name_of_compressed_pressure_file,32);

	if (Processing_Data_Write() != 0)
	{
		printf("Writing pressure pixel values to sd card failed\n");
		return 1;
	}
	
	return 0;

}

/*-----------------------------------------------------------------------------* 
 * Function:    Link_Snow_Profile_With_Test
 * Brief:       This function will store slope angle and aspect to processed data file and pressure pixel file
 * Parameter:   SlopeAngle and Aspect
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Link_Snow_Profile_With_Test(int_16 SlopeAngle, char *Aspect)
{
	MQX_FILE_PTR fd_ptr;

	char Name_of_Pixelated_means_file[32];
	char Name_of_compressed_pressure_file[32];


	sprintf(Name_of_Pixelated_means_file, "a:\\%s\\X%s", Snow_Profile_Directory[Folder_name_index], Snow_Profile_File_Name[File_name_index]+1);
	sprintf(Name_of_compressed_pressure_file, "a:\\%s\\P%s", Snow_Profile_Directory[Folder_name_index], Snow_Profile_File_Name[File_name_index]+1);

	printf("\nName_of_Pixelated_means_file:%s", Name_of_Pixelated_means_file);
	printf("\nBlocky Pixel File  Name:%s", Name_of_compressed_pressure_file);


	fd_ptr = fopen(Name_of_Pixelated_means_file, "r");
	if (fd_ptr == NULL)
	{
		printf("File error while opening file %s in read/ mode...!\n", Name_of_Pixelated_means_file);
		return 1;

	}

	//Read Header information from file

	if (Read_Header_Data_From_File(fd_ptr) == 1)
	{
		printf("File error while reading Header data from file %s...!\n", Name_of_Pixelated_means_file);
		fclose(fd_ptr);
		return 1;

	}

	if (read(fd_ptr, Pixels_buffer, ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t))) != ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t)))
	{
		printf("reading data from file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}

	fclose(fd_ptr);
	
	

		fd_ptr = fopen(Name_of_compressed_pressure_file, "r");
		if (fd_ptr == NULL)
		{
			printf("File error while opening file %s in read/ mode...!\n", Name_of_compressed_pressure_file);
			return 1;
	
		}

		if (Read_Header_Data_From_File(fd_ptr) == 1)
		{
			printf("File error while reading Header data from file %s...!\n", Name_of_compressed_pressure_file);
			fclose(fd_ptr);
			return 1;
	
		}
		
		if (read(fd_ptr,(void *)AlgorithmData->compressedPressure, (DEPTH_RAW_COUNT*sizeof(uint8_t))) != (DEPTH_RAW_COUNT*sizeof(uint8_t)))
		{
			printf("reading data from file  Failed.\n");
			fclose(fd_ptr);
			return 1;
		}
		
	fclose(fd_ptr);


	printf("\n Old Slope values SA=%d,aspect %s", Data_header.Slope_Angle,
			Data_header.Aspect);
	Data_header.Slope_Angle = SlopeAngle;
	strcpy(Data_header.Aspect, Aspect);

	printf("\n New Slope values SA=%d,aspect %s", Data_header.Slope_Angle,
			Data_header.Aspect);


	if (Write_pixel_files(Name_of_Pixelated_means_file,Pixels_buffer) != 0)
	{
		printf("Writing pressure pixel values to sd card failed\n");
		return 1;
	}

	memcpy(Test_file_directory.Processed_Data_file_name,Name_of_compressed_pressure_file,32);
	
	if (Processing_Data_Write() != 0)
	{
		printf("Writing pressure pixel values to sd card failed\n");
		return 1;
	}
	
	//	if (Write_pixel_files(Name_of_blocky_pixel_file,pressureBlockyPixels_buff) != 0)
	//	{
	//		printf("Writing pressure pixel values to sd card failed\n");
	//		return 1;
	//	}

	return 0;

}


/*-----------------------------------------------------------------------------* 
 * Function:    Discard_New_Test_Data
 * Brief:       This function will remove latest raw data file ,processed file and pressure pixel file
 * Parameter:   None
 * Return:      None 
 -----------------------------------------------------------------------------*/
//void Discard_New_Test_Data(void)
//{
//	char temp_Raw_Data_file_name[20];
//	char temp_Processed_Data_file_name[24];
//	char temp_pixel_data_file_name[24];
//	_mqx_int error_code;
//
//	Data_File_Count--;
//	sprintf(temp_Raw_Data_file_name, "a:raw%05d.hex", Data_File_Count);
//	sprintf(temp_Processed_Data_file_name, "a:Processed%05d.hex", Data_File_Count);
//	sprintf(temp_pixel_data_file_name, "a:Pixel%05d.hex", Data_File_Count);
//
//	error_code = _io_ioctl(filesystem_handle, IO_IOCTL_DELETE_FILE,
//			(uint_32_ptr) temp_Raw_Data_file_name);
//	if (error_code != 0)
//	{
//		printf("\n Error While deleting Raw data file");
//	}
//	error_code = _io_ioctl(filesystem_handle, IO_IOCTL_DELETE_FILE,
//			(uint_32_ptr) temp_Processed_Data_file_name);
//	if (error_code != 0)
//	{
//		printf("\n Error While deleting Processed data file");
//	}
//	error_code = _io_ioctl(filesystem_handle, IO_IOCTL_DELETE_FILE,
//			(uint_32_ptr) temp_pixel_data_file_name);
//	if (error_code != 0)
//	{
//		printf("\n Error While deleting Pixel data file");
//	}
//
//	return;
//}
/*-----------------------------------------------------------------------------* 
 * Function:    Write_row_files
 * Brief:       This function will create pressure pixel values
 * Parameter:   Name of pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/


uint_8 Write_raw_files(MQX_FILE_PTR fd_ptr,volatile uint_8 *temp_buff)
{

	if (fd_ptr == NULL)
	{
		printf("Writing a file from SD CARD Failed.\n");
		return 1;
	}

	/* Writing Global row bufffer to file*/ 
	uint_32 num_of_bytes = Acq_Data_Count * 2;
	uint_32 i=0;
	while(num_of_bytes > 0)
	{
		if(num_of_bytes >512)
		{
			num_of_bytes-= write(fd_ptr,(void *)(temp_buff +(i*512)),512);		
		}
		else
		{
			num_of_bytes-= write(fd_ptr,(void *)(temp_buff +(i*512)),num_of_bytes);
		}
		i++;
	}

	return 0;

}
/*-----------------------------------------------------------------------------* 
 * Function:    Write_pressure_pixel_values
 * Brief:       This function will create pressure pixel values
 * Parameter:   Name of pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/


uint_8 Write_pixel_files(const char *File_Name,uint_16 *buffer)
{
	MQX_FILE_PTR fd_ptr;
	fd_ptr = fopen(File_Name, "w");

	if (fd_ptr == NULL)
	{

		printf("File error while opening %s file in Write mode...!\n", File_Name);
		printf("Writing a file from SD CARD Failed.\n");
		return 1;

	}

	if (write(fd_ptr, &Data_header, sizeof(Data_header)) != sizeof(Data_header))
	{
		printf("Writing data to file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}
	if (write(fd_ptr, buffer, ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint_16))) != ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint_16)))
	{
		printf("Writing data to file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}
	fclose(fd_ptr);
	return 0;

}

/*-----------------------------------------------------------------------------* 
 * Function:    Read_pressure_pixelated_means_values
 * Brief:       This function will read pressure pixel values
 * Parameter:   Name of pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/
uint_8 Read_pressure_pixelated_means_values(const char *Name_of_pressure_pixel_file)
{
	MQX_FILE_PTR fd_ptr;
	//	dispaly_Waiting_for_file();
	fd_ptr = fopen(Name_of_pressure_pixel_file, "r");
	if (fd_ptr == NULL)
	{
		printf("File error while opening file %s in read/ mode...!\n", Name_of_pressure_pixel_file);
		return 1;

	}
	//Clear_Global_Buffers();
	//Read Header information from file

	if (Read_Header_Data_From_File(fd_ptr) == 1)
	{
		printf("File error while reading Header data from file %s...!\n",
				Name_of_pressure_pixel_file);
		fclose(fd_ptr);
		return 1;

	}

	if (read(fd_ptr, Pixels_buffer, ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t))) != ((GRAPH_RAW_PIXEL_COUNT*5+1)*sizeof(uint16_t)))
	{
		printf("reading data from file  Failed.\n");
		fclose(fd_ptr);
		return 1;
	}


	//	if(Settings_Data.default_gragh_view == DETAILED_DATA)
	//	{
	//
	//		if (read(fd_ptr, pressurePixels_buff, (GRAPH_RAW_PIXEL_COUNT*sizeof(uint16_t))) != (GRAPH_RAW_PIXEL_COUNT*sizeof(uint16_t)))
	//		{
	//			printf("reading data from file  Failed.\n");
	//			fclose(fd_ptr);
	//			return 1;
	//		}
	//	}
	//	else if(Settings_Data.default_gragh_view == SMOOTH_DATA)
	//	{
	//		if (read(fd_ptr, pressureBlockyPixels_buff, (GRAPH_RAW_PIXEL_COUNT*sizeof(uint16_t))) != (GRAPH_RAW_PIXEL_COUNT*sizeof(uint16_t)))
	//		{
	//			printf("reading data from file  Failed.\n");
	//			fclose(fd_ptr);
	//			return 1;
	//		}
	//	}


	fclose(fd_ptr);
	printf("\nReading Completed");   
	return 0;
}

/*-----------------------------------------------------------------------------* 
 * Function:    Update_Flie_Header
 * Brief:       This function will Update the File variables with latest test result
 * Parameter:   Name of pressure pixel file
 * Return:      status 
 -----------------------------------------------------------------------------*/
static void Update_File_Header(void)
{

	// File_Type_ID	
	strncpy(Data_header.File_Type_ID,"P-001",6);
	//sprintf(Data_header.File_Type_ID,"P-%3d",test_no);

	// Device_id	
	strncpy(Data_header.Device_ID,Device_id,14);

	// Software_version
	strncpy(Data_header.Software_version,Software_ver,10);	

	// File_Process_Type; 
	Data_header.Test_ID = Test_Count_Flash.Total_Test_Ct;   

	// Time_Zone.

	if(Settings_Data.Dst_status == DST_ON)
	{
		strncpy(Data_header.Time_Zone,TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_DST_Offset_String,8);
	}
	else
	{
		strncpy(Data_header.Time_Zone,TimeZoneTable[Settings_Data.Prev_Zone_List_selection].UTC_Offset_String,8); 
	}

	// Local_Time
	strncpy(Data_header.Local_Time,Data_header.Time_stamp.Time,16);

	// Date_Number
	Data_header.Date_Number = 100;

	// Time_Delay
	Data_header.Time_Delay = 0xAA;

	Data_header.Irdms_Device_Pts[0] = IRDMS_DEVICE_PTS[0];
	Data_header.Irdms_Device_Pts[1] = IRDMS_DEVICE_PTS[1];
	Data_header.Irdms_Device_Pts[2] = IRDMS_DEVICE_PTS[2];
	Data_header.Irdms_Device_Pts[3] = IRDMS_DEVICE_PTS[3];
	
	Data_header.Device_Pts[0] = DEVICE_PTS[0];
	Data_header.Device_Pts[1] = DEVICE_PTS[1];
	Data_header.Device_Pts[2] = DEVICE_PTS[2];
	Data_header.Device_Pts[3] = DEVICE_PTS[3];
	
	Data_header.Favorite = 0;
	
	//G_Force_Sensor_Cal_
//	Data_header.G_Force_Sensor_Cal_0N = 2.123;
//	Data_header.G_Force_Sensor_Cal_25N = 2.123;
//	Data_header.G_Force_Sensor_Cal_3N = 2.123;
//	Data_header.G_Force_Sensor_Cal_9N = 2.123;
//	Data_header.G_Force_Sensor_Cal_18N = 2.123;
//
//	Data_header.NG_Force_Sensor_Cal_0N = 2.123;
//	Data_header.NG_Force_Sensor_Cal_25N = 2.123;
//	Data_header.NG_Force_Sensor_Cal_3N = 2.123;
//	Data_header.NG_Force_Sensor_Cal_9N = 2.123;
//	Data_header.NG_Force_Sensor_Cal_18N = 2.123;

	// Tip_Condition
	strncpy(Data_header.Tip_Condition,"G",2); 

	// Slope n Aspect
	Data_header.Slope_Angle = 0;			
	strncpy(Data_header.Aspect,"NA",4);

	// Adding dummy variable for padding
	Data_header.dummy1 = 0;

	// Sample_Rate
	Data_header.Sample_Rate = (Settings_Data.Sampling_Rate);

	// Three_Test_Average
	Data_header.Three_Test_Average = 0;

	//Warped_Average_Profile
	Data_header.Warped_Average_Profile = 0;

	// Three_Test_First
	strncpy(Data_header.Three_Test_First,"00001234",8);
	strncpy(Data_header.Three_Test_Second,"00002345",8);
	strncpy(Data_header.Three_Test_Third,"00003456",8);

	// compass_sample_count	
	Data_header.compass_sample_count =  LSM_SAMPLE_COUNT % 255;

	printf("\nSize of Data_header%d\n",sizeof(Data_header));

	//just copying to read structure 
	//	memcpy(&SD_Data_header, &Data_header, sizeof(Data_header));

}


/*-----------------------------------------------------------------------------
 **************************  END   ***************************************
 -----------------------------------------------------------------------------*/
