/*
 * uart.h
 *
 *  Created on: Dec 19, 2013
 *      Author: GADGEON PC
 */

#ifndef GPS_H_
#define GPS_H_
/******************************************************************************
*
*  HAL_CC4000.h - CC4000 Slim Driver Definitions.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

//*****************************************************************************
// PLATFORM SPECIFIC DEFINITIONS
// 
// Device: MSP430F5529 Chip
// MSP430 Board: MSP430F5529 Experimenter's Board, Rev 0.2
// GPS Board: GNS TC6000GN Rev 1.0 and 2.0
// 
// Modify these definitions to port to other MSP430 devices
// 
//*****************************************************************************

// RETURNS 1 if the number is between low and high
#define BETWEEN(x, low, high)  ((((x) > (low)) && ((x) < (high)))  ? 1 : 0)

//*****************************************************************************
//
// CC4000 Reset GPIO Definitions.
//
//*****************************************************************************
#define CC4000_RESET_PORT_SEL	P7SEL
#define CC4000_RESET_PORT_DIR   P7DIR
#define CC4000_RESET_PORT_OUT	P7OUT
#define CC4000_RESET_PIN       	BIT5

//*****************************************************************************
//
// CC4000 Push-to-Fix GPIO Definitions.
//
//*****************************************************************************
#define CC4000_P2F_PORT_SEL		P2SEL
#define CC4000_P2F_PORT_DIR		P2DIR
#define CC4000_P2F_PORT_OUT		P2OUT
#define CC4000_P2F_PIN       	BIT6

//*****************************************************************************
//
// CC4000 Fix Available GPIO Definitions.
//
//*****************************************************************************
#define CC4000_FIX_PORT_SEL		P1SEL
#define CC4000_FIX_PORT_IFG		P1IFG
#define CC4000_FIX_PORT_IES		P1IES
#define CC4000_FIX_PORT_DIR		P1DIR
#define CC4000_FIX_PORT_IN		P1IN
#define CC4000_FIX_PIN       	BIT0


//*****************************************************************************
//
// CC4000 PPS OUT GPIO Definitions.
//
//*****************************************************************************
#define CC4000_PPS_PORT_SEL		P2SEL
#define CC4000_PPS_PORT_DIR		P2DIR
#define CC4000_PPS_PORT_IN		P2IN
#define CC4000_PPS_PIN       	BIT3

//*****************************************************************************
//
// CC4000 UART Definitions.
//
//*****************************************************************************
#define CC4000_UART_PORT_SEL   	P4SEL
#define CC4000_UART_PORT_DIR   	P4DIR
#define CC4000_UART_PIN_TXD    	BIT4
#define CC4000_UART_PIN_RXD     BIT5
#define CC4000_UART_CTL_REG0    UCA1CTL1
#define CC4000_UART_CTL_REG1    UCA1CTL1
#define CC4000_UART_BAUD_REG0	UCA1BR0
#define CC4000_UART_BAUD_REG1	UCA1BR1
#define CC4000_UART_MOD_REG1	UCA1MCTL
#define CC4000_UART_RX_BUFFER	UCA1RXBUF
#define CC4000_UART_TX_BUFFER	UCA1TXBUF
#define CC4000_UART_STATUS_REG	UCA1STAT
#define CC4000_UART_INT_REG		UCA1IE
#define CC4000_UART_INT_VECTOR	USCI_A1_VECTOR
#define CC4000_UART_INT_ISR		USCI1_ISR
#define _4MHZ 4
#define _25MHZ 25

/***************** End of PLATFORM SPECIFIC DEFINITIONS ********************/


//*****************************************************************************
//
// NMEA Messages State Machine Definitions.
//
//*****************************************************************************

// GPS Messages Status 
#define OLD_GPS_MSG		    0x00
#define NEW_GPS_MSG 		0x01
// GPS Message Delimiters
#define START_OF_MSG		'$'
#define FIELD_DELIMITER		','
#define CHECKSUM_DELIMITER	'*'
#define END_OF_MSG_CR		'\r'
#define END_OF_MSG_LF		'\n'
// GPS Message Properties
#define GPS_MSG_MAX_SIZE	82			// Section 5.3 NMEA Standard
#define GPS_HEADER_SIZE	 	6			// Section 5.2 NMEA Standard, 
                                        // Header: $GPxxx,
#define GPS_CHECKSUM_SIZE	2			// 2 Bytes
#define GPS_GSV_MAX_MSGS	4			// Maximum number of GSV Messages
#define GPS_GSV_MSG_NUM		9			// GSV msg field that indicates 
                                        // current msg	number	
#define GPS_GSV_MSG_TOT		7       // GSV msg field that indicates total number of
                                        // GSV messages the receiver is sending.
                                        
#define GPS_MAX_SV_NUM      20
// GPS Message Headers
#define GPGLL_HEADER	 	"$GPGLL"
#define GPRMC_HEADER	 	"$GPRMC"
#define GPGGA_HEADER	 	"$GPGGA"
#define GPVTG_HEADER	 	"$GPVTG"
#define GPGSA_HEADER	 	"$GPGSA"
#define GPGSV_HEADER	 	"$GPGSV"
#define PTIGCD_HEADER	 	"$PTIGCD"


// GPS Message Errors (API return - MS)	// Section 5.4 NMEA Standard
#define NO_ERROR			0x00
#define CHECKSUM_ERROR		0x01
#define INVALID_CHARACTER	0x02
#define INVALID_HEADER		0x04
#define MSG_SIZE_ERROR		0x08
#define UART_ERROR			0x10
#define OTHER_ERROR			0x20
// GPS State Machine States
#define GPS_MSG_START		0
#define GPS_DATA			1
#define GPS_CHECKSUM		2
#define GPS_MSG_END			3

//*****************************************************************************
//
// CFWVER Message Definitions.
//
//*****************************************************************************
#define CFWVER_TOT_MSGS		1
#define CFWVER_MSG_NUM		2
#define CFWVER_DEVICE       5
#define CFWVER_VERSION      7
#define CFWVER_DATE         8
#define CFWVER_FW_DATE      8
#define CFWVER_FW_TIME      9


//*****************************************************************************
//
// GPSVER Message Definitions.
//
//*****************************************************************************
#define GPSVER_TOT_MSGS		1
#define GPSVER_MSG_NUM		2
#define GPSVER_SPVERSION   11

//*****************************************************************************
//
// CC4000 Status Definitions
//
//*****************************************************************************
#define CC4000_TRACKING		1
#define CC4000_ACQUIRING	2
#define CC4000_INITIALIZING 3
#define CC4000_STATUS_FAIL  0
#define CC4000_NO_STATUS  4

#define CC4000_LOCK_30MS  983    // About 60ms
#define CC4000_LOCK_60MS  1966   // About 60ms
#define CC4000_LOCK_75MS  2458   // About 75ms
#define CC4000_LOCK_150MS  4915  // About 150ms
#define CC4000_LOCK_450MS  14746 // About 450ms
#define CC4000_LOCK_550MS  18022 // About 550ms
#define CC4000_LOCK_650MS  21299 // About 650ms

//*****************************************************************************
//
// CC4000 Core Functions Definitions.
//
//*****************************************************************************
extern void halCC4000Init();
extern void gpsCC4000On(char opt);
extern void CC4000BaudCfg(int,unsigned long);
extern void gpsCC4000Off(void);
extern void gpsCC4000OStartFixes(void);
extern void gpsCC4000OStopFixes(void);
extern void resetCC4000API();
extern void parseGPSMessage(char *gpsMsg, char gpsMsgParam, char *returnParam,
                     char paramSize);
extern char getCC4000Status();
//*****************************************************************************
//
// CC4000 Core Messages Function Definitions.
//
//*****************************************************************************
extern char GLLMsgAvailable();
extern char *getGLLGPSMessage(void);
extern char RMCMsgAvailable();
extern char *getRMCGPSMessage(void);
extern char GGAMsgAvailable();
extern char *getGGAGPSMessage(void);
extern char GSAMsgAvailable();
extern char *getGSAGPSMessage(void);
extern char VTGMsgAvailable();
extern char *getVTGGPSMessage(void);
extern char GSVMsgAvailable();
extern char *getGSVGPSMessage(char gsvMsgNum);

extern char CFWVERMsgAvailable();
extern char *getCFWVERMessage(void);
extern char *getGGPSVERMessage(void);
extern char GPSVERMsgAvailable();

extern void getGPSFirmwareVersion(char *fver, char fverLength);
extern void getGPSSPVersion(char *spver, char spverLength);


//*****************************************************************************
//
// Prototypes for CC4000 APIs.
//
//*****************************************************************************

char getGPSMessageError();

//*****************************************************************************
//
// GPS Message Buffers structure
//
//*****************************************************************************
typedef struct gpsBufferStruct 
{
    char GPGLLBuffer[GPS_MSG_MAX_SIZE];
    char GPRMCBuffer[GPS_MSG_MAX_SIZE];
    char GPGGABuffer[GPS_MSG_MAX_SIZE];
    char GPVTGBuffer[GPS_MSG_MAX_SIZE];
    char GPGSABuffer[GPS_MSG_MAX_SIZE];
    char GPGSVBuffer[GPS_GSV_MAX_MSGS][GPS_MSG_MAX_SIZE];	
    char CFWVERBuffer[GPS_MSG_MAX_SIZE];
    char GPSVERBuffer[GPS_MSG_MAX_SIZE];
    
} GPSBuffer;


typedef enum
{
	GPS_POWER_ON,
	GPS_POWER_OFF,
	GPS_LOCKING,
	GPS_LOCKED,
	GPS_STANDBY
}GPS_CURRENT_STATE;

extern GPS_CURRENT_STATE GPS_Current_State,GPS_Prev_State;
extern uint_8 GPS_uart_deinit(void);
extern void gps_deinit(void);
extern uint_8 GPS_uart_init(void);
//void bsp_serial_io_init_manual(void);
extern void uart_init_manual(
	    /* [IN] Serial device number */
	    uint_8 dev_num,

	    /* [IN] Required functionality */
	    uint_8 flags
	);

extern GPSBuffer gpsBuffer;

extern char gpsBufferUpdate;
extern int last_nmea;
extern char ptigcdCount;
extern LWGPIO_STRUCT GPS_EN;
// State Machine Variables
extern char gpsTempBuffer[GPS_MSG_MAX_SIZE]; 	    // GPS Temp Buffer
extern char gpsTempBufferPtr;		            // Pointer used for GPS Temp Buffer
extern char gpsSMState;                            // Keeps the state of the state machine
extern char gpsMsgChecksum;			    // Tracks checksum within a msg
extern char gpsChecksumBuffer[GPS_CHECKSUM_SIZE];
extern char gpsChecksumBufferPtr;
extern char gpsGSVBufferPtr;

extern char gpsMsgError;			    // Error Description

extern GPS_CURRENT_STATE GPS_Current_State;
extern LWGPIO_STRUCT GPS_RST,GPS_PUSH_FIX,GPS_PPS,FIX_AVL;
//*****************************************************************************
//
// NMEA Message Flags.
//
//*****************************************************************************
extern char GLLMsgFlag;
extern char RMCMsgFlag;
extern char GGAMsgFlag;
extern char VTGMsgFlag;
extern char GSAMsgFlag;
extern char GSVMsgFlag;
extern char CFWVERMsgFlag;
extern char GPSVERMsgFlag;
/*GPIO*/
/*GPIO  configuration*/
#define BSP_GPS_PPS                 (GPIO_PORT_B | GPIO_PIN10)   /*input pis*/

#define BSP_FIX_AVL                 (GPIO_PORT_B | GPIO_PIN11)

#define BSP_GPS_RST                 (GPIO_PORT_B | GPIO_PIN18) /*out put pis*/

#define BSP_PSH_FIX                 (GPIO_PORT_B | GPIO_PIN19)

#define BSP_GPIO_MUX_GPIO           1

extern LWGPIO_STRUCT GPS_RST,GPS_PUSH_FIX,GPS_PPS,FIX_AVL;

extern void gps_gpio_init(void);

#endif /* UART_H_ */
