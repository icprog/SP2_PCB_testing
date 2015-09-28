/*
 * Bulk_transfer.c
 *
 *  Created on: May 28, 2014
 *      Author: user
 */

#include "common_headers.h"
#include "Bulk_transfer.h"

static int _setupAdv(void);
static int _startAdv(void);
void Ble_button_isr(void *pin);
static void _commandParser(void);
static int _setupAuthentication(void);
int _tranferCb(int state,UINT8 *buf, int len);

static void Update_serial_no(void);
void Ble_Host_Interrupt_init(void);
static void Wakeup_BLE_Module(void);
void Ble_wakeup_pin_init(void);
void Ble_reset_pin_init(void);
uint_8 Ble_set_baudrate(void);
uint_8 Test_progress_status = NO;

void Reset_BLE(void);

uint_8 Send_manifest_list(void);
int Send_File(char filename[31],char file_name_to_send[16]);
//uint_8 Send_Files_From_Manifest_List(uint_8);
char Query_list_name[16]={0};

static uint_8 Ble_button_press=0; 
uint_32 BLE_curr_baud = BLE_CURRENT_BAUD_RATE;
		
LWGPIO_STRUCT BLE_TX_BUTTON;
#define BLE_BUTTON			 (GPIO_PORT_D | GPIO_PIN0)
#define BLE_BUTTON_MUX_IRQ 	 (LWGPIO_MUX_D0_GPIO)


uint_8 Ble_wakeup = NO;
uint_8 BLE_host_wakeup_flag = NO;
LWGPIO_STRUCT BLE_host_interrupt;
LWGPIO_STRUCT BLE_wakeup_pin;
LWGPIO_STRUCT BLE_reset_pin;

uint_8 Ble_powerup_stat = POWER_OFF;
/*-----------------------------------------------------------------------------* 
 * Function:    Ble_wakeup_isr
 * Brief:       This function performs all BLE related operations.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void Ble_wakeup_isr(void *pin)
{	
//    Ble_wakeup = YES; 
	lwgpio_int_clear_flag(&BLE_host_interrupt);
}



/*-----------------------------------------------------------------------------* 
 * Function:    Ble_transfer_task
 * Brief:       This function performs all BLE related operations.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/

uint_8 Ble_Module_init(void)
{
	uint_8 status=0;	
	idevice_ready = NO;
	
	/* Initialise the BLE_wakeup_pin  pin */
	Ble_Host_Interrupt_init();		
	/* Initialise the BLE wakeup pin */
	Ble_wakeup_pin_init();	
	/* Initialise the BLE reset pin and Enable BLE */
	Ble_reset_pin_init();
	
	/* Enable_BLE from BLE */
	Enable_BLE();

	if(Ble_uart_init()!=0)
	{		
		printf("\nBLE uart failed to open..\n");
		return 1;
	}
	
	/* Initialising BLE call back functions */
	iDevices_Init(NULL);

	
	uint_32 BLE_Ready_count = 100000000;	
	while(idevice_ready == NO && BLE_Ready_count !=0)
	{
		BLE_Ready_count--;
//		Time_Delay_Sleep(2);
	}
		
	idevice_ready = NO;
	if(!BLE_Ready_count)
	{		
		printf("\nBLE Module not Ready..");
		return 1;
	}

	status = _setupAdv();
	status = _setupAuthentication();
	status = iDevices_initTransferService(2048, _tranferCb);
	status = iDevices_SetAdvEnable(ON, YES);
//	status = iDevices_UpdateConnParams(6, 7, 0, 200);
	status = iDevices_UpdateConnParams(25, 28, 0, 300);		
	return 0;
}




uint_8 Ble_set_baudrate(void)
{
	uint_8 	status=0;

	if(Ble_uart_init()!=0)
	{		
		printf("\nBLE uart failed to open..\n");
		return 1;
	}

	/* Initialising BLE call back functions */
	iDevices_Init(NULL);
	
	status = iDevices_SetBaudrate(BLE_NEW_BAUD_RATE);
	printf("\niDevices_SetBaudrate status : %d",status);
	
	/* Update device serial no: from BLE ID*/
	Update_serial_no();
	
	status = iDevices_Sleep();
	printf("\niDevices_Sleep status : %d",status);

	BLE_curr_baud = BLE_NEW_BAUD_RATE;

	Disable_BLE();
	Ble_uart_deinit();
	
	return 0;
}


void Stop_BLE_bulk_transfer(void)
{   
	Disable_BLE();	
	Ble_uart_deinit();
    /* Clearing BLE  2048 bytes (_rxData) Buffer..*/
    Clear_BLE_rxn_buff();
    	
}

static int _setupAdv(void)
{
	int status = IDEV_SUCCESS;

	// let's default adv data to something
	UINT8 name[28] = "AVATECH :";
	UINT8 advData[MAX_ADV_SIZE];

	memcpy(name + 9,Device_id,14);
	iDevices_InitAdvData(advData, NULL);
//	iDevices_AdvDataSetLocalName(advData, name, 8, NO);
	iDevices_AdvDataSetLocalName(advData, name, 23, NO);
	status = iDevices_SetAdvData(advData, iDevices_AdvDataGetDataSize(advData), NO);

	return status;
	
//	int status = IDEV_SUCCESS;
//
//	// let's default adv data to something
//	UINT8 name[28] = "AVTCH SP2:";
//	UINT8 advData[MAX_ADV_SIZE];
//
//	memcpy(name + 10,Device_id,14);
//	iDevices_InitAdvData(advData, NULL);
////	iDevices_AdvDataSetLocalName(advData, name, 8, NO);
//	iDevices_AdvDataSetLocalName(advData, name, 24, NO);
//	status = iDevices_SetAdvData(advData, iDevices_AdvDataGetDataSize(advData), NO);
//
//	return status;
}

int BLE_Advertising(uint_8 state)
{
	if(Settings_Data.BLE_power_status == BLUETOOTH_ON)	
	{
		if(state == ON)
		{   
			Test_progress_status = NO; 		
			iDevices_SetAdvEnable(ON, YES);
			lwgpio_int_enable(&BLE_host_interrupt, TRUE );
			return 0;
		}

		else
		{	
			Test_progress_status = YES; 
			lwgpio_int_enable(&BLE_host_interrupt, FALSE );
			iDevices_SetAdvEnable(OFF, NO);		
			return 0;
		}
	}
	return 0;

}


static UINT8 _authKey[16] = {0x43, 0x8c, 0x76, 0x92, 0x15, 0xce, 0x39, 0x8c, 0x5f, 0x57, 0xde, 0x3f, 0x70, 0x78, 0xee, 0x06};


static int _setupAuthentication(void)
{
	int status = IDEV_SUCCESS;

	status = iDevices_SetEncryptionKey(_authKey);
	status = iDevices_SetAuthenticationEnable(YES);

	return status;
}

static MQX_FILE_PTR fd_ptr_manifest_file; 
static uint_8 BLE_data_size = 0;


int _tranferCb(int state,UINT8 * buff, int len)
{

	switch (state) {
	case TRANSFER_START:

		BLE_data_size = len;

		// for TRANSFER_START, buf is NULL, and len is total size of data to be sent
		//		printf("_tranferCb(TRANSFER_START): dataSize = %d\n", len);
		break;
	case TRANSFER_DATA:
		// for TRANSFER_DATA, len is size of buf, up to a max buffer size,
		// specified when service was created
		//		printf("_tranferCb(TRANSFER_DATA): dataSize = %d\n", len);
		break;
	case TRANSFER_STOP:
		// for TRANSFER_STOP, len is 0
		//		printf("_tranferCb(TRANSFER_STOP): dataSize = %d\n", len);
		if(BLE_data_size == 16 && buff[0] == 'Q')
		{
			memcpy(Query_list_name,"a:",2);
			memcpy(Query_list_name+2,buff+3,12);    // buff+3 for avoiding QRY from received string  (QRY_ALL)
			//			printf("\n Requested Query list : %s\n",Query_list_name);
			ble_transmission_status = BLUETOOTH_QUERY_LIST_COMPLETED;
		}
		else
		{
			fd_ptr_manifest_file = fopen("a:Manifest_File.txt","w");
			if(fd_ptr_manifest_file == NULL)
			{
				printf("\n Error in opening Manifest file..\n");
			}
			if(write(fd_ptr_manifest_file,buff,BLE_data_size) != BLE_data_size)
			{
				printf("\n Manifest file writing failed..\n");
				fclose(fd_ptr_manifest_file);
				return 1;
			}

			ble_transmission_status = BLUETOOTH_SYNC_FILES;
			//			printf("\n Manifest file writing Success..\n");
			fclose(fd_ptr_manifest_file);
		}
		break;

	default:
		break;
	}
	return 0;
}


/*-----------------------------------------------------------------------------* 
 * Function:    Send_File
 * Brief:       This function performs all BLE related operations.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
int Send_File(char file_path[31],char file_name_to_send[16])
{
	int status = 0;
	_mqx_int error_code;
	
	error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
	if(error_code==MQX_OK)
	{
//		printf("\nCHANGED DIRECTORY TO a:\\");
	}
	else
	{
		printf("\nCHANGE DIRECTORY FAILED");
		return 1;
	}
	
	FILE *fp = fopen(file_path, "rb");
	if (fp == NULL) 
	{
//		fprintf(stderr, "failed to open file %s\n", file_path);
		printf("failed to open file %s\n", file_path);
		return 1;
	}

	fseek(fp, 0L, IO_SEEK_END);
	
	int filesize = ftell(fp);
	/*Adding File Name size to file total file length*/
	filesize += 20;	
	fseek(fp, 0L, IO_SEEK_SET);
	
//	printf("filesize = %d\n", filesize);
	UINT8 *dataBuf = (UINT8 *)MALLOC(filesize);
	
	memset(dataBuf,0,20);
	memcpy(dataBuf,file_name_to_send,strlen(file_name_to_send));
//	printf("\nSending File %s with name size %d\n",file_name_to_send, strlen(file_name_to_send));
	
	/*File Name size*/
	int bytesRead = 20;
	
	while (bytesRead < filesize) 
	{

		int status = fread(&dataBuf[bytesRead], 1, 700, fp);
		if (status > 0) 
		{
			bytesRead += status;
//			printf("bytesRead = %d\n", bytesRead);
		} 
		else 
		{
			fprintf(stderr, "failed to read file %s w/ status = %d\n", file_path, status);
//			printf("failed to read file %s w/ status = %d\n", file_path, status);
			free(dataBuf);
			fclose(fp);
			return 1;
		}
	}

	if (bytesRead > 0) 
	{
		status = iDevices_sendBulkData(dataBuf, bytesRead);
		free(dataBuf);
		fclose(fp);
		return status;		
	}
	else
	{
		printf("Try again\n");
		free(dataBuf);
		fclose(fp);
		return 1;
	}
}

int osSetWakeUpModulePin(int state)
{
    // TODO: to be filled out for each embedded host system
	
	/* Does BLE module wake up control   */
//	lwgpio_set_pin_output(BLE_WAKEUP_PIN, state);
//	return 0;
		
	Ble_wakeup = NO;
	BLE_hand_shake_timer_start(50);
    return 0;
}


int osPollWakeUpHostPin(void)
{
    // TODO: to be filled out for each embedded host system
	
//	if(lwgpio_get_pin_input(BLE_INT_PIN))
//	{
//		lwgpio_set_pin_output(BLE_WAKEUP_PIN,0);
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
	
	
	
	if(Ble_wakeup == YES)
	{
		Ble_wakeup = NO;
		return 1;
	}
	else
	{
		return 0;
	}
	
	return 1;
}


void Ble_Host_Interrupt_init(void)
{

	/*******************************************************************************
     Opening the pin for input, initialize interrupt on it and set interrupt handler.
	 *******************************************************************************/
	/* opening pins for input */
	if (!lwgpio_init(&BLE_host_interrupt, BLE_INT_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing button GPIO as input failed.\n");
		_task_block();
	}

	/* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&BLE_host_interrupt, BLE_INT_PIN_MUX_IRQ);
	lwgpio_set_attribute(&BLE_host_interrupt, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(&BLE_host_interrupt, LWGPIO_INT_MODE_RISING))
	{
		printf("Initializing button GPIO for interrupt failed.\n");
		_task_block();
	}
	
	lwgpio_int_clear_flag (&BLE_host_interrupt);

	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(&BLE_host_interrupt), Ble_wakeup_isr, (void *) &BLE_host_interrupt);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(&BLE_host_interrupt), 3, 0, TRUE);
	
	lwgpio_int_clear_flag (&BLE_host_interrupt);
	/* enable interrupt on GPIO peripheral module*/
//	lwgpio_int_enable(&BLE_host_interrupt, TRUE );
	lwgpio_int_enable(&BLE_host_interrupt, FALSE );
	BLE_host_wakeup_flag = NO;

}


/*-----------------------------------------------------------------------------* 
 * Function:    Ble_wakeup_pin_init
 * Brief:       This function initialise BLE module wakeup gpio pin.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void Ble_wakeup_pin_init(void)
{
	/* opening pins for output */
	if (!lwgpio_init(&BLE_wakeup_pin, BLE_WAKEUP_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW))
	{
		printf("Initializing button GPIO as output failed.\n");

	}
	// for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&BLE_wakeup_pin, BLE_WAKEUP_PIN_MUX_IRQ);
	
	/* Makes BLE module wake up pin   */
	lwgpio_set_pin_output(BLE_WAKEUP_PIN, LWGPIO_VALUE_LOW);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Ble_reset_pin_init
 * Brief:       This function initialise BLE module wakeup gpio pin.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Wakeup_BLE_Module(void)
{
	printf("\nTrying to wakeup module...\n");
	lwgpio_set_pin_output(BLE_WAKEUP_PIN, LWGPIO_VALUE_HIGH);
	_time_delay(2);
	while(!lwgpio_get_pin_input(BLE_INT_PIN)){}
	lwgpio_set_pin_output(BLE_WAKEUP_PIN, LWGPIO_VALUE_LOW);
	while(lwgpio_get_pin_input(BLE_INT_PIN)){}
	printf("\nModule wake up Successfully\n");
}

/*-----------------------------------------------------------------------------* 
 * Function:    Ble_reset_pin_init
 * Brief:       This function initialise BLE module wakeup gpio pin.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void Ble_reset_pin_init(void)
{
	/* opening pins for output */
	if (!lwgpio_init(&BLE_reset_pin, BLE_RESET_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW))
	{
		printf("Initializing button GPIO as output failed.\n");

	}
	// for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&BLE_reset_pin, BLE_RESET_PIN_MUX_IRQ);
	
	/* Makes BLE module wake up pin   */
	lwgpio_set_pin_output(BLE_RESET_PIN, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------* 
 * Function:    Enable_BLE
 * Brief:       This function enables BLE from Reset.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void Enable_BLE(void)
{	
	lwgpio_set_pin_output(BLE_RESET_PIN, LWGPIO_VALUE_HIGH);
}

/*-----------------------------------------------------------------------------* 
 * Function:    Disable_BLE
 * Brief:       This function enables BLE from Reset.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void Disable_BLE(void)
{	
	lwgpio_set_pin_output(BLE_RESET_PIN, LWGPIO_VALUE_LOW);
}


/*-----------------------------------------------------------------------------* 
 * Function:    Send_Files_From_Manifest_List
 * Brief:       This function Sends files listed on Manifest list
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
// uint_8 Send_Files_From_Manifest_List(uint_8 BLE_manifest_file_name[16])
// {
//	 	MQX_FILE_PTR manifest_fd_ptr;
//	 	_mqx_int error_code;
//		uint_8 	tempFilename[16]={0};   
//		uint_32 test_ID =0;
//
//		error_code = ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) "a:\\");
//		if(error_code == MQX_OK)
//		{
//			printf("\nCHANGED DIRECTORY TO a:\\");
//		}
//		else
//		{
//			printf("\nCHANGE DIRECTORY FAILED");
//		}
//			
//		/* Opening manifest file from SD card*/
//		manifest_fd_ptr = fopen("a:file_manifest.bin","r");	
//		
//		if (manifest_fd_ptr == NULL)
//		{
//			printf("\nError in opening file file_manifest.bin\n");
//			return 1;
//		}
//	 	 
//		while(fread((char *)&test_ID,4,1,manifest_fd_ptr) == 1)
//		{
//			memset(tempFilename,0,16);
//			sprintf((char *)tempFilename,"a:P%08d",test_ID);
//			printf("\n Sending Test File %s\n",tempFilename);
//			Send_File((char *)tempFilename);
//		}
//		
//		printf("\n Sending Test file completed\n");
//		return 0;
// }


 
 /*-----------------------------------------------------------------------------* 
  * Function:    Send_Files_From_Manifest_List
  * Brief:       This function Sends files listed on Manifest list
  * Parameter:   Task ID
  * Return:      None
  -----------------------------------------------------------------------------*/
 
 int Send_Files_From_Manifest_List(uint_8 BLE_manifest_file_name[20])
 {
	 MQX_FILE_PTR fd_ptr, ble_fd_ptr;
	 char root_dir_name[10]="a:\\";
	 uint32_t temp_test_count;
	 char temp_file_path[32];
	 char tempFilename[16]={0};
	 uint_8 Total_file_count = 0,Send_file_count=0;
	 int result =0;

	 if(ioctl(filesystem_handle, IO_IOCTL_CHANGE_CURRENT_DIR, (uint_32_ptr) root_dir_name) == MQX_OK)
	 {
		 printf("\nCHANGED DIRECTORY TO %s" , root_dir_name);
	 }
	 else
	 {
		 printf("\nCHANGE DIRECTORY FAILED");
		 return 1;
	 }

	 /* Opening settings file from SD card*/
	 fd_ptr = fopen("a:_ALL_PATH", "r");
	 if (fd_ptr == NULL)
	 {		
		 printf("\n\n**********READING PATH OF FILE FOR SYNCING FAILED\n\n***************\n");
		 return 1;
	 }

	 ble_fd_ptr = fopen((char *)BLE_manifest_file_name, "r");
	 if (ble_fd_ptr == NULL)
	 {
		 fclose(fd_ptr);
		 printf("\nREADING BLE FILE FOR SYNCING FAILED\n");
		 return 1;
	 }

	 /* Finding the total file count from Query list*/
	 fseek(ble_fd_ptr, 0L, IO_SEEK_END);	
	 Total_file_count = (ftell(ble_fd_ptr))/4;
	 fseek(ble_fd_ptr, 0L, IO_SEEK_SET);

	 while(read(ble_fd_ptr, &temp_test_count, 4) == 4)
	 {
		 if(Button_Press_Status == LEFT_BUTTON_PRESSED)
		 {
			 printf("\nFile Sending cancelled\n");
			 Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			 break;
		 }
		 fseek(fd_ptr, ((temp_test_count-1)*36)+4, IO_SEEK_SET);
		 if(read(fd_ptr, &temp_file_path, 32)==32)
		 {
			 // 			printf("\nFile path for Test %u is ", temp_test_count, temp_file_path);


			 memset(tempFilename,0,16);
			 sprintf((char *)tempFilename,"P%08d",temp_test_count);
			 // 			printf("\n Sending Test File %s\n",tempFilename);

			 result = Send_File((char *)temp_file_path,(char *)tempFilename);
			 if(result == 0)
			 {
				 Send_file_count++;
			 }
			 else 
			 {
				 break;
			 }
			 //Send the temp_file_path file through BLE eg: Send_File(temp_file_path)
		 }
	 }

	 /* Closing settings file from SD card*/
	 fclose(fd_ptr);
	 fclose(ble_fd_ptr);
	 return result;
//	 return (Total_file_count - Send_file_count);
 }
 
 /*-----------------------------------------------------------------------------
  *  Function:     Create_Task
  *  Brief:        Creates a new task
  *  Parameter:    Task index
  *  Return:       Task id
  -----------------------------------------------------------------------------*/
 static void Update_serial_no(void)
 {
 	printf("\n Device_serial_no reading ");		
 	if(iDevices_GetBDAddr(Device_serial_no) == IDEV_SUCCESS)
 	{
 		sprintf(Device_id,"%02X%02X-%02X%02X-%02X%02X",Device_serial_no[5],Device_serial_no[4],Device_serial_no[3],Device_serial_no[2],Device_serial_no[1],Device_serial_no[0]);
 		
 		/** Max size of Drive name supported by MQX IO_IOCTL_SET_VOLUME is 11, so limiting its size....**/
 		sprintf(Drive_Name,"SP2-%s",Device_id+7);
 		printf("success\n Device_serial_no %s\n",Device_id);
 		
 		/**  Naming SD Card....**/
 		if (ioctl(filesystem_handle, IO_IOCTL_SET_VOLUME, (uint_32_ptr)Drive_Name) != MFS_NO_ERROR)
 		{
 			printf("Error While Naming SDcard.\n");
 		}
 	}
 	else
 	{
 		printf("\nDevice_serial_no reading failed\n");
 	}
 	
 }
 void Test_BLE()
 {
     printf("\n***********STARTING BLE TEST*************\n");
 	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("BLE TEST",strlen("BLE TEST"));
 	Draw_string_new(15,80, (uint_8 *)"STARTING BLE TEST",COLOUR_BLACK,MEDIUM_FONT);
 	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
 	_time_delay(1000);

 	
 		if(Ble_Module_init()==0)
 		{
 			 printf("BLE TESTING SUCCESS...\n");
 			 Draw_string_new(25,200, (uint_8 *)"BLE TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);

 		}
 		else
 		{
 			printf("BLE TESTING FAIL...\n");
 			Draw_string_new(25,200, (uint_8 *)"BLE TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);

 		}
 		
 	Disable_BLE();
 	printf("\n************BLE TEST COMPLETED*************\n");
 	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
 	_time_delay(1000);
 }

