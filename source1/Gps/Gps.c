
/**HEADER********************************************************************
 * 
 * Copyright (c) 2008 Freescale Semiconductor;
 * All Rights Reserved                       
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: hello.c$
 * $Version : 3.8.11.0$
 * $Date    : Jun-6-2012$
 *
 * Comments:
 *
 *   This file contains the source for the hello2 example program.
 *
 *END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <nvic.h>
#include "Gps.h"
#include "UI_Timer.h"
#include "Sd_Card.h"
#include "Button_Layout.h"
#include "Mqx_rtc.h"
#include "common_headers.h"
//*****************************************************************************
//
// Global Variables.
//
//*****************************************************************************
LWGPIO_STRUCT GPS_RST,GPS_PUSH_FIX,GPS_PPS,FIX_AVL;
GPSBuffer gpsBuffer;
GPS_CURRENT_STATE GPS_Current_State = GPS_POWER_OFF, GPS_Prev_State = GPS_POWER_OFF;
char gpsBufferUpdate;
int last_nmea;
char ptigcdCount;
char pcLatitude[] = "              ";
char utc[] = "          ";
// State Machine Variables
char gpsTempBuffer[GPS_MSG_MAX_SIZE]; 	    // GPS Temp Buffer

char gpsTempBufferPtr;		            // Pointer used for GPS Temp Buffer
char gpsSMState;                            // Keeps the state of the state machine
char gpsMsgChecksum;			    // Tracks checksum within a msg
char gpsChecksumBuffer[GPS_CHECKSUM_SIZE];
char gpsChecksumBufferPtr;
char gpsGSVBufferPtr;

char gpsMsgError;			    // Error Description

char gpsTempBufferForHW_Test[8][GPS_MSG_MAX_SIZE]; 
volatile int GpsTestDataAvailable=0;
volatile int Gps_TestDataCount=0;

//*****************************************************************************
//
// NMEA Message Flags.
//
//*****************************************************************************
char GLLMsgFlag = OLD_GPS_MSG;
char RMCMsgFlag = OLD_GPS_MSG;
char GGAMsgFlag = OLD_GPS_MSG;
char VTGMsgFlag = OLD_GPS_MSG;
char GSAMsgFlag = OLD_GPS_MSG;
char GSVMsgFlag = OLD_GPS_MSG;
char CFWVERMsgFlag = OLD_GPS_MSG;
char GPSVERMsgFlag = OLD_GPS_MSG;

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif
/* Task IDs */


UART_MemMapPtr sci_ptr;
uint_8 GPS_test_flag = 0;

/*UART0  configuration*/
#define RS232CHANNEL "ittya:"	//GPS at UART0

MQX_FILE_PTR GPS_Fp;
uint_8 gps_data_read(void);
extern void gps_buffer_clear(void);
static void GPS_int_service_routine(void *pin);
unsigned char GPS_Pin_Intr_Ctr=0;
void GPS_Input_Pin_Init()
{
	GPS_Pin_Intr_Ctr=0;
	/*******************************************************************************
     Opening the pin for input, initialize interrupt on it and set interrupt handler.
	 *******************************************************************************/
	/* opening pins for input */
	if (!lwgpio_init(&FIX_AVL, BSP_FIX_AVL, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing button GPIO as input failed.\n");
		_task_block();
	}

	/* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&FIX_AVL, BSP_GPIO_MUX_GPIO);
//	lwgpio_set_attribute(&FIX_AVL, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(&FIX_AVL, LWGPIO_INT_MODE_FALLING))
	{
		printf("Initializing button GPIO for interrupt failed.\n");
		_task_block();
	}
	lwgpio_int_clear_flag (&FIX_AVL);
	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(&FIX_AVL), GPS_int_service_routine, (void *) &FIX_AVL);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(&FIX_AVL), 3, 0, TRUE);
	    lwgpio_int_clear_flag (&FIX_AVL);
	/* enable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&FIX_AVL, TRUE );
	
	
//	Individual_Button_Config(&FIX_AVL, BSP_FIX_AVL, BSP_GPIO_MUX_GPIO);
}


void gps_gpio_init(void)
{       /*GPS RST as o/p*/
	lwgpio_init(&GPS_RST , BSP_GPS_RST, LWGPIO_DIR_OUTPUT, 	 LWGPIO_VALUE_LOW);  

	lwgpio_set_functionality(&GPS_RST, BSP_GPIO_MUX_GPIO);   
	/*GPS push fix as o/p*/
	lwgpio_init(&GPS_PUSH_FIX, BSP_PSH_FIX, LWGPIO_DIR_OUTPUT, 	 LWGPIO_VALUE_LOW);  

	lwgpio_set_functionality(&GPS_PUSH_FIX, BSP_GPIO_MUX_GPIO); 

	/*GPS pps as i/p*/		
	lwgpio_init(&GPS_PPS , BSP_FIX_AVL, LWGPIO_DIR_INPUT, 	 LWGPIO_VALUE_NOCHANGE); 

	lwgpio_set_functionality(&GPS_PPS, BSP_GPIO_MUX_GPIO); 
#if 1
	GPS_Input_Pin_Init();
#else
	/*GPS fix avl as i/p*/
	lwgpio_init(&FIX_AVL , BSP_FIX_AVL, LWGPIO_DIR_INPUT, 	 LWGPIO_VALUE_NOCHANGE); 
	lwgpio_set_functionality(&FIX_AVL, BSP_GPIO_MUX_GPIO);  
#endif
}


#define GPS_EN_PIN   (GPIO_PORT_B|GPIO_PIN9)
#define GPS_EN_PIN_MUX  (LWGPIO_MUX_B9_GPIO)

extern void gps_regulator_init(void);
extern void gps_enable(void);
extern void gps_disable(void);

LWGPIO_STRUCT GPS_EN;


/*-----------------------------------------------------------------------------
 *  Function:     gps_power_init
 *  Brief:        Configuring the gpio pins which control the GPS power
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void gps_regulator_init(void)
{

	if (!lwgpio_init(&GPS_EN, GPS_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}

	lwgpio_set_functionality(&GPS_EN, GPS_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&GPS_EN, LWGPIO_VALUE_LOW);

}

/*-----------------------------------------------------------------------------
 *  Function:     gps_enable
 *  Brief:        Enable  the GPS power
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void gps_power_on(void)
{
	printf("GPS power on\n");
	lwgpio_set_value(&GPS_EN, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     gps_disable
 *  Brief:        Disable  the GPS power
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void gps_power_off(void)
{

	lwgpio_set_value(&GPS_EN, LWGPIO_VALUE_LOW);
	printf("GPS power off\n");
}




void uart0_rx_tx_isr(pointer )
{
	char rcvByte;
	/* Get the device registers */   
	sci_ptr = _bsp_get_serial_base_address (0);		//DES get base address for UART5
	rcvByte = sci_ptr->S1;				//DES read status register.  Part of two step process to clear the interrupt flag. 
	rcvByte = sci_ptr->D;				//DES get the character 
	//	printf("%c",rcvByte);
	//
	// Enter State Machine
	//	  
	GPS_test_flag =1;
	switch (gpsSMState) 
	{

	case GPS_MSG_START:

		// Is there a new GPS Message?
		if (rcvByte == START_OF_MSG)
		{
			//
			// Reset variables
			//
			gpsMsgError = NO_ERROR;
			gpsMsgChecksum = 0;	
			gpsChecksumBufferPtr = 0;
			gpsTempBufferPtr = 0;

			//
			// Update GPS Temp Buffer
			//
			gpsTempBuffer[gpsTempBufferPtr++] = rcvByte;

			//
			// Go to next state
			//
			gpsSMState = GPS_DATA;  		
		} 
		else 
		{
			//
			// DO_NOTHING, stay in this state until START_OF_MSG is found
			//
		}
		break; // End of GPS_MSG_START state

	case GPS_DATA:

		// Is it the checksum delimiter?
		if (rcvByte == CHECKSUM_DELIMITER) 
		{
			//
			// Go to next state
			//
			gpsSMState = GPS_CHECKSUM;
		}
		else 
		{ // Regular GPS Data  	

			//		  		
			// Update checksum
			//
			gpsMsgChecksum ^= rcvByte;
		}
		if(gpsTempBufferPtr == 20)
		{
			//            __no_operation();
		}

		// Update GPS Temp Buffer
		if (gpsTempBufferPtr < GPS_MSG_MAX_SIZE) 
		{ 
			gpsTempBuffer[gpsTempBufferPtr++] = rcvByte;
		}
		else
		{
			gpsMsgError = MSG_SIZE_ERROR;
			gpsSMState = GPS_MSG_START;
		}  		
		break; // End of GPS_DATA state

	case GPS_CHECKSUM:	

		// Is it the CR character?    	
		if (rcvByte == END_OF_MSG_CR){   
			// Verify checksum
			char tempChecksum = gpsChecksumBuffer[0] << 4;
			tempChecksum |=  gpsChecksumBuffer[1];

			// PTIGCD Message Checksum comes erroneously
			if (gpsMsgChecksum == tempChecksum || strncmp(gpsTempBuffer, PTIGCD_HEADER, GPS_HEADER_SIZE+1) == 0) 
			{ 
				// Go to final state
				gpsSMState = GPS_MSG_END;
			} 
			else 
			{
				gpsMsgError = CHECKSUM_ERROR;
				gpsSMState = GPS_MSG_START;
			}
		} 
		else 
		{
			// Save Checksum value from GPS message
			if(gpsChecksumBufferPtr < GPS_CHECKSUM_SIZE) 
			{ 
				if((rcvByte - '0') <= 9) { 	// If number is 0-9
					gpsChecksumBuffer[gpsChecksumBufferPtr++] = (rcvByte - '0') ;
				} else {					// If number is A-F
					gpsChecksumBuffer[gpsChecksumBufferPtr++] = (rcvByte - 'A' + 10) ;
				}    			
			} 
			else 
			{
				gpsMsgError = MSG_SIZE_ERROR;
				gpsSMState = GPS_MSG_START;
			}
		}

		//
		// Update GPS Temp Buffer
		//
		if (gpsTempBufferPtr < GPS_MSG_MAX_SIZE) 
		{ 
			gpsTempBuffer[gpsTempBufferPtr++] = rcvByte;
		}
		else
		{
			gpsMsgError = MSG_SIZE_ERROR;
			gpsSMState = GPS_MSG_START;
		}
		break; // End of GPS_CHECKSUM state

	case GPS_MSG_END:	

		//
		// Is it the LF character?
		//    	
		if (rcvByte == END_OF_MSG_LF && gpsTempBufferPtr < GPS_MSG_MAX_SIZE)
		{   
			//
			// Update GPS Temp Buffer
			//	
			gpsTempBuffer[gpsTempBufferPtr++] = rcvByte;
			gpsTempBuffer[gpsTempBufferPtr++] = '\0'; // Indicate end of message  
			//Shihab Added for HW Testing
			memcpy(gpsTempBufferForHW_Test[Gps_TestDataCount], gpsTempBuffer, gpsTempBufferPtr); 
			GpsTestDataAvailable=1;
			Gps_TestDataCount++;
			if(Gps_TestDataCount>5)
				Gps_TestDataCount=0;
			// Update correct GPS Buffer
			// GPGLL MSG
			if (strncmp(gpsTempBuffer, GPGLL_HEADER, GPS_HEADER_SIZE) == 0)
			{ 
				GLLMsgFlag = NEW_GPS_MSG;
				memcpy(gpsBuffer.GPGLLBuffer, gpsTempBuffer, gpsTempBufferPtr);  
			}

			//  	
			// GPRMC MSG
			//
			else if (strncmp(gpsTempBuffer, GPRMC_HEADER, GPS_HEADER_SIZE) == 0) 
			{ 
				RMCMsgFlag = NEW_GPS_MSG;
				memcpy(gpsBuffer.GPRMCBuffer, gpsTempBuffer, gpsTempBufferPtr);
			}

			//
			// GPGGA MSG
			//
			else if (strncmp(gpsTempBuffer, GPGGA_HEADER, GPS_HEADER_SIZE) == 0) 
			{ 
				GGAMsgFlag = NEW_GPS_MSG;
				memcpy(gpsBuffer.GPGGABuffer, gpsTempBuffer, gpsTempBufferPtr);		
			}

			//
			// GPVTG MSG
			//
			else if (strncmp(gpsTempBuffer, GPVTG_HEADER, GPS_HEADER_SIZE) == 0) 
			{
				VTGMsgFlag = NEW_GPS_MSG; 
				memcpy(gpsBuffer.GPVTGBuffer, gpsTempBuffer, gpsTempBufferPtr); 
			}

			//
			// GPGSA MSG
			//
			else if (strncmp(gpsTempBuffer, GPGSA_HEADER, GPS_HEADER_SIZE) == 0) 
			{
				GSAMsgFlag = NEW_GPS_MSG; 
				memcpy(gpsBuffer.GPGSABuffer, gpsTempBuffer, gpsTempBufferPtr); 
			}

			//
			// GPGSV MSG
			//				  	
			else if (strncmp(gpsTempBuffer, GPGSV_HEADER, GPS_HEADER_SIZE) == 0) 
			{
				//	
				// Update GPGSV Messages Counter
				//
				gpsGSVBufferPtr++;

				//
				// Verify there are no lost GPGSV messages within a group
				//
				if ( gpsGSVBufferPtr == (gpsTempBuffer[GPS_GSV_MSG_NUM] - '0') ) {
					memcpy(gpsBuffer.GPGSVBuffer[gpsGSVBufferPtr-1], gpsTempBuffer, gpsTempBufferPtr);
				}
				else 
				{
					gpsGSVBufferPtr = 0;
					gpsMsgError = OTHER_ERROR;
				}

				// If last message GSV Message
				if((gpsTempBuffer[GPS_GSV_MSG_NUM] - '0') == (gpsTempBuffer[GPS_GSV_MSG_TOT] - '0') )
				{				  			
					// We've received all current GSV messages

					gpsBufferUpdate = NEW_GPS_MSG;

					last_nmea=1;
					// Indicate # of GSV Messages
					GSVMsgFlag = gpsGSVBufferPtr;	

					//
					// Update global GPS State Machine variables
					//		  			
					gpsGSVBufferPtr = 0;				  			
				}		  				  		
				// Other Header
			}

			//
			// $PTIGCD MSG
			//
			else if (strncmp(gpsTempBuffer, PTIGCD_HEADER, GPS_HEADER_SIZE+1) == 0) 
			{
				if (ptigcdCount == 0)
				{
					memcpy(gpsBuffer.CFWVERBuffer, gpsTempBuffer, gpsTempBufferPtr);
					ptigcdCount++;
					CFWVERMsgFlag = NEW_GPS_MSG; 
				}
				else if (ptigcdCount == 1)
				{				  			
					memcpy(gpsBuffer.GPSVERBuffer, gpsTempBuffer, gpsTempBufferPtr);
					ptigcdCount = 0;
					GPSVERMsgFlag = NEW_GPS_MSG; 
				} 
			}				  	

			else
			{
				gpsMsgError = INVALID_HEADER;
			}    		    		   
		}
		else                 
		{
			gpsMsgError = MSG_SIZE_ERROR + INVALID_CHARACTER;
		}		
		// Go to first state
		gpsSMState = GPS_MSG_START;
		break; // End of GPS_MSG_END state

	default:
		// Error: Should never get here
		break;	
	} // End of State Machine SWITCH case
}




_mqx_int uart0_serial_io_bsp_init(uint_8 )
{
	SIM_MemMapPtr   sim = SIM_BASE_PTR;
	PORT_MemMapPtr  pctl;

	/* Setup GPIO for UART devices */

	pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
	/* PTD6 as RX function (Alt.3) + drive strength */
	pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
	/* PTD7 as TX function (Alt.3) + drive strength */
	pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;



	return 0;
}


void uart_init_manual(
		/* [IN] Serial device number */
		uint_8 dev_num,

		/* [IN] Required functionality */
		uint_8 flags
)
{
	SIM_MemMapPtr   sim = SIM_BASE_PTR;
	PORT_MemMapPtr  pctl;
	pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
	if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
	{
		/* PTD6 as RX function (Alt.3) + drive strength */
		pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
		/* PTD7 as TX function (Alt.3) + drive strength */
		pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
	}
	if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
	{
		/* PTD6 default */
		pctl->PCR[16] = 0;
		/* PTD7 default */
		pctl->PCR[17] = 0;
	}
	if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
	{
		/* start SGI clock */
		sim->SCGC4 |= SIM_SCGC4_UART0_MASK;
	}
	if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
	{
		/* stop SGI clock */
		sim->SCGC4 &= (~ SIM_SCGC4_UART0_MASK);
	}

}

uint_8 GPS_uart_deinit(void)
{
	
	if(Sensor_status.GPS__status == NOT_INITIALISED)
		 {
			 printf("\nGPS UART is Already uninitialised \n");
			 return 1;
		 }
	
	_nvic_int_disable(INT_UART0_RX_TX);
	_nvic_int_init (INT_UART0_RX_TX, 2, FALSE); 	
	if(MQX_OK==fclose(GPS_Fp))
	{
		printf("\nUART0 closed succesfully\n");
	}
	else
	{
		printf("\nUART0 close Error\n");
	}
	
	printf("GPS UART Deinit\n");
	Sensor_status.GPS__status = NOT_INITIALISED;
	return 0;
	
}



uint_8 GPS_uart_init(void)
{
	
	 if(Sensor_status.GPS__status == INITIALISED)
	 {
		 printf("\nGPS UART is Already initialised \n");
		 return 1;
	 }
	// Configuring UART registers
	GPS_Fp = fopen( RS232CHANNEL, BSP_DEFAULT_IO_OPEN_MODE );

	_nvic_int_init  (INT_UART0_RX_TX, 2, TRUE);
	_int_install_isr(INT_UART0_RX_TX, uart0_rx_tx_isr, GPS_Fp); 
	_nvic_int_enable(INT_UART0_RX_TX);
	//uart_init_manual(0,IO_PERIPHERAL_PIN_MUX_ENABLE);
	// configuring UART 0 and PIN MUX
	_bsp_serial_io_init(0,IO_PERIPHERAL_PIN_MUX_ENABLE);
	printf("GPS UART Init\n");
	
	Sensor_status.GPS__status = INITIALISED;
	return 0;
	//	uart0_serial_io_bsp_init(IO_PERIPHERAL_PIN_MUX_ENABLE);
}


void Test_Gps_sensor_output(void)
{
	uint_16 gps_lock_time=0;
	uint_8  Time[20]={0};


	

		if(gps_data_read()== 0)
		{

			printf("DATE: %s\n",Data_header.Time_stamp.Date);

			printf("TIME: %s\n",Data_header.Time_stamp.Time);

			printf("LATI: %s\n",Data_header.Gps_data.Latitude);

			printf("LONG: %s\n",Data_header.Gps_data.Longitude);
			
			Update_rtc((char *)Data_header.Time_stamp.Time,Data_header.Time_stamp.Date);
			Set_Time_Zone(-25200);
			
		}
		else
		{
			printf("DATE: %s\n",Data_header.Time_stamp.Date);

			printf("TIME: %s\n",Data_header.Time_stamp.Time);

			printf("LATI: %s\n",Data_header.Gps_data.Latitude);

			printf("LONG: %s\n",Data_header.Gps_data.Longitude);	
			
		}

}


uint_8 gps_data_read(void)
{
	extern char Gps_Read_Flag;
	
    getGGAGPSUTCTime(Data_header.Time_stamp.Time,16);
    if(strlen(Data_header.Time_stamp.Time)<6)
    {
		strncpy(Data_header.Time_stamp.Time,"NA",16);
		strncpy(Data_header.Time_stamp.Date,"NA",16);
		strncpy(Data_header.Gps_data.Latitude,"NA",16);
		strncpy(Data_header.Gps_data.Longitude,"NA",16);
        return 1;
    }
    
    getRMCGPSDate(Data_header.Time_stamp.Date, 16);
    if(strlen(Data_header.Time_stamp.Date)<6)
    {
		strncpy(Data_header.Time_stamp.Time,"NA",16);
		strncpy(Data_header.Time_stamp.Date,"NA",16);
		strncpy(Data_header.Gps_data.Latitude,"NA",16);
		strncpy(Data_header.Gps_data.Longitude,"NA",16);
        return 1;
    }
	
	getRMCGPSLatitude(Data_header.Gps_data.Latitude, 16);
	if(strlen(Data_header.Gps_data.Latitude)<6)
	{
		strncpy(Data_header.Time_stamp.Time,"NA",16);
		strncpy(Data_header.Time_stamp.Date,"NA",16);
		strncpy(Data_header.Gps_data.Latitude,"NA",16);
		strncpy(Data_header.Gps_data.Longitude,"NA",16);
		return 1;
	}

	getRMCGPSLongitude(Data_header.Gps_data.Longitude, 16);
	if(strlen(Data_header.Gps_data.Longitude)<6)
	{
		strncpy(Data_header.Time_stamp.Time,"NA",16);
		strncpy(Data_header.Time_stamp.Date,"NA",16);
		strncpy(Data_header.Gps_data.Latitude,"NA",16);
		strncpy(Data_header.Gps_data.Longitude,"NA",16);
		return 1;
	}

	else
	{
		return 0;
	}
}

void gps_buffer_clear(void)
{
	Data_header.Time_stamp.Time[0]='\0';
	Data_header.Time_stamp.Date[0]='\0';
	Data_header.Gps_data.Latitude[0]='\0';
	Data_header.Gps_data.Longitude[0]='\0';

}

static void GPS_int_service_routine(void *)
{
	if (lwgpio_int_get_flag(&FIX_AVL) == TRUE )
	{
		GPS_Pin_Intr_Ctr++;
		if(GPS_OFF_timer_count == 0x00)
		{
			/* disable interrupt on GPIO peripheral module*/
			lwgpio_int_enable(&FIX_AVL, FALSE );
			GPS_ON_timer_start(GPS_ON_TIMEOUT);	// 3 min
			gpsCC4000OStopFixes();
			GPS_Current_State = GPS_POWER_OFF;
		}
		else if(GPS_Pin_Intr_Ctr>=5)
		{
			GPS_Pin_Intr_Ctr = 5;
			GPS_Lock_timer_start(5000);	// 5 sec
			GPS_Current_State = GPS_LOCKING;
		}
		GPS_test_flag =1;
	}
	lwgpio_int_clear_flag(&FIX_AVL);
}

void Test_Gps(void)
{
    printf("\n\n\n***********STARTING GPS TEST*************\n");
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("GPS TEST",strlen("GPS TEST"));
	Draw_string_new(15,80, (uint_8 *)"STARTING GPS TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(1000);
	
	
	uint_16 gps_lock_time=0;
	uint_8  Time[20]={0};

//	printf("\nInitialising GPS ...");	
//	halCC4000Init();                           	/*Initialising GPS*/     
//	printf("\nGPS initialisation Completed...");

//	gps_power_on();
//	gps_enable();
//	gpsCC4000On(1);

//	Update_rtc("00000000","000000");   

	uint_16 gps_time_out =500;

	while(!GPS_test_flag && --gps_time_out)
	{		
		_time_delay(10);
	}	
	
	if(!gps_time_out)
	{
		printf("GPS TEST FAILED\n");
		Draw_string_new(25,200, (uint_8 *)"GPS TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
	}
	else
	{
		printf("\nGPS TEST SUCCESS\n");
		Draw_string_new(25,200, (uint_8 *)"GPS TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
	}
	printf("\n************GPS TEST COMPLETED*************\n");
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(1000);
	
//	gpsCC4000Off();
//	gps_disable();	
}
