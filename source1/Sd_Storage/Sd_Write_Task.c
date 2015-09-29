/*
 * Sd_Write_Task.c
 *
 *  Created on: Jan 8, 2014
 *      Author: GADGEON-03
 */

#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include <fio.h>
#include <timer.h>
#include "UI_Timer.h"
#include <mfs.h>
#include <sdcard.h>
#include <spi.h>
#include <part_mgr.h>
#include <stdlib.h>
#include "UI_Display.h"
#include "Sd_Card.h"
#include "UI_task.h"
#include "Hw_Timer.h"
#include "data_acquisition.h"
#include "Data_Processessing_Functions.h"
#include "config.h"
#include <stdlib.h> 
#include "common_headers.h"

#define  SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL

MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partition_handle, fd_ptr1, fd_ptr3, fd_ptr, fd_ptr_test;
//uint_32 Buffer_Left_To_Write=0;
volatile uint_8 Buffer_Overflow_Flag = 0, Test_Err = 0;
volatile uint_8 Sd_card_error_detected = 0;
volatile MQX_FILE_PTR fd_Adc;
char filesystem_name[] = "a:", filesystem_name_copy[30];
char partman_name[] = "pm:";
char partition_name[] = "pm:1";
//char Raw_Data_file_name[20] = "a:raw00000.hex";
uint_16 Data_File_Count = 0;
uint_32 SD_Write_Cnt = 0;
uint_8 Sd_card_folder_count=0;
uint_16 Sd_card_file_count=0;
Sd_Card_TempData SD_Card_Read_Data, Data_Write_file;
uint_16 date_word = 0, time_word = 0;
File_Header_information Data_header;//SD_Data_header;
_mqx_int error_code;

#define USR_SDCARD_GPIO_DETECT              (GPIO_PORT_E | GPIO_PIN6)
#define USR_SDCARD_DETECT_MUX_GPIO          (LWGPIO_MUX_E6_GPIO)

#if defined USR_SDCARD_GPIO_DETECT
LWGPIO_STRUCT sd_detect;
#endif
#if defined BSP_SDCARD_GPIO_PROTECT
LWGPIO_STRUCT sd_protect;
#endif
#define SD_EN_PIN   (GPIO_PORT_E|GPIO_PIN7)
#define SD_EN_PIN_MUX  (LWGPIO_MUX_E7_GPIO)

//static void SD_enable_pin_init(void);
//static void SD_enable(void);
//static void SD_disable(void);
LWGPIO_STRUCT SD_EN;

static void flip_directory(void);
extern void SD_enable_pin_init(void);
extern void SD_disable();
//uint_32 string_to_int(char);

uint_8 Create_file_system_if_sd_card_detected(boolean inserted);
/*-----------------------------------------------------------------------------
 *  Function:     SD_enable_pin_init
 *  Brief:        This function initialise the GPIO SD_EN for SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void SD_enable_pin_init(void)
{

	if (!lwgpio_init(&SD_EN, SD_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}

	lwgpio_set_functionality(&SD_EN, SD_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&SD_EN, LWGPIO_VALUE_LOW);
}

/*-----------------------------------------------------------------------------
 *  Function:     SD_enable
 *  Brief:        This function enables SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void SD_enable(void)
{
	// Enabling SD Card enable pin
	lwgpio_set_value(&SD_EN, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     SD_enable
 *  Brief:        This function disables SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void SD_disable(void)
{
	lwgpio_set_value(&SD_EN, LWGPIO_VALUE_LOW);

	SIM_MemMapPtr   sim  = SIM_BASE_PTR;

	//	PORTE_PCR0 = PORT_PCR_MUX(1);
	//	PORTE_PCR1 = PORT_PCR_MUX(1);
	//	PORTE_PCR2 = PORT_PCR_MUX(1);
	//	PORTE_PCR3 = PORT_PCR_MUX(1);
	//	PORTE_PCR4 = PORT_PCR_MUX(1);
	//	PORTE_PCR5 = PORT_PCR_MUX(1);
	//	
	//	  //Change PINS TO INPUT
	//	  GPIOE_PDDR &= ~GPIO_PDDR_PDD(GPIO_PIN(0) + GPIO_PIN(1) + GPIO_PIN(2) + GPIO_PIN(3) + GPIO_PIN(4) + GPIO_PIN(5));	

	/* Enable clock gate to SDHC module */
	sim->SCGC3 &= ~SIM_SCGC3_ESDHC_MASK;

}



uint_8 Sd_Card_init(void) 
{

	if(Sensor_status.SDcard_status == INITIALISED)
	{
		printf("\nSD Card Already Initialised..!");
		return 1;
	}

	SD_enable_pin_init();
	SD_enable();

	/* Open low level communication device */
	com_handle = fopen(SDCARD_COM_CHANNEL, (void *) (SPI_FLAG_FULL_DUPLEX));

	if (NULL == com_handle) {
		printf("Error opening communication handle %s.\n",SDCARD_COM_CHANNEL);
		return 1;
	}

#if defined USR_SDCARD_GPIO_DETECT
	/* Init GPIO pins for other SD card signals */
	error_code = lwgpio_init(&sd_detect, USR_SDCARD_GPIO_DETECT,
			LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	if (!error_code) {
		printf("Initializing GPIO with sdcard detect pin failed.\n");
		return 1;
	}
	/*Set detect and protect pins as GPIO Function */
	lwgpio_set_functionality(&sd_detect, USR_SDCARD_DETECT_MUX_GPIO);
	lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
	/* Init GPIO pins for other SD card signals */
	error_code = lwgpio_init(&sd_protect, BSP_SDCARD_GPIO_PROTECT,
			LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	if (!error_code) {
		printf("Initializing GPIO with sdcard protect pin failed.\n");
		return 1;
	}
	/*Set detect and protect pins as GPIO Function */
	lwgpio_set_functionality(&sd_protect, BSP_SDCARD_PROTECT_MUX_GPIO);
	lwgpio_set_attribute(&sd_protect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

	/* Install SD card device */
	error_code = _io_sdcard_install("sdcard:", (pointer) &_bsp_sdcard0_init,
			com_handle);
	if (error_code != MQX_OK) {
		printf("Error installing SD card device (0x%x)\n", error_code);
		return 1;
	}

	// create file system
	if(Create_file_system_if_sd_card_detected(TRUE)==1)
	{
		return 1;
	}

	printf("\nSD card Init Success..!");
	Sensor_status.SDcard_status = INITIALISED;
	return 0;

}


uint_8 Create_file_system_if_sd_card_detected(boolean inserted)
{
	boolean readonly = FALSE, last = FALSE;

	_mqx_uint param;
#if defined USR_SDCARD_GPIO_DETECT
#ifdef BSP_MPC8308RDB
	/* Set function as GPIO to detect sdcard */
	lwgpio_set_functionality(&sd_detect,USR_SDCARD_DETECT_MUX_GPIO);
	lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif
	if(inserted==TRUE)
	{
		inserted = !lwgpio_get_value(&sd_detect);
		if(inserted==FALSE)
		{
			printf("Sd Card Not Inserted\n");
			return 1;
		}
	}

#endif

#if defined BSP_SDCARD_GPIO_PROTECT
	/* Get value of protect pin */
	readonly = lwgpio_get_value(&sd_protect);
#endif
#ifdef BSP_MPC8308RDB
	/* Set function as SD_CD which indicate that card is present in Present State Register */
	lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_SD_CD);
	lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

	if (inserted) 
	{
		Time_Delay_Sleep(200);
		/* Open the device which MFS will be installed on */
		sdcard_handle = fopen("sdcard:", 0);
		if (sdcard_handle == NULL) 
		{
			printf("Unable to open SD card device. Check Sd card adapter \n");
			return 1;
		}

		/* Set read only flag as needed */
		param = 0;
		if (readonly) 
		{
			param = IO_O_RDONLY;
		}
		if (IO_OK!= ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS,(char_ptr) &param)) 
		{
			printf("Setting device read only failed.\n");
			return 1;
		}

		/* Install partition manager over SD card driver */
		error_code = _io_part_mgr_install(sdcard_handle, partman_name,
				0);
		if (error_code != MFS_NO_ERROR) 
		{
			printf("Error installing partition manager: %s\n",
					MFS_Error_text((uint_32) error_code));
			return 1;
		}

		/* Open partition */
		partition_handle = fopen(partition_name, NULL);
		if (partition_handle != NULL) 
		{
			/* Validate partition */
			error_code = _io_ioctl(partition_handle, IO_IOCTL_VAL_PART,	NULL);
			if (error_code != MFS_NO_ERROR) 
			{
				printf("Error validating partition: %s\n",
						MFS_Error_text((uint_32) error_code));
				printf("Not installing MFS.\n");
				return 1;
			}

			/* Install MFS over partition */
			error_code = _io_mfs_install(partition_handle,filesystem_name, 0);
			if (error_code != MFS_NO_ERROR) 
			{
				printf("Error initializing MFS over partition: %s\n",
						MFS_Error_text((uint_32) error_code));
			}

		} 
		else 
		{

			// printf("Installing MFS over SD card driver...\n");

			/* Install MFS over SD card driver */
			error_code = _io_mfs_install(sdcard_handle, filesystem_name,(_file_size) 0);
			if (error_code != MFS_NO_ERROR) 
			{
				printf("Error initializing MFS: %s\n",
						MFS_Error_text((uint_32) error_code));
			}
		}

		/* Open file system */
		if (error_code == MFS_NO_ERROR) 
		{
			filesystem_handle = fopen(filesystem_name, NULL);
			error_code = ferror(filesystem_handle);
			if (error_code == MFS_NOT_A_DOS_DISK) 
			{
				printf("NOT A DOS DISK! You must format to continue.\n");
			}
			else if (error_code != MFS_NO_ERROR) 
			{
				printf("Error opening filesystem: %s\n",
						MFS_Error_text((uint_32) error_code));
				return 1;
			}

			printf("SD card is detected with proper file system. \n");
			/**  Naming SD Card....**/
			if (ioctl(filesystem_handle, IO_IOCTL_SET_VOLUME, (uint_32_ptr) "AVATECH") != MFS_NO_ERROR)
			{
				printf("Error While Naming SDcard.\n");
			}

			//printf ("SD card installed to %s\n", filesystem_name);
			if (readonly) 
			{
				printf("SD card is locked (read only).\n");
			}
		}

		return 0;
	} 
	else 
	{
		/* Close the filesystem */
		if ((filesystem_handle != NULL) && (MQX_OK != fclose(filesystem_handle))) 
		{
			printf("Error closing filesystem.\n");
		}
		filesystem_handle = NULL;

		/* Uninstall MFS  */
		error_code = _io_dev_uninstall(filesystem_name);
		if (error_code != MFS_NO_ERROR) 
		{
			printf("Error uninstalling filesystem.\n");
		}

		/* Close partition */
		if ((partition_handle != NULL)&& (MQX_OK != fclose(partition_handle)))
		{
			printf("Error closing partition.\n");
		}
		partition_handle = NULL;

		/* Uninstall partition manager  */
		error_code = _io_dev_uninstall(partman_name);
		if (error_code != MFS_NO_ERROR) 
		{
			printf("Error uninstalling partition manager.\n");
		}

		/* Close the SD card device */
		if ((sdcard_handle != NULL)	&& (MQX_OK != fclose(sdcard_handle))) 
		{
			printf("Unable to close SD card device.\n");
		}
		sdcard_handle = NULL;

		/* Uninstall SD card device */
		error_code = _io_dev_uninstall("sdcard:");
		if ( error_code != MQX_OK )
		{
			printf("Error code is : %d",error_code);
			_task_block();
		}

		if(MQX_OK != fclose(com_handle))
		{
			printf("Unable to close communication channel.\n");
		}		
		else
			printf("Communication channel closed successfully.\n");

		printf("SD card uninstalled.\n");
	}
	return 0;
}



/*-----------------------------------------------------------------------------
 *  Function:     SD_Buffer_Init
 *  Brief:        This function initialise the SD buffer nad its pointers
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void SD_Buffer_Init()
{
	unsigned char buffer_ctr = 0;
	//	Buffer_Left_To_Write=0;
	Test_Err = 0;

	//printf("Clearing SD Buffer...\n");
	/*************************************************************/
	//    SDCardBuffer.currentWriteBuff = 0;
	//    SDCardBuffer.currentReadBuff = 0;
	//    for (buffer_ctr = 0; buffer_ctr < QUEUE_NOS; buffer_ctr++)
	//    {
	//        SDCardBuffer.BlockDataBuff[buffer_ctr].Ready_For_SD = FALSE;
	//        SDCardBuffer.BlockDataBuff[buffer_ctr].write_ptr = 0;
	//        SDCardBuffer.BlockDataBuff[buffer_ctr].Queue_size = 0;
	//    }

	/*************************************************************/
}

/*-----------------------------------------------------------------------------
 *  Function:     sdcard_task
 *  Brief:        This task performs Initialise file system on SD card.
                  writes data to SD card from SD Buffer
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void sdcard_task(uint_32 )
{

}

void Write_RTC_file_information_To_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr;   
	uint_8 tempBuffer[2];
	char name[20]="\\";
	char root_dir_name[10]="a:\\";

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:__RTC72.bin", "w");
	if (fd_ptr == NULL)
	{
		printf("RTC file create error\n");
		return;
	}

	strcat(name, Test_file_directory.test_folder_name);
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		//printf("\nCHANGE DIRECTORY FAILED");
		printf("Error %d\n",error_code);
		//		fclose(fd_ptr);
		//		return;
	}
	tempBuffer[0]=0x01;
	write(fd_ptr, tempBuffer, 1);
	fclose(fd_ptr);
}

/*-----------------------------------------------------------------------------
 *  Function:     Read_Data_file_information_From_Sdcard
 *  Brief:        This function reads settings file from SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
int Read_RTC_information_From_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr; 
	uint_8 tempBuffer[2];   

	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:__RTC72.bin", "r");
	if (fd_ptr == NULL)
	{
		printf("No RTC file found, so considering RTC not updated\n");
		return 1;
	}
	/* Reads  settings file from SD card*/
	if (read(fd_ptr, tempBuffer, 1) == 1)
	{
		if(tempBuffer[0]==1)
		{
			printf("RTC is previously updated");
			fclose(fd_ptr);
			return 0;
		}
		else
		{
			printf("RTC is not updated previously");
			fclose(fd_ptr);
			return 1;
		}
	}
	else
	{
		printf("File is empty\n");
		fclose(fd_ptr);
		return 1;
	}

	/* Closing settings file from SD card*/

}


/*-----------------------------------------------------------------------------
 *  Function:     Read_Data_file_information_From_Sdcard
 *  Brief:        This function reads settings file from SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Read_Data_file_information_From_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr; 
	uint_8 tempBuffer[2];   

	/* Opening settings file from SD card*/
	fd_ptr = fopen(SETTINGS_FILE_NAME, "r");
	if (fd_ptr == NULL)
	{
		Data_File_Count = 0;
		Default_Settings_Data();
		Write_Data_file_information_To_Sdcard();
		return;
	}

	/* Reads  settings file from SD card*/
	if (read(fd_ptr, tempBuffer, 2) == 2)
	{
		Data_File_Count = (uint_16)((tempBuffer[0] * 0x0100) + tempBuffer[1]);

		if (read(fd_ptr, (char *) &Settings_Data, sizeof(Settings_Data)) != sizeof(Settings_Data))
		{
			Default_Settings_Data();
			Update_settings_Data();
			fclose(fd_ptr);
			return;
		}
	}
	else
	{
		Data_File_Count = 0;
		Default_Settings_Data();
	}
	Update_settings_Data();

	/* Closing settings file from SD card*/
	fclose(fd_ptr);
}


/*-----------------------------------------------------------------------------
 *  Function:     Write_Data_file_information_To_Sdcard
 *  Brief:        This function Writes settings file to SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Write_Data_file_information_To_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr;   
	uint_8 tempBuffer[2];
	char name[20]="\\";
	char root_dir_name[10]="a:\\";

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		//printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

	/* Opening settings file from SD card*/
	fd_ptr = fopen(SETTINGS_FILE_NAME, "w");
	if (fd_ptr == NULL)
	{
		Update_settings_Data();
		return;
	}

	strcat(name, Test_file_directory.test_folder_name);
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		//printf("\nCHANGE DIRECTORY FAILED");
		printf("Error %d\n",error_code);
		//		fclose(fd_ptr);
		//		return;
	}

	tempBuffer[0] = (uint_8)((Data_File_Count & 0xFF00) >> 8);
	tempBuffer[1] = (uint_8)(Data_File_Count & 0xFF);

	write(fd_ptr, tempBuffer, 2);
	write(fd_ptr, &Settings_Data, sizeof(Settings_Data));
	Update_settings_Data();
	fclose(fd_ptr);
}

/*-----------------------------------------------------------------------------
 *  Function:     Read_Data_file_information_From_Sdcard
 *  Brief:        This function reads settings file from SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Read_Favourite_file_information_From_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr;  

	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:__FAVOURITES_V77.bin", "r");
	if (fd_ptr == NULL)
	{
		Favourites_file_count=FavouriteEntry[0].Favourites_Dir_Count=0;
		//		printf("\nREADING FAVOURITE DIRECTORY FAILED\n");
		return;
	}

	if (read(fd_ptr, (char *) FavouriteEntry, sizeof( FavouriteTableEntry)*64) == sizeof( FavouriteTableEntry)*64)
	{
		Favourites_file_count=FavouriteEntry[0].Favourites_Dir_Count;
		printf("\nSize of favourite: %d", sizeof( FavouriteTableEntry)*64);
	}
	else
	{
		printf("\nNO FAVOURITE FILE INFORMATION AVAILABLE");
	}

	/* Closing settings file from SD card*/
	fclose(fd_ptr);
}

/*-----------------------------------------------------------------------------
 *  Function:     Write_Favourite_file_information_To_Sdcard
 *  Brief:        This function Writes favourite file to SD card.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Write_Favourite_file_information_To_Sdcard(void)
{
	MQX_FILE_PTR fd_ptr;
	char name[20]="\\";
	char root_dir_name[10]="a:\\";

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , root_dir_name);
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:__FAVOURITES_V77.bin", "w");
	if (fd_ptr == NULL)
	{
		printf("\nFAVOURITE_FILE NOT CREATED]\n");
		return;
	}

	write(fd_ptr, &FavouriteEntry, sizeof( FavouriteTableEntry)*64);
	printf("Size of written favourite: %d", sizeof( FavouriteTableEntry)*64);
	fclose(fd_ptr);

	strcat(name, Test_file_directory.test_folder_name);
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

}

/*-----------------------------------------------------------------------------
 *  Function:     Read_Test_count_Ble_Folder_name
 *  Brief:        This function reads Test file count and ble folder name upon initializing from SD card; Temporary, should be loaded from flash!!!
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Read_Test_count_Ble_Folder_name(void)
{
	MQX_FILE_PTR fd_ptr;
	char name[20]="\\";
	char root_dir_name[10]="a:\\";

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , root_dir_name);
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

	/* Opening settings file from SD card*/
	fd_ptr = fopen("a:_ALL_PATH", "r");
	if (fd_ptr == NULL)
	{
		//		printf("\nREADING __TEST_FILE_MANIFEST_V63 FAILED\n");
		return;
	}
	//	//Final Test file will be written to the end of the file. So in rebooting, this will get the final test count and reload it.
	//	fseek(fd_ptr, -36, IO_SEEK_END);
	//	if(read(fd_ptr, &Test_Count_Flash.Total_Test_Ct, 4) == 4)
	//	{
	//		printf("\nTest file count read from SD CARD %u", Test_Count_Flash.Total_Test_Ct);
	//	}
	fseek(fd_ptr, 7, IO_SEEK_CUR);
	if (read(fd_ptr, &ble_daily_sync_file_name, 8) == 8)
	{
		printf("\nLatest test folder previously created read from SD CARD is: %s with length %u", ble_daily_sync_file_name, strlen(ble_daily_sync_file_name));
	}


	/* Closing settings file from SD card*/
	fclose(fd_ptr);
}

/*-----------------------------------------------------------------------------
 *  Function:     Update_BLE_file_manifest
 *  Brief:        This function syncs the test files between both devices.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Update_BLE_file_manifest(void)
{
	MQX_FILE_PTR daily_file_ptr, weekly_file_ptr, all_file_ptr, test_file_ptr;
	char name[20]="\\";
	char root_dir_name[10]="a:\\";
	uint32_t tempcount=0;

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name);
	if(error_code==MQX_OK)
	{
		printf("\nCHANGED DIRECTORY TO %s" , root_dir_name);
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}

	/* Opening settings file from SD card*/
	if(strcmp(ble_daily_sync_file_name, Test_file_directory.test_folder_name) == 0)
	{
		daily_file_ptr = fopen("a:_TODAY", "a");
		if (daily_file_ptr == NULL)
		{
			return;
		}
	}
	else
	{
		daily_file_ptr = fopen("a:_TODAY", "w");	
		if (daily_file_ptr == NULL)
		{
			return;
		}
	}
	weekly_file_ptr = fopen("a:_PAST_WEEK", "a");
	if (weekly_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		return;
	}

	all_file_ptr = fopen("a:_ALL", "a");
	if (all_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		fclose(weekly_file_ptr);
		return;
	}

	test_file_ptr = fopen("a:_ALL_PATH", "a");
	if (test_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		fclose(weekly_file_ptr);
		fclose(all_file_ptr);
		return;
	}


	write(daily_file_ptr, &Test_Count_Flash.Total_Test_Ct, 4);
	printf("\nFile Name %08u written to Daily_File_Manifest with size: %d", Test_Count_Flash.Total_Test_Ct, sizeof( Test_Count_Flash.Total_Test_Ct));

	write(weekly_file_ptr, &Test_Count_Flash.Total_Test_Ct, 4);
	printf("\nFile Name %08u written to Weekly_File_Manifest with size: %d", Test_Count_Flash.Total_Test_Ct, sizeof( Test_Count_Flash.Total_Test_Ct));

	write(all_file_ptr, &Test_Count_Flash.Total_Test_Ct, 4);
	printf("\nFile Name %08u written to All_File_Manifest with size: %d", Test_Count_Flash.Total_Test_Ct, sizeof( Test_Count_Flash.Total_Test_Ct));

	write(test_file_ptr, &Test_Count_Flash.Total_Test_Ct, 4);
	printf("\nFile Name %08u written to Test_File_Manifest with size: %d", Test_Count_Flash.Total_Test_Ct, sizeof( Test_Count_Flash.Total_Test_Ct));
	write(test_file_ptr, &Test_file_directory.Processed_Data_file_name, 32);
	printf("\nFile Path %s written to Test_File_Manifest with size: %d", Test_file_directory.Processed_Data_file_name, sizeof(Test_file_directory.Processed_Data_file_name));

	/* Closing settings file from SD card*/
	fclose(daily_file_ptr);
	fclose(weekly_file_ptr);
	fclose(all_file_ptr);
	fclose(test_file_ptr);
}

uint_8 Sd_Card_deinit(void) 
{

	if(Sensor_status.SDcard_status == NOT_INITIALISED)
	{
		printf("\nSD Card Already uninstalled ..!");
		return 1;
	}

	Create_file_system_if_sd_card_detected(FALSE);	
	SD_disable();

	printf("\nSD Card  uninstalled successfully..!");
	Sensor_status.SDcard_status = NOT_INITIALISED;
	return 1;

	//	state=0;
}

/*-----------------------------------------------------------------------------
 *  Function:     Create_subdir
 *  Brief:        Creates a folder and names it with the specified date, and creates all the file paths using this date and rtc time.
 *  Parameter:    date: This is the date obtained from the RTC or the GPS
 *  Return:       None
 -----------------------------------------------------------------------------*/
uint_8 Create_subdir(char date[12])
{
	_mqx_int error_code;
	char name[20]="a:\\";

	snprintf(Test_file_directory.Processed_Data_file_name,31, 			"a:\\%s\\P%s", Test_file_directory.test_folder_name, Test_file_directory.test_file_name+1);
	snprintf(Test_file_directory.Name_of_pressure_pixel_file,31, 		"a:\\%s\\X%s", Test_file_directory.test_folder_name, Test_file_directory.test_file_name+1);
	snprintf(Test_file_directory.Name_of_RawDataFile,31, 				"a:\\%s\\R%s", Test_file_directory.test_folder_name, Test_file_directory.test_file_name+1);

	printf("\nProcessed file name: %s",Test_file_directory.Processed_Data_file_name 	);
	printf("\nName_of_pressure_pixel_file name: %s",Test_file_directory.Name_of_pressure_pixel_file 		);
	printf("\nName_of_RawDataFile name: %s",Test_file_directory.Name_of_RawDataFile 				);

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
	if(error_code==MQX_OK)
	{
		//		printf("\nCHANGED DIRECTORY TO %s" , name);
	}
	else
	{
		printf("\nCHANGE DIRECTORY FAILED");
		return 1;
	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_CREATE_SUBDIR , (uint_32_ptr) date);
	if(error_code==MQX_OK)
	{
		printf("\n\nCREATED DIRECTORY: %s" , date);
	}
	else
	{
		printf("\nDIRECTORY CREATION FAILED/ ALREADY PRESENT: %s\n", date);
		//		return 1;
	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) date);
	if(error_code==MQX_OK)
	{
		//		printf("\nCHANGED DIRECTORY TO %s" , date);
	}
	else
	{
		printf("\nCHANGE DIRECTORY FAILED");
		//		return 1;
	}

	return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     Sd_Card_File_List
 *  Brief:        This function lists all the files in the folder with the index folder_count obtained after calling 
 	 	 	 	  the function Sd_Card_Folder_List(). Call this only after listing all the folders first.
 *  Parameter:    folder_count: This is the index of the folder in the SD card.
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Sd_Card_File_List(uint_8 folder_count) 
{
	_mqx_int error_code;
	char name[20]="\\";
	uint_32 hour_check;



	// List all files and subdirectories in a directory.
	MFS_SEARCH_DATA search_data;
	MFS_SEARCH_PARAM search;
	char filepath[] = "*.*";
	search.ATTRIBUTE = MFS_SEARCH_ANY;
	search.WILDCARD = filepath;
	search.SEARCH_DATA_PTR = &search_data;

	Sd_card_file_count=0;

	//To select the current snow profile directory.
	strcat(name, Snow_Profile_Directory[folder_count]);
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
//	if(error_code==MQX_OK)
//	{
//		//		printf("\nCHANGED DIRECTORY TO %s" , name);
//	}
//	else
//	{
//		//		printf("\nCHANGE DIRECTORY FAILED");
//		return;
//	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_FIRST_FILE, (uint_32_ptr) &search);
	if(error_code==MQX_OK)
	{
		//  printf("\nFIRST FILE 0 LISTED:%s" , search_data.NAME);
	}
	else
	{
		printf("\nNO FILES PRESENT");
		return;
	}
	hour_check = (uint_32)(search_data.TIME & MFS_MASK_HOURS) 	>>		MFS_SHIFT_HOURS;

	if((strlen(search_data.NAME)>0)&&(strlen(search_data.NAME)<=16)&& (search_data.NAME[0]==OUTPUT_FILE_FORMAT))
	{
		sprintf(Snow_Profile_File_Name[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], search_data.NAME);
		if(hour_check >12)
		{
			hour_check = hour_check - 12;
			sprintf(Snow_Profile_List_Data[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], "X%02d%02uP%02u",
					hour_check, (uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>	MFS_SHIFT_MINUTES,
					(uint_32)(search_data.TIME & MFS_MASK_SECONDS) << 1);
		}
		else
		{
			sprintf(Snow_Profile_List_Data[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], "X%02d%02uA%02u",
					hour_check, (uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>	MFS_SHIFT_MINUTES,
					(uint_32)(search_data.TIME & MFS_MASK_SECONDS) << 1);			
		}

		//						printf("\n\n FILE LIST\n\n  %s  Sd Card File Count:%d", Snow_Profile_List_Data[Sd_card_file_count],Sd_card_file_count);
		//			printf("\n\n FILE NAME LIST\n\n  %s  Sd Card File Count:%d", Snow_Profile_File_Name[Sd_card_file_count],Sd_card_file_count);

		++Sd_card_file_count;
	}


	while(strlen(search_data.NAME)!=0)
	{
		error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_NEXT_FILE, (uint_32_ptr) &search_data);
		if(error_code==MQX_OK)
		{
			//			   printf("\nNEXTFILE %d LISTED:%s" , Sd_card_file_count, search_data.NAME);
		}
		else
		{
			//			printf("\nALL FILES LISTED SUCCESSFULLY");
			break;
		}
		hour_check = (uint_32)(search_data.TIME & MFS_MASK_HOURS) 	>>		MFS_SHIFT_HOURS;


		if((strlen(search_data.NAME)>0)&&(strlen(search_data.NAME)<=16)&& (search_data.NAME[0]==OUTPUT_FILE_FORMAT))
		{
			sprintf(Snow_Profile_File_Name[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], search_data.NAME);
			if(hour_check >12)
			{
				hour_check = hour_check - 12;
				sprintf(Snow_Profile_List_Data[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], "X%02d%02uP%02u",
						hour_check, (uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>	MFS_SHIFT_MINUTES,
						(uint_32)(search_data.TIME & MFS_MASK_SECONDS) << 1);
			}
			else
			{
				sprintf(Snow_Profile_List_Data[Sd_card_file_count%MAX_NO_OF_FILES_IN_DIR], "X%02d%02uA%02u",
						hour_check, (uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>	MFS_SHIFT_MINUTES,
						(uint_32)(search_data.TIME & MFS_MASK_SECONDS) << 1);
			}

			//								printf("\n\n FILE LIST\n\n  %s  Sd Card File Count:%d", Snow_Profile_List_Data[Sd_card_file_count],Sd_card_file_count);
			//				printf("\n\n FILE NAME LIST\n\n  %s  Sd Card File Count:%d", Snow_Profile_File_Name[Sd_card_file_count],Sd_card_file_count);
			++Sd_card_file_count;
		}

	}
	//	printf("\n\n TOTAL Sd Card File Count:%d", Sd_card_file_count);
	if(Sd_card_file_count >= MAX_NO_OF_FILES_IN_DIR)
	{
		Sd_card_file_count = MAX_NO_OF_FILES_IN_DIR;
	}
	//	printf("\n NEW Sd Card File Count:%d", Sd_card_file_count);

	flip_directory();

}

/*-----------------------------------------------------------------------------
 *  Function:     Sd_Card_Folder_List
 *  Brief:        Lists all the folders in root directory and stores into array. Sd_Card_File_List() is called after calling this 
 	 	 	 	  function using the index of the folder listed in this array.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Sd_Card_Folder_List(void) 
{
	_mqx_int error_code;

	//	List all files and subdirectories in a directory.
	MFS_SEARCH_DATA search_data;
	MFS_SEARCH_PARAM search;
	char filepath[] = "*.*";
	search.ATTRIBUTE = MFS_SEARCH_SUBDIR;
	search.WILDCARD = filepath;
	search.SEARCH_DATA_PTR = &search_data;

	Sd_card_folder_count=0;
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
//	if(error_code==MQX_OK)
//	{
//		//		printf("\nCHANGED DIRECTORY TO a:\\");
//	}
//	else
//	{
//		//		printf("\nCHANGE DIRECTORY FAILED\n");
//		return;
//	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_FIRST_FILE, (uint_32_ptr) &search);
	//	printf("\n\nSTORED SNOW PROFILE FOLDER LIST\n\n");
	sprintf(Snow_Profile_Directory[Sd_card_folder_count++], search_data.NAME, NULL);
	//	printf("\n%s", Snow_Profile_Directory[0]);
	while (error_code == MFS_NO_ERROR) 
	{
		//		printf("\n%s", search_data.NAME);
		error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_NEXT_FILE,	(uint_32_ptr) &search_data);
		sprintf(Snow_Profile_Directory[Sd_card_folder_count % MAX_NO_OF_DIR], search_data.NAME, NULL);
		//		printf("\n%s", Snow_Profile_Directory[Sd_card_folder_count % MAX_NO_OF_DIR]);
		++Sd_card_folder_count;
	}

	//	printf("\n\n TOTAL SD Card FOLDER COUNT:%d", Sd_card_folder_count);
	if(Sd_card_folder_count >= MAX_NO_OF_DIR)
	{
		Sd_card_folder_count = MAX_NO_OF_DIR;
	}
	//	printf("\n NEW SD Card FOLDER COUNT:%d", Sd_card_folder_count);
	//	Sort_Sd_Card_Folder_List_in_Descending_Order();

}

/*-----------------------------------------------------------------------------
 *  Function:     Delete_File
 *  Brief:        This function deletes all of the test files of that particular test.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Delete_File(uint_8 folder_count, uint_8 file_count) 
{
	char name[20]="a:\\";

	printf("\n\nFOLDER: %s  FILE: %s\n" ,  Snow_Profile_Directory[folder_count],  Snow_Profile_File_Name[file_count]);

	//To select the current snow profile directory.
	strcat(name, Snow_Profile_Directory[folder_count]);
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) name);
	if(error_code==MQX_OK)
	{
		//		printf("\nCHANGED DIRECTORY TO %s\n" , name);
	}
	else
	{
		printf("\nCHANGE DIRECTORY FAILED in Delete_File()\n");
		return;
	}

	sprintf(Test_file_directory.Processed_Data_file_name, 			"a:\\%s\\P%s", Snow_Profile_Directory[folder_count], Snow_Profile_File_Name[file_count]+1);
	sprintf(Test_file_directory.Name_of_pressure_pixel_file, 		"a:\\%s\\X%s", Snow_Profile_Directory[folder_count], Snow_Profile_File_Name[file_count]+1);
	sprintf(Test_file_directory.Name_of_RawDataFile, 				"a:\\%s\\R%s", Snow_Profile_Directory[folder_count], Snow_Profile_File_Name[file_count]+1);

	error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_DELETE_FILE,	(uint_32_ptr) Test_file_directory.Processed_Data_file_name);
	if(error_code==MQX_OK)
	{
		//		printf("\nPROCESSED FILE DELETED :  %s" , Test_file_directory.Processed_Data_file_name);
	}
	else
	{
		printf("\nPROCESSED FILE DELETION FAILED");
		//		return;
	}

	error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_DELETE_FILE,	(uint_32_ptr) Test_file_directory.Name_of_RawDataFile);
	if(error_code==MQX_OK)
	{
		//		printf("\nRAW DATA FILE DELETED: %s" , Test_file_directory.Name_of_RawDataFile);
	}
	else
	{
		printf("\nRAW DATA FILE DELETION FAILED");
		//		return;
	}


	error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_DELETE_FILE,	(uint_32_ptr) Test_file_directory.Name_of_pressure_pixel_file);
	if(error_code==MQX_OK)
	{
		//		printf("\nPRESSURE PIXEL DATA FILE DELETED %s" , Test_file_directory.Name_of_pressure_pixel_file);
	}
	else
	{
		printf("\nPRESSURE PIXEL FILE DELETION FAILED");
		//		return;
	}

	if(Sd_card_file_count <= 1)
	{
		error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
		sprintf(name, "\\%s", Snow_Profile_Directory[folder_count]);
		error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_REMOVE_SUBDIR,	(uint_32_ptr) name);
		if(error_code==MQX_OK)
		{
			printf("\nFOLDER  %s REMOVED in Delete_File" , name);
			Next_State = UI_SNOW_PROFILE_LIST;  
			State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;
			Sd_Card_Folder_List();
			Folder_name_index = 0;
			Snow_Profiles_List_selection = 0;
			display_Snow_Profiles_List();
			return;
		}
	}

	if(File_name_index >0)
		File_name_index--;
	
	if(Snow_Profiles_File_List_selection)
		Snow_Profiles_File_List_selection--;
	
	Sd_Card_File_List(folder_count);
	display_Snow_Profile_Details();
	Next_State = UI_SNOW_PROFILE_DETAILS;  
	State_of_Screen = UI_IDLE_CHECK_DISPLAY_OFF;

	//	if(file_count < 2)
	//	{
	//		Sd_Card_File_List(folder_count);
	//		error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	//		if(error_code==MQX_OK)
	//			printf("\nCHANGED DIRECTORY TO a:\\" );
	//		else
	//			printf("\nCHANGE DIRECTORY FAILED\n");
	//		if(Sd_card_file_count <= 1)
	//		{
	//			sprintf(name, "\\%s", Snow_Profile_Directory[folder_count]);
	//			error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_REMOVE_SUBDIR,	(uint_32_ptr) name);
	//			if(error_code==MQX_OK)
	//				printf("\nFOLDER  %s REMOVED in Delete_File" , name);
	//			else
	//				printf("\nFOLDER %s REMOVAL FAILED in Delete_File", name);
	//		}
	//	}
}

/*-----------------------------------------------------------------------------
 *  Function:     List_File_Attributes
 *  Brief:        Temporary function created for debugging purpose. This lists all the files and folders in SD card
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void List_File_Attributes(void)  
{
	_mqx_int error_code;
	char name[20]="a:\\";

	//	List all files and sub directories in a directory.
	MFS_SEARCH_DATA search_data;
	MFS_SEARCH_PARAM search;
	char filepath[] = "*.*";
	search.ATTRIBUTE = MFS_SEARCH_ANY;
	search.WILDCARD = filepath;
	search.SEARCH_DATA_PTR = &search_data;

	printf("\nLISTING FILE ATTRIBUTES!!!! ");
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
	{
		printf("\nDIRECTORY CHANGED TO \"a:\\\" for LISTING FILE ATTRIBUTES");
	}
	else
	{
		//		printf("\nDIRECTORY CHANGING FAILED FOR LISTING FILE ATTRIBUTES");
		//		return;
	}
	error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_FIRST_FILE, (uint_32_ptr) &search);
	date_word = search_data.DATE;
	time_word = search_data.TIME;
	while (error_code == MFS_NO_ERROR) 
	{

		printf ("\nName: %-12.12s    FILE SIZE: %6lu    DATE(MM-DD-YY): %02lu-%02lu-%04lu    TIME(HH:MM:SS): %02lu:%02lu:%02lu", 
				search_data.NAME, 
				search_data.FILE_SIZE,
				(uint_32)(search_data.DATE & MFS_MASK_MONTH) 	>> 		MFS_SHIFT_MONTH,
				(uint_32)(search_data.DATE & MFS_MASK_DAY)   	>>		MFS_SHIFT_DAY,
				(uint_32)((search_data.DATE & MFS_MASK_YEAR) 	>>		MFS_SHIFT_YEAR) + 1980,
				(uint_32)(search_data.TIME & MFS_MASK_HOURS) 	>>		MFS_SHIFT_HOURS,
				(uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>		MFS_SHIFT_MINUTES,
				(uint_32)(search_data.TIME & MFS_MASK_SECONDS) 	<< 		1);
		error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_NEXT_FILE,	(uint_32_ptr) &search_data);

	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "20141008");
	if(error_code==MQX_OK)
	{
		printf("\nDIRECTORY CHANGED TO 20141008 for LISTING FILE ATTRIBUTES");
	}
	else
	{
		//		printf("\nDIRECTORY CHANGING FAILED FOR LISTING FILE ATTRIBUTES");
		//		return;
	}
	error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_FIRST_FILE, (uint_32_ptr) &search);

	while (error_code == MFS_NO_ERROR) 
	{

		printf ("\nName: %-12.12s    FILE SIZE: %6lu    DATE(MM-DD-YY): %02lu-%02lu-%04lu    TIME(HH:MM:SS): %02lu:%02lu:%02lu", 
				search_data.NAME, 
				search_data.FILE_SIZE,
				(uint_32)(search_data.DATE & MFS_MASK_MONTH) 	>> 		MFS_SHIFT_MONTH,
				(uint_32)(search_data.DATE & MFS_MASK_DAY)   	>>		MFS_SHIFT_DAY,
				(uint_32)((search_data.DATE & MFS_MASK_YEAR) 	>>		MFS_SHIFT_YEAR) + 1980,
				(uint_32)(search_data.TIME & MFS_MASK_HOURS) 	>>		MFS_SHIFT_HOURS,
				(uint_32)(search_data.TIME & MFS_MASK_MINUTES) 	>>		MFS_SHIFT_MINUTES,
				(uint_32)(search_data.TIME & MFS_MASK_SECONDS) 	<< 		1);
		error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_NEXT_FILE,	(uint_32_ptr) &search_data);

	}

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
	{
		printf("\nDIRECTORY CHANGED TO \"a:\\\" After LISTING FILE ATTRIBUTES");
	}
	else
	{
		//		printf("\nDIRECTORY CHANGING FAILED after LISTING FILE ATTRIBUTES");
		//		return;
	}


}

/*-----------------------------------------------------------------------------
 *  Function:     flip_directory
 *  Brief:        This function Flips the Snow Profile Time List and the Snow Profile File name List for proper display in File List view,
 	 	 	 	  i.e., in decremental order. Dates obtained in listing is actually in incremental ordr, but viewing is in dec order.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void flip_directory(void)
{
	char swap_dir_time[MAX_NO_OF_FILES_IN_DIR][16], swap_dir_name[MAX_NO_OF_FILES_IN_DIR][16];
	uint16_t count_start, count_end;
	//	  printf("\n FLIPPED LISTS!!!");
	count_start = 0;
	while(count_start < Sd_card_file_count)
	{
		memcpy(swap_dir_time[count_start], Snow_Profile_List_Data[count_start], 16);
		memcpy(swap_dir_name[count_start], Snow_Profile_File_Name[count_start], 16);
		++count_start;
	}
	count_start = 0;
	count_end = Sd_card_file_count-1;

	while(count_start < Sd_card_file_count)
	{
		memcpy(Snow_Profile_List_Data[count_start], swap_dir_time[count_end], 16);
		memcpy(Snow_Profile_File_Name[count_start], swap_dir_name[count_end], 16);
		//		  printf("\nTime %s Date %s", Snow_Profile_List_Data[count_start], Snow_Profile_File_Name[count_start]);
		--count_end;
		++count_start;
	}

}

/*-----------------------------------------------------------------------------
 *  Function:     List_File_Attributes
 *  Brief:        Temprorary function created for debugging purpose. This lists all the files and folders in SD card
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Create_ble_manifest_after_startup_and_delete(void)  
{
	printf("\n\n\n*****************CREATING NEW BLE MANIFESTS************\n\n");

	MQX_FILE_PTR daily_file_ptr, weekly_file_ptr, all_file_ptr, test_file_ptr;
	char name[20]="a:\\";
	char temp_folder_name[20]={"\0"}, temp_date[6];
	uint_8 i=0, count=0;
	uint_32 temp_test_count=0, file_date_time_in_seconds, rtc_date_time_in_seconds, testfile_byte_count=0;
	char buffer[(36*Test_Count_Flash.Total_Test_Ct)+1], temp_string[32];

	RTC_TIME_STRUCT time;
	DATE_STRUCT date;
	_rtc_get_time (&time);
	rtc_date_time_in_seconds = time.seconds;
	printf("\nRtc_date_time_in_seconds %u", rtc_date_time_in_seconds);

	Get_timestamp_from_rtc();
	sprintf(temp_folder_name,Data_header.Time_stamp.Date);
	Sd_Card_Folder_List();

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
	{
		//		printf("\nCHANGED DIRECTORY TO a:\\");
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}
	/* OPEN ALL THE FILES IN WRITE MODE TO OVERWRITE THE FILES */
	daily_file_ptr = fopen("a:_TODAY", "w");	
	if (daily_file_ptr == NULL)
	{
		return;
	}


	weekly_file_ptr = fopen("a:_PAST_WEEK", "w");
	if (weekly_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		return;
	}

	all_file_ptr = fopen("a:_ALL", "w");
	if (all_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		fclose(weekly_file_ptr);
		return;
	}

	test_file_ptr = fopen("a:_ALL_PATH", "w");
	if (test_file_ptr == NULL)
	{
		fclose(daily_file_ptr);
		fclose(weekly_file_ptr);
		fclose(all_file_ptr);
		return;
	}

	memset(buffer, 0x00, (36*Test_Count_Flash.Total_Test_Ct));
	write(test_file_ptr, &buffer, (36*Test_Count_Flash.Total_Test_Ct));
	fseek(test_file_ptr, 0, IO_SEEK_SET);

	printf("\n\n\n*****************DATE TIME CONVERSIONS*********\n\n");


	/* TIME AND DATE OF FILE INTO SECONDS  */	

	for(count=0; count < Sd_card_folder_count-1; count++)
	{
		//		printf("\nSnow_Profile_Directory: %s\n",Snow_Profile_Directory[count] );

		sprintf(temp_date, "%c%c%c%c", Snow_Profile_Directory[count][0], Snow_Profile_Directory[count][1],
				Snow_Profile_Directory[count][2], Snow_Profile_Directory[count][3]);
		//		printf("\nYEAR STRING: %s",temp_date );
		date.YEAR = (uint_16)strtol(temp_date, NULL, 10);
		//		printf("\nYEAR INT: %u",date.YEAR );

		sprintf(temp_date, "%c%c", Snow_Profile_Directory[count][4], Snow_Profile_Directory[count][5]);
		//		printf("\nMONTH STRING: %s",temp_date );
		date.MONTH = (uint_16)strtol(temp_date, NULL, 10);
		//		printf("  MONTH INT: %u",date.MONTH );

		sprintf(temp_date, "%c%c", Snow_Profile_Directory[count][6], Snow_Profile_Directory[count][7]);
		//		printf("\nDAY STRING: %s",temp_date );
		date.DAY = (uint_16)strtol(temp_date, NULL, 10);
		//		printf("  DAY INT: %u",date.DAY );

		Sd_Card_File_List(count);
		flip_directory();
		for(i=0;i<Sd_card_file_count;i++)
		{
			//			printf("\nSnow_Profile_List_Data[i]: %s  ",Snow_Profile_List_Data[i] );
			//			printf("  Snow_Profile_File_Name[i]: %s",Snow_Profile_File_Name[i] );

			sprintf(temp_date, "%c%c", Snow_Profile_List_Data[i][1], Snow_Profile_List_Data[i][2]);
			//			printf("\nHOUR STRING: %s",temp_date );
			date.HOUR = (uint_16)strtol(temp_date, NULL, 10);
			if(Snow_Profile_List_Data[i][5] == 'P')
			{
				date.HOUR += 12;
			}
			//			printf("\nHOUR INT: %u",date.HOUR );

			sprintf(temp_date, "%c%c", Snow_Profile_List_Data[i][3], Snow_Profile_List_Data[i][4]);
			//			printf("\nMINUTES STRING: %s",temp_date );
			date.MINUTE = (uint_16)strtol(temp_date, NULL, 10);
			//			printf("  MINUTES INT: %u",date.MINUTE);

			sprintf(temp_date, "%c%c", Snow_Profile_List_Data[i][6], Snow_Profile_List_Data[i][7]);
			//			printf("\nMINUTES STRING: %s",temp_date );
			date.SECOND = (uint_16)strtol(temp_date, NULL, 10);
			//			printf("  MINUTES INT: %u",date.SECOND);

			file_date_time_in_seconds = date_time_to_seconds(&date);
			//			printf("\nFile_date_time_in_seconds : %u", file_date_time_in_seconds);

			temp_test_count = (uint_32)strtol(Snow_Profile_File_Name[i]+1, NULL, 10);
			write(all_file_ptr, &temp_test_count, 4);
			//			printf("\nFile Name %08u written to All_File_Manifest with size: %d\n", temp_test_count, sizeof( temp_test_count));

			if(rtc_date_time_in_seconds > file_date_time_in_seconds)
			{
				if((rtc_date_time_in_seconds - file_date_time_in_seconds) < 604800)
				{
					write(weekly_file_ptr, &temp_test_count, 4);
					//					printf("\n@@@@File Name %08u written to Weekly_File_Manifest with size: %d @@@@\n", temp_test_count, sizeof( temp_test_count));
				}
			}
			if(strcmp(temp_folder_name, Snow_Profile_Directory[count]) == 0)
			{
				write(daily_file_ptr, &temp_test_count, 4);
				//				printf("\n^^^^^File Name %08u written to Daily_File_Manifest with size: %d ^^^^^\n", temp_test_count, sizeof( temp_test_count));

			}

			temp_test_count = (uint_32)strtol(Snow_Profile_File_Name[i]+1, NULL, 10); //Base 10 because multiplication occurs, so hex value error if base 16
			sprintf(temp_string, "a:\\%s\\P%s", Snow_Profile_Directory[count], Snow_Profile_File_Name[i]+1);
			fseek(test_file_ptr, ((temp_test_count-1)*36), IO_SEEK_SET);
			write(test_file_ptr, &temp_test_count, 4);
			write(test_file_ptr, &temp_string, 32);
			fseek(test_file_ptr, 0, IO_SEEK_SET);
		}
	}

	/* Closing settings file from SD card*/
	fclose(daily_file_ptr);
	fclose(weekly_file_ptr);
	fclose(all_file_ptr);
	fclose(test_file_ptr);

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
	{
		//		printf("\nCHANGED DIRECTORY TO a:\\");
	}
	else
	{
		//		printf("\nCHANGE DIRECTORY FAILED");
		return;
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     date_time_to_seconds
 *  Brief:        Function to convert the folder date name and file time name to corresponding total time in seconds for creating manifests,
 	 	 	 	  i.e., all, daily and weekly manifests by comparing with current rtc time.
 	 	 	 	  FUNCTION SHOWING A DIFF OF 1 MONTH, SO DECREMENTED 31 DAYS FROM FINAL DAY_SUM- CAUSE TO BE FOUND;TEMP SOLUTION ONLY.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
uint_32 date_time_to_seconds(DATE_STRUCT_PTR time)
{
	uint_16 day_sum;

#define FEB    2

	static const uint_16 days_no_table[]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	uint_32 result = 0, month, year;

	day_sum = 365 * (time->YEAR - 1970);
	/* get leap year */
	for( year = 1970; year < time->YEAR; year++)
	{
		if( year % 4 == 0 )
		{
			/* leap year, add day */
			result = 1;
			if( year % 100 == 0)
			{
				/* is not leap year, substract day*/  
				result = 0;
			}
			if( year % 400 == 0)
			{
				/* finally is leap year, get back one more day */
				result = 1;
			}
		}

		day_sum += result;      
	}

	result = 0;
	year = time->YEAR;
	for( month = 1; month < time->MONTH; month++)
	{
		day_sum+= days_no_table[month - 1];
		/* is leap year? */
		if( month == FEB )
		{
			if( year % 4 == 0 )
			{
				/* leap year, add day */
				result = 1;
				if( year % 100 == 0)
				{
					/* is not leap year, substract day*/  
					result = 0;
				}
				if( year % 400 == 0)
				{
					/* finally is leap year, get back one more day */
					result = 1;
				}
			}

			day_sum += result;   
		}
	}

	day_sum += time->DAY-1;
	day_sum = day_sum - 31;		//An Additional month is being added during computation.. .To remove it.... 
	//	   printf("\nDay_Sum : %u", day_sum);

	return ((uint_32)(time -> MINUTE * SECS_IN_MINUTE) + 
			(uint_32)(time -> HOUR * SECS_IN_HOUR) + (uint_32)(day_sum * SECS_IN_DAY));  


}

void Sort_Sd_Card_Folder_List_in_Descending_Order(void)
{
	uint_32  fol_date_time_in_seconds, cur_fol_date_time_in_seconds;
	char temp_folder_name[16], temp_date[6];
	DATE_STRUCT pre_date, cur_date;
	uint_16 fol_cnt,sel_cnt;


	for(fol_cnt=0;fol_cnt<Sd_card_folder_count;fol_cnt++)
	{
		sprintf(temp_date, "%c%c%c%c", Snow_Profile_Directory[fol_cnt][0], Snow_Profile_Directory[fol_cnt][1],
				Snow_Profile_Directory[fol_cnt][2], Snow_Profile_Directory[fol_cnt][3]);
		pre_date.YEAR = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", Snow_Profile_Directory[fol_cnt][4],Snow_Profile_Directory[fol_cnt][5]);
		pre_date.MONTH = (uint_16)strtol(temp_date, NULL, 10);

		sprintf(temp_date, "%c%c", Snow_Profile_Directory[fol_cnt][6], Snow_Profile_Directory[fol_cnt][7]);
		pre_date.DAY = (uint_16)strtol(temp_date, NULL, 10);

		pre_date.HOUR = 0;
		pre_date.MINUTE = 0;
		pre_date.SECOND = 0;

		fol_date_time_in_seconds = date_time_to_seconds(&pre_date);

		for(sel_cnt = fol_cnt; sel_cnt < Sd_card_folder_count; sel_cnt++)
		{
			sprintf(temp_date, "%c%c%c%c", Snow_Profile_Directory[sel_cnt][0], Snow_Profile_Directory[sel_cnt][1],
					Snow_Profile_Directory[sel_cnt][2], Snow_Profile_Directory[sel_cnt][3]);
			cur_date.YEAR = (uint_16)strtol(temp_date, NULL, 10);

			sprintf(temp_date, "%c%c", Snow_Profile_Directory[sel_cnt][4],Snow_Profile_Directory[sel_cnt][5]);
			cur_date.MONTH = (uint_16)strtol(temp_date, NULL, 10);

			sprintf(temp_date, "%c%c", Snow_Profile_Directory[sel_cnt][6], Snow_Profile_Directory[sel_cnt][7]);
			cur_date.DAY = (uint_16)strtol(temp_date, NULL, 10);

			cur_date.HOUR = 0;
			cur_date.MINUTE = 0;
			cur_date.SECOND = 0;

			cur_fol_date_time_in_seconds = date_time_to_seconds(&cur_date);

			if(fol_date_time_in_seconds < cur_fol_date_time_in_seconds)	//Swapping
			{	
				sprintf(temp_folder_name, Snow_Profile_Directory[fol_cnt]);
				sprintf(Snow_Profile_Directory[fol_cnt], Snow_Profile_Directory[sel_cnt]);
				sprintf(Snow_Profile_Directory[sel_cnt], temp_folder_name);	
				//				break;
			}
		}

	}


}

/*-----------------------------------------------------------------------------
 *  Function:     Delete_All_Sd_Card_Data
 *  Brief:        This function deletes all of the files in sd card - EQUAL TO A CLEAN FORMATTING UNLESS FOLDERS WITH NON TEST FILES ARE PRESENT.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Delete_All_Sd_Card_Data(void) 
{
	char file_to_delete[24];
	uint_16 fol_cnt, file_cnt;
	MFS_SEARCH_DATA search_data;
	MFS_SEARCH_PARAM search;
	char filepath[] = "*.*";
	search.ATTRIBUTE = MFS_SEARCH_ANY;
	search.WILDCARD = filepath;
	search.SEARCH_DATA_PTR = &search_data;

	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
		printf("\nCHANGED DIRECTORY TO a:\\");
	else
		printf("\nCHANGE DIRECTORY FAILED\n");

	error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_FIRST_FILE, (uint_32_ptr) &search);
	if(error_code==MQX_OK)
	{
		//  printf("\nFIRST FILE 0 LISTED:%s" , search_data.NAME);
	}
	else
	{
		printf("\nNO FILES PRESENT");
		return;
	}

	while(strlen(search_data.NAME)!=0)
	{
		// If you donot want to delete any files, add their names here in below format (replace hello2 or add others like RA.HEX!!
		if((strcmp(search_data.NAME , "RA.HEX") != 0) && (strcmp(search_data.NAME , "Hello2") != 0))
		{	
			sprintf(file_to_delete, "a:%s", search_data.NAME);

			error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_DELETE_FILE,	(uint_32_ptr) file_to_delete);
			if(error_code==MQX_OK)
			{
				//				printf("\nFILE REMOVED :  %s" , file_to_delete);
			}
			else
				printf("\nFILE REMOVAL FAILED :  %s" , file_to_delete);
		}
		else
		{
			printf("\n\n RA.HEX File %s skipped", search_data.NAME);
		}


		error_code = ioctl(filesystem_handle, IO_IOCTL_FIND_NEXT_FILE, (uint_32_ptr) &search_data);
		if(error_code!=MQX_OK)
		{
			printf("\nALL FILES DELETED SUCCESSFULLY");
			break;
		}

	}
	// Now to delete all the Folders and Test Files!!
#if 1
	Sd_Card_Folder_List();
	for(fol_cnt = 0; fol_cnt < Sd_card_folder_count; fol_cnt++)
	{
		Sd_Card_File_List(fol_cnt);
		for(file_cnt = 0; file_cnt < Sd_card_file_count; file_cnt++)
		{
			Delete_File(fol_cnt, file_cnt);
		}

		error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
		if(error_code==MQX_OK)
			printf("\nCHANGED DIRECTORY TO a:\\" );
		else
			printf("\nCHANGE DIRECTORY FAILED\n");

		sprintf(file_to_delete, "\\%s", Snow_Profile_Directory[fol_cnt]);
		error_code = _io_ioctl(filesystem_handle,	IO_IOCTL_REMOVE_SUBDIR,	(uint_32_ptr) file_to_delete);
		if(error_code==MQX_OK)
			printf("\nFOLDER  %s REMOVED" , file_to_delete);
		else
			printf("\nFOLDER %s REMOVAL FAILED", file_to_delete);
	}

#endif
	for(fol_cnt = 0; fol_cnt < Favourites_file_count; fol_cnt++)
	{
		FavouriteEntry[fol_cnt].Favourites_Dir_Count = 0;
		Favourites_file_count = 0;
	}
}

void format_sd_card(void)
{	
	printf("\nStarting Default Format\n\n");
	error_code = ioctl(filesystem_handle, IO_IOCTL_DEFAULT_FORMAT, NULL);	
	if(error_code == MQX_OK)
	{
		printf("\n\nFORMATTING SUCCESS!!!!!!!!!!!!!!!!!!!!!!\n\n");
	}
	else
	{
		printf("\n\nFORMATTING FAILED!!!!!!!!\n\n");
	}
}

long lSize = 0;
uint8_t Test_Sd_card(void)
{
	uint8_t error_code = 0;
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("SDCARD TEST",strlen("SDCARD TEST"));

	
	static uint_8 state=0;
	MQX_FILE_PTR fd_ptr;

//	printf("Initialising SD CARD ...\n");
//	if(	Sd_Card_init()==1)
//	{
//		printf("SD CARD initialisation Failed...\n");
//		return;
//	}
//	printf("SD CARD initialisation Completed...\n");

	Draw_string_new(15,80, (uint_8 *)"STARTING SD TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	printf("\n*******STARTING SD CARD TEST*********\n");
	_time_delay(1000);
	
	ui_timer_start(30000); ///30 sec
	//	printf("\n30 sec Timer started\n");

	while (1) 
	{
//		Create_file_system_if_sd_card_detected(TRUE);
		printf("Writing a file to SD CARD...\n");

		fd_ptr=fopen("a:Test_File.txt","w"); 
		if(fd_ptr==NULL) 
		{ 
			printf ("File error while opening file in write mode...!\n");
			printf("Writing a file to SD CARD Failed.\n");
			error_code = 1;
			break;
		}
		else
		{
			if ( fwrite("hello",1,5,fd_ptr)< 0) 
			{
				printf ("ERROR while writing data to file\n");
				printf("Writing a file to SD CARD Failed.\n");
				error_code = 1;

			}
			else
			{
				printf("Writing a file to SD CARD Success.\n"); 
			}
			fclose (fd_ptr);
		}



		printf("Reading a file from SD CARD...\n");

		fd_ptr=fopen("a:Test_File.txt","r"); 

		if(fd_ptr==NULL) 
		{ 
			printf ("File error while opening file in read mode...!\n");
			printf("Reading a file from SD CARD Failed.\n");
			error_code = 1;
			break;
		}
		else
		{
			fseek(fd_ptr,0,IO_SEEK_END);                                		/*  Set File Pointer to end of file*/
			lSize=ftell(fd_ptr);
			//char *buffer = (char*) malloc (sizeof(char)*lSize);					/* allocate memory to contain the whole file*/
			char buffer[128];//
	/* 		if (buffer == NULL) 
			{ 
				printf ("Memory error...malloc failed!\n"); 

			} */

			fseek(fd_ptr,0,IO_SEEK_SET);                                        /*  Set File Pointer to Begin of file*/
			size_t result = fread (buffer,1,lSize,fd_ptr);	
			if (result != lSize) 
			{ 
				printf ("Reading error.Length mismatch in the file..!\n");
				ui_timer_stop();
				printf("Reading a file from SD CARD Failed.\n");
				error_code = 1;
			}
			fclose (fd_ptr); 

			if((buffer[0]=='h')&&(buffer[1]=='e')&&(buffer[2]=='l')&&(buffer[3]=='l')&&(buffer[4]=='o'))
			{
				printf("Reading a file from SD CARD Success.\n");
				Draw_string_new(15,200, (uint_8 *)"SD TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
				ui_timer_stop();
				break; 
			}
			else
			{
				printf ("Reading error.Data mismatch between write and read..!\n");
				printf("Reading a file from SD CARD Failed.\n");
				Draw_string_new(15,200, (uint_8 *)"SD TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
				ui_timer_stop();
				error_code = 1;
				break; 
			}
		}
		//		}

		if(Check_UI_Timer_Timeout()==TIME_OUT)
		{
			printf("Testing SD CARD failed. Not completed within 30 second. \n");
			Draw_string_new(15,200, (uint_8 *)"SD TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
			error_code = 1;
			break;
		}		
	}
	printf("\n\n*********SD CARD TEST COMPLETED********\n\n");
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(1000);
	return error_code;
}
