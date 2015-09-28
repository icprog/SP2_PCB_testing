/******************************************************************************
 *
 *     Filename:       $Hw_Init.c
 *     Created on:     $Date: Mar 25, 2014    (11:56:12 AM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file includes function to initialise hardware modules
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include "Hw_Init.h"
#include "lsm303.h"
#include "tmp006.h"
#include "disk.h"
#include "mqx_prv_new.h"
#include "Gps.h"
#include "UI_Display.h"
#include "UI_Display_Time.h"
//#define GPS_LOCK_DISABLE

unsigned char Usb_Connected = 0;

LWGPIO_STRUCT USB_Pin;

#define USB_PIN				 (GPIO_PORT_A | GPIO_PIN17)
#define USB_PIN_MUX_IRQ 	 (LWGPIO_MUX_A17_GPIO)

static void USB_Isr(void *)
{
	Usb_Connected = 1;
	lwgpio_int_clear_flag(&USB_Pin);
}

void USB_Pin_Init()
{
	/* opening pins for input */
	if (!lwgpio_init(&USB_Pin, USB_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing button GPIO as input failed.\n");
		_task_block();
	}

	/* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&USB_Pin, USB_PIN_MUX_IRQ);
	lwgpio_set_attribute(&USB_Pin, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(&USB_Pin, LWGPIO_INT_MODE_FALLING))
	{
		printf("Initializing button GPIO for interrupt failed.\n");
		_task_block();
	}
	lwgpio_int_clear_flag (&USB_Pin);
	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(&USB_Pin), USB_Isr, (void *) &USB_Pin);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(&USB_Pin), 3, 0, TRUE);
	    lwgpio_int_clear_flag (&USB_Pin);
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&USB_Pin, TRUE );
}

/*-----------------------------------------------------------------------------
 *  Function:     Init_HardWare
 *  Brief:        Initialise all the needed hardware modules 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void Init_HardWare(void)
{   
	printf("\n********************** HARDWARE INIT *******************\n");

	power_rail_init();
	power_rail_enable();
	Kill_Controller_Init();
	Kill_ON_Controller();
	OSC_EN_init();

	ADC_Init();
	ADC_deinit();
	
	printf("Initialising LCD...\n");
	spi_init(); /*Init SPI configuration*/
	lcd_clear(); /* Clearing LCD */
	buff_clear();
	Refresh_Lcd_Buffer((uint_8 *) logo_image);

//	Time_Delay_Sleep(2000);
	printf("Initialising Button...\n");
	Button_Init();
	
	USB_Pin_Init();
	
	// Initialising sd card
	printf("Initialising SD CARD ...\n");
	if(	Sd_Card_init()==1)
	{
		printf("SD CARD initialisation Failed...\n");
		//Handle SD card Error condition
		display_sd_error_screen();
		while (1)
		{
		}
	}

	printf("SD CARD initialisation Completed...\n");
				
//	USB_Init();
	
	Default_Settings_Data();
	Update_settings_Data();

	Read_Flash(Test_Count_Flash.Total_Test_Ctr_Buff, 4);
	printf("\n TOTAL TEST CTR Read:         %u", Test_Count_Flash.Total_Test_Ct);
	if(Test_Count_Flash.Total_Test_Ct == 0xFFFFFFFF)
	{
		Test_Count_Flash.Total_Test_Ct = 0;
		Write_Flash(Test_Count_Flash.Total_Test_Ctr_Buff, 4);
	//	Delete_All_Sd_Card_Data();

	}
	Read_Data_file_information_From_Sdcard();
	Read_Favourite_file_information_From_Sdcard();
	Read_Test_count_Ble_Folder_name();
	Create_ble_manifest_after_startup_and_delete();


	//    sprintf(ble_daily_sync_file_name, Data_header.Time_stamp.Date);

	//TO UPDATE FOLDER AND FILE LIST DURING INITIALISATION FOR FAVOURITES CONTENT
	Sd_Card_Folder_List();
	Create_Snow_Profiles_List_Content();
	Create_Snow_Profiles_File_List_Content();
	//	Sd_Card_deinit();

	strncpy(Data_header.Gps_data.Latitude,"NA",16);
	strncpy(Data_header.Gps_data.Longitude,"NA",16);

#ifndef GPS_LOCK_DISABLE
	printf("\nInitialising GPS ...\n");	

	GPS_OFF_timer_start(GPS_OFF_TIMEOUT);	// 3 min
	GPS_Current_State = GPS_POWER_ON;

	//	halCC4000Init(); removed 	uart_init();
	resetCC4000API();
	/*GPIO for GPS*/
	gps_gpio_init();

	gps_regulator_init(); 
	gps_power_on();
	lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
	gpsCC4000OStartFixes(); 


	//	gps_enable();
	//	GPS_uart_init();
	////	gpsCC4000On(1);	removed _nvic_int_enable(INT_UART0_RX_TX);
	//	lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);

#else
	GPS_Current_State = GPS_POWER_ON;
#endif
	Buzzer_Init();
	Buzzer_On();
	ui_timer_de_init();
	Buzzer_Off();
	/* Disables DDR power  */
	/* Initialise the DDR Power pin */
	//	DDR_power_pin_init();

	// disable power rail
	power_rail_disable();
	
	ui_timer_init();
//	Time_Delay_Sleep(100);

	/* Initialise the BLE and uart baud rate */
	Ble_set_baudrate();
		
	if(Settings_Data.BLE_power_status == BLUETOOTH_ON)
	{
		if(Ble_Module_init()){
			printf("\nBLE Init failed.. Please try again\n"); 
		}
		else{
			printf("\nBLE init Success.... \n");
		}		
		
		BLE_host_wakeup_flag = NO;
		lwgpio_int_enable(&BLE_host_interrupt, TRUE );
	}
	
	Update_rtc("000000", "010114");
	printf("\n********************** COMPLETED*******************\n");
}


/*-----------------------------------------------------------------------------
 *  Function:     Software_reboot
 *  Brief:        Reboot the device and start execution from beginning
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Software_reboot()
{
	printf("\nRebooting...!!!\n\n");
	unsigned int temp_AIRCR = 0;
	VCORTEX_SCB_STRUCT_PTR scb =
			(VCORTEX_SCB_STRUCT_PTR) &(((CORTEX_SCS_STRUCT_PTR) CORTEX_PRI_PERIPH_IN_BASE)->SCB);
	temp_AIRCR = (unsigned int) scb->AIRCR;   //Read AIRCR register
	temp_AIRCR &= 0x0000ffff;   //DES mask of the top 16-bits
	temp_AIRCR |= 0x05Fa0000; //DES When writing to AIRCR the update 16-bit must be "0x05FA" per ARMv7-M Architecture Reference Manual (must register on ARM.com to get)
	temp_AIRCR |= SCB_AIRCR_SYSRESETREQ_MASK; //DES set the SYSRESETREQ bit to generate software reset
	scb->AIRCR = temp_AIRCR;
	_mqx_exit(1);
}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
