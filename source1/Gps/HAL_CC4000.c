/*****************************************************************************
*
*  HAL_CC4000.c - CC4000 Slim Driver Implementation.
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
//
//!  @file       HAL_CC4000.c
//! \addtogroup  CC4000_GPS
//! @{
//
//*****************************************************************************

#include <stdlib.h> 
#include "string.h"
#include "HAL_CC4000.h"
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <nvic.h>
#include "Gps.h"


void gps_deinit(void)
{
    gpsCC4000OStopFixes();
    gpsCC4000Off();	
    GPS_uart_deinit();

}

void gps_enable(void)
{
    gps_buffer_clear();
    resetCC4000API();
//    gpsCC4000On(1);                             /*Reset Line HIGH*/ 
    gpsCC4000OStartFixes();     

}

void gps_disable(void)
{
    gps_buffer_clear();
    resetCC4000API();
    gpsCC4000OStopFixes();
//    gpsCC4000Off(); 
	/* enable interrupt on GPIO peripheral module*/
}

void gps_restart(void)
{
    gps_buffer_clear();
    resetCC4000API();
    
    /* Turn Off GPS */
    gpsCC4000OStopFixes();
    gpsCC4000Off();
    
    Time_Delay_Sleep(500);
    
    /* Turn On GPS */
    gpsCC4000On(1);                           
    gpsCC4000OStartFixes();  
    
}

//*****************************************************************************
//
// Global Variables.
//
//*****************************************************************************

//GPSBuffer gpsBuffer;
//
//char gpsBufferUpdate;
//extern int last_nmea;
//char ptigcdCount;
//
//// State Machine Variables
//char gpsTempBuffer[GPS_MSG_MAX_SIZE]; 	    // GPS Temp Buffer
//char gpsTempBufferPtr;		            // Pointer used for GPS Temp Buffer
//char gpsSMState;                            // Keeps the state of the state machine
//char gpsMsgChecksum;			    // Tracks checksum within a msg
//char gpsChecksumBuffer[GPS_CHECKSUM_SIZE];
//char gpsChecksumBufferPtr;
//char gpsGSVBufferPtr;
//
//char gpsMsgError;			    // Error Description
//
//
//
////*****************************************************************************
////
//// NMEA Message Flags.
////
////*****************************************************************************
//char GLLMsgFlag = OLD_GPS_MSG;
//char RMCMsgFlag = OLD_GPS_MSG;
//char GGAMsgFlag = OLD_GPS_MSG;
//char VTGMsgFlag = OLD_GPS_MSG;
//char GSAMsgFlag = OLD_GPS_MSG;
//char GSVMsgFlag = OLD_GPS_MSG;
//char CFWVERMsgFlag = OLD_GPS_MSG;
//char GPSVERMsgFlag = OLD_GPS_MSG;




/*****************************************************************************
* 
* NOTE: Platform Independent Section
* 
* ***************************************************************************/


//*****************************************************************************
//
//! \brief Initializes the serial communications peripheral and GPIO ports 
//! to communicate with the CC4000.
//!
//! 9600 is the default baudrate. For opt, UCRXIE specifies whether UART RX
//! Interrupts are to be enabled.
//!
//! \param baud is the baudrate, either 9600 or 115200
//! \param opt are the options
//! \param returnParam[] a pointer to an array containing returned parameter
//! \param returnSize is the number of characters to return
//!
//! \return None.
//
//*****************************************************************************
void halCC4000Init()
{
    resetCC4000API();  
    GPS_uart_init();
	/*GPIO for GPS*/
	gps_gpio_init();
	gps_regulator_init();
    
    // CC4000 Reset Configuration
    
    // CC4000 PushToFix Configuration (TBD)
    
    /********** End of Platform Specific Configurations *******************/    
    gpsCC4000Off();	
    gpsCC4000OStopFixes();	
}


//void CC4000BaudCfg(int osc, unsigned long baud){
//  
//  if(osc == _4MHZ)
//    switch(baud)
//    {
//    case 230400:
//        CC4000_UART_BAUD_REG0 = 17;           // 4MHz / 230400
//        CC4000_UART_BAUD_REG1 = 0x00;           // 4MHz / 230400
//        CC4000_UART_MOD_REG1 = UCBRS_3;
//        break;
//    case 115200:
//        CC4000_UART_BAUD_REG0 = 34;           // 4MHz / 115200
//        CC4000_UART_BAUD_REG1 = 0x00;           // 4MHz / 115200
//        CC4000_UART_MOD_REG1 = UCBRS_6;         // Modulation UCBRSx = 7
//        break;		                                
//    case 9600:
//    default:                                    // Defaults to 9600
//        CC4000_UART_BAUD_REG0 = 0xA0;           // 4MHz / 9600
//        CC4000_UART_BAUD_REG1 = 0x01;           // 4MHz / 9600
//        CC4000_UART_MOD_REG1 = UCBRS_6;         // Modulation UCBRSx = 7		
//        break;
//    }
//  
//  else  
//    switch(baud)
//    {
//    case 230400:
//        CC4000_UART_BAUD_REG0 = 0x6C;           // 25MHz / 230400
//        CC4000_UART_BAUD_REG1 = 0x00;           // 25MHz / 230400
//        CC4000_UART_MOD_REG1 = 0x08;
//        break;
//    case 115200:
//        CC4000_UART_BAUD_REG0 = 0xD9;           // 25MHz / 115200
//        CC4000_UART_BAUD_REG1 = 0x00;           // 25MHz / 115200
//        CC4000_UART_MOD_REG1 = UCBRS_4;         // Modulation UCBRSx = 5
//        break;		                                
//    case 9600:
//    default:                                    // Defaults to 9600
//        CC4000_UART_BAUD_REG0 = 0x2C;           // 25MHz / 9600
//        CC4000_UART_BAUD_REG1 = 0x0A;           // 25MHz / 9600
//        CC4000_UART_MOD_REG1 = UCBRS_4;         // Modulation UCBRSx = 5		
//        break;
//    }
//  
//  
//}

//*****************************************************************************
//
//! \brief Turns on CC4000 by making the reset line HIGH.
//!
//! Also enables UART RX Interrupt handler and turns on LED2.
//!
//! \return None.
//
//*****************************************************************************
void gpsCC4000On(char )
{
	
	lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_HIGH);
	_nvic_int_enable(INT_UART0_RX_TX);
	

//    CC4000_RESET_PORT_OUT |= CC4000_RESET_PIN;	// Turns CC4000 ON.
    
//    if(opt & UCRXIE)
//    {
//	CC4000_UART_INT_REG |= UCRXIE;          // Enable UART RX interrupt
//    }
//    else
//    {
// 	CC4000_UART_INT_REG &= ~UCRXIE;         // Disable UART RX interrupt
//    }
//    
//    CC4000_UART_CTL_REG0 &= ~UCSWRST;	        // Release from reset
//    
//    Board_ledOn(LED3);
}

//*****************************************************************************
//
//! \brief Turns off CC4000 by making the reset line LOW.
//!
//! Also disables UART RX Interrupt handler and turns on LED2.
//!
//! \return None.
//
//*****************************************************************************
void gpsCC4000Off(void)
{
	
//  CC4000_RESET_PORT_OUT &= ~CC4000_RESET_PIN;	   // Turns CC4000 OFF.  
	lwgpio_set_value(&GPS_RST, LWGPIO_VALUE_LOW); 
	_nvic_int_disable(INT_UART0_RX_TX);
//CC4000_UART_INT_REG &= ~UCRXIE;			    // Disable UART RX interrupts  
// Board_ledOff(LED3);  
}


//*****************************************************************************
//
//! \brief Command the CC4000 to look for fixes.
//!
//! Turns on the Push to Fix.
//!
//! \return None.
//
//*****************************************************************************
void gpsCC4000OStartFixes(void)
{
	GPS_Pin_Intr_Ctr=0;
//    CC4000_P2F_PORT_OUT |= CC4000_P2F_PIN;
	lwgpio_set_value(&GPS_PUSH_FIX, LWGPIO_VALUE_HIGH);	
//    Board_ledOn(LED2);
}

//*****************************************************************************
//
//! \brief Command the CC4000 to stop looking for fixes.
//!
//! Turns off the Push to Fix.
//!
//! \return None.
//
//*****************************************************************************
void gpsCC4000OStopFixes(void)
{
	lwgpio_set_value(&GPS_PUSH_FIX, LWGPIO_VALUE_LOW);	
//    CC4000_P2F_PORT_OUT &= ~CC4000_P2F_PIN;
//    Board_ledOff(LED2);
}

/*****************************************************************************
* 
* NOTE: End of Platform Independent Section
* 
* ***************************************************************************/


//*****************************************************************************
//
//! \brief Parses standard NMEA GPS Messages and returns the requested parameter.
//!
//! Note: Please provide +1 to paramSize to account for '\0'
//!
//! \param gpsMsg[] is the GPS Message to be parsed
//! \param gpsMsgParam is the position of the parameter to be returned
//! \param returnParam[] a pointer to an array containing returned parameter
//! \param paramSize is the number of characters to return. paramSize-1 characters are
//!                     returned (one '\0' is used)
//!
//! \return None.
//
//*****************************************************************************
void parseGPSMessage(char *gpsMsg, char gpsMsgParam, char *returnParam,
                     char paramSize)
{	
    char i = 0;
    // Initialize local variables
    char *gpsMsgPtr = gpsMsg;
    char gpsMsgParamCtr = 0;
    
    // Check if GPS Message is populated, else skip
    if(strlen(gpsMsg)) 
    {
        // Find location of parameter
        while(gpsMsgParamCtr < gpsMsgParam) 
        {
            if(*gpsMsgPtr == FIELD_DELIMITER)
            {
                gpsMsgParamCtr++;
            }
            gpsMsgPtr++;
        }
        
        // Save parameter in given array
        while( *gpsMsgPtr != FIELD_DELIMITER && 
               *gpsMsgPtr != CHECKSUM_DELIMITER &&
               i < paramSize-1)
        {
            *(returnParam+i) = *(gpsMsgPtr++);
            i++;
        }
        *(returnParam+i) = '\0'; // Set end of string
    }
}


//*****************************************************************************
//
//! \brief Indicates the number of new GSV Messages available.
//! 
//! Note: This function will set the GSAMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b 1-6 depending on # of Msgs available, 
//!         \b OLD_GPS_MSG if none are available
//
//*****************************************************************************
char GSVMsgAvailable(){		
    
    char temp = GSVMsgFlag;
    GSVMsgFlag = 0;
    
    return temp;
}


//*****************************************************************************
//
//! \brief Provides the complete GPS GGA Message.
//!
//! \param None.
//!
//! \return Pointer to GPS GGA message buffer
//
//*****************************************************************************
char *getGGAGPSMessage(void) {
    
    return gpsBuffer.GPGGABuffer;
}


//*****************************************************************************
//
//! \brief Indicates whether a new GPS VTG Message is available.
//! 
//! Note: This function will set the VTGMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new GLL Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char VTGMsgAvailable(){	
    
    if(VTGMsgFlag == NEW_GPS_MSG)
    {
        VTGMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }	
}

//*****************************************************************************
//
//! \brief Provides the complete GPS GLL Message .
//!
//! \param None.
//!
//! \return Pointer to GPS GLL message in buffer.
//
//*****************************************************************************
char *getGLLGPSMessage(void) {
    
    return gpsBuffer.GPGLLBuffer;
}

//*****************************************************************************
//
//! \brief Indicates whether a new GPS GLL Message is available.
//! 
//! Note: This function will set the GLLMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new GLL Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char GLLMsgAvailable(){	
    
    if(GLLMsgFlag == NEW_GPS_MSG)
    {
        GLLMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}

//*****************************************************************************
//
//! \brief Indicates whether a new GPS RMC Message is available.
//! 
//! Note: This function will set the RMCMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new RMC Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char RMCMsgAvailable(){	
    
    if(RMCMsgFlag == NEW_GPS_MSG)
    {
        RMCMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}

//*****************************************************************************
//
//! \brief Provides the complete GPS RMC Message.
//!
//! \param None.
//!
//! \return Pointer to GPS RMC message buffer
//
//*****************************************************************************
char *getRMCGPSMessage(void) {	
    
    return gpsBuffer.GPRMCBuffer;
}

//*****************************************************************************
//
//! \brief Indicates whether a new GPS GGA Message is available.
//! 
//! Note: This function will set the GGAMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new GGA Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char GGAMsgAvailable(){	
    
    if(GGAMsgFlag == NEW_GPS_MSG)
    {
        GGAMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}

//*****************************************************************************
//
//! \brief Provides the complete GPS VTG Message.
//!
//! \param None.
//!
//! \return Pointer to GPS VTG message buffer
//
//*****************************************************************************
char *getVTGGPSMessage(void) {
    
    return gpsBuffer.GPVTGBuffer;
}

//*****************************************************************************
//
//! \brief Provides a complete GPS GSV Message.
//!
//! \param gsvMsgNum is the GPS GSV sentence to provide.
//!
//! \return Pointer to GPS GSV message buffer indicated by gsvMsgNum
//
//*****************************************************************************
char *getGSVGPSMessage(char gsvMsgNum) {
    
    return gpsBuffer.GPGSVBuffer[gsvMsgNum];
}


//*****************************************************************************
//
//! \brief Indicates if there is any error in the GPS Messages.
//!
//! \param None.
//!
//! \return GPS Message Error value,
//!         NO_ERROR if all is ok.
//
//*****************************************************************************
char getGPSMessageError(){
    return gpsMsgError;
}

//*****************************************************************************
//
//! \brief Provides the complete GPS GSA Message.
//!
//! \param None.
//!
//! \return Pointer to GPS GGA message buffer
//
//*****************************************************************************
char *getGSAGPSMessage(void) 
{		
    return gpsBuffer.GPGSABuffer;
}

//*****************************************************************************
//
//! \brief Indicates whether a new GPS GSA Message is available.
//! 
//! Note: This function will set the GSAMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new GSA Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char GSAMsgAvailable(){
    
    if(GSAMsgFlag == NEW_GPS_MSG)
    {
        GSAMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}


//*****************************************************************************
//
//! \brief Indicates whether a new CFWVER Message is available.
//! 
//! Note: This function will set the CFWVER as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new CFWVER Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char CFWVERMsgAvailable(){	
    
    if(CFWVERMsgFlag == NEW_GPS_MSG)
    {
        CFWVERMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}


//*****************************************************************************
//
//! \brief Provides the complete GPS VTG Message.
//!
//! \param None.
//!
//! \return Pointer to GPS VTG message buffer
//
//*****************************************************************************
char *getCFWVERMessage(void) {
    
    return gpsBuffer.CFWVERBuffer;
}

//*****************************************************************************
//
//! \brief Indicates whether a new GPSVER Message is available.
//! 
//! Note: This function will set the GPSVERMsgFlag as OLD_GPS_MSG after it is
//!       called.
//!
//! \param None.
//!
//! \return \b NEW_GPS_MSG if new GPSVER Msg available, 
//!         \b OLD_GPS_MSG otherwise
//
//*****************************************************************************
char GPSVERMsgAvailable(){	
    
    if(GPSVERMsgFlag == NEW_GPS_MSG)
    {
        GPSVERMsgFlag = OLD_GPS_MSG;
        return NEW_GPS_MSG;
    }
    else
    {	
        return OLD_GPS_MSG;	
    }
}

//*****************************************************************************
//
//! \brief Provides the complete GPSVER Message.
//!
//! \param None.
//!
//! \return Pointer to GPS VTG message buffer
//
//*****************************************************************************
char *getGGPSVERMessage(void) {
    
    return gpsBuffer.GPSVERBuffer;
}

//*****************************************************************************
//
//! \brief Provides the Firmware Version.
//!
//! \param fwver[] is an array in which the FW Version will be stored
//! \param fverLength is the number of characters to return, usually 8
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char fwver[9];
//!	getGPSFirmwareVersion(fwver, 9);
//! if(strlen(fwver) > 0)
//!     printf("%s\n",fwver);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! fwver contains "00.80.00"
//! @endcode
//
//*****************************************************************************
void getGPSFirmwareVersion(char *fver, char fverLength){
    
    parseGPSMessage(getCFWVERMessage(), CFWVER_VERSION, fver, fverLength);   
}


//*****************************************************************************
//
//! \brief Provides the SP Version.
//!
//! \param spver[] is an array in which the SP Version will be stored
//! \param spverLength is the number of characters to return, usually 6
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char spver[6];
//!	getGPSSPVersion(spver, 6);
//! if(strlen(spver) > 0)
//!     printf("%s\n",spver);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! fwver contains "21.61"
//! @endcode
//
//*****************************************************************************
void getGPSSPVersion(char *spver, char spverLength){
    
    parseGPSMessage(getGGPSVERMessage(), GPSVER_SPVERSION, spver, spverLength);   
}

//*****************************************************************************
//
//! \brief Reset API flags and buffers
//!
//! \param None.
//!
//! \return None
//!
//
//*****************************************************************************
void resetCC4000API(){
    unsigned char i,j;  	
    
    //
    // Initialize GPS Buffer variables
    //
    for (i = 0; i < GPS_MSG_MAX_SIZE; i++) 
    {
    	gpsBuffer.GPGLLBuffer[i] = '\0';
        gpsBuffer.GPRMCBuffer[i] = '\0';
        gpsBuffer.GPGGABuffer[i] = '\0';
        gpsBuffer.GPVTGBuffer[i] = '\0';
        gpsBuffer.GPGSABuffer[i] = '\0';		
        gpsBuffer.CFWVERBuffer[i] = '\0';
        gpsBuffer.GPSVERBuffer[i] = '\0';
        
	
        for (j = 0; j < GPS_GSV_MAX_MSGS; j++)
        {
            gpsBuffer.GPGSVBuffer[j][i] = '\0';
        }
        gpsTempBuffer[i] = '\0';
    }
    
    ptigcdCount = 0;
    
    gpsGSVBufferPtr = 0;
    gpsTempBufferPtr = 0;
    
    for (i = 0; i < GPS_CHECKSUM_SIZE; i++)
    {
        gpsChecksumBuffer[i] =  '\0';  	
    }
    gpsChecksumBufferPtr = 0;
    
    gpsMsgError = NO_ERROR;
    
    //
    // Initialize GPS State Machine
    //
    gpsSMState = GPS_MSG_START;
    gpsBufferUpdate = OLD_GPS_MSG;
    
    
    GLLMsgFlag = OLD_GPS_MSG;
    RMCMsgFlag = OLD_GPS_MSG;
    GGAMsgFlag = OLD_GPS_MSG;
    VTGMsgFlag = OLD_GPS_MSG;
    GSAMsgFlag = OLD_GPS_MSG;
    GSVMsgFlag = OLD_GPS_MSG;
    CFWVERMsgFlag = OLD_GPS_MSG;
    GPSVERMsgFlag = OLD_GPS_MSG;
}

//*****************************************************************************
//
//! \brief Checks for CC4000 Status using the Fix pin
//!
//! \param None.
//!
//! \return CC4000_INITIALIZING if currently initializing
//!         CC4000_ACQUIRING if currently acquiring satellites
//!         CC4000_TRACKING if currently tracking satellites
//!         CC4000_STATUS_FAIL if failed to find status
//!
//! \par Note: This function is blocking and may take 50ms to 2s
//!
//
//*****************************************************************************
char getCC4000Status()
{
    char result = CC4000_STATUS_FAIL;
    
    
//    CC4000_FIX_PORT_DIR &= ~CC4000_FIX_PIN;
//    CC4000_FIX_PORT_SEL &= ~CC4000_FIX_PIN;
    
    while(result == CC4000_STATUS_FAIL)
    {
        
        unsigned char firstStatus = 0;
        unsigned char finalStatus = 0;
        firstStatus=lwgpio_get_value(&FIX_AVL);
        Time_Delay_Sleep(150);
//        firstStatus = CC4000_FIX_PORT_IN & CC4000_FIX_PIN;
        
        
        // Delay for ~150ms
        // Start timing till the transition
//        TA2CCR0 = 4915;
//        TA2CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK ~32768Hz, contmode, clear TAR	
//        while(!(TA2CTL & TAIFG));
        
        
        // Check FIX line now
//        finalStatus = CC4000_FIX_PORT_IN & CC4000_FIX_PIN;
        finalStatus=lwgpio_get_value(&FIX_AVL);   
        
        // Is FIX STATUS HIGH?
        if(firstStatus == 1 && finalStatus == 1)
        {
            // We're tracking     
            result = CC4000_TRACKING;
            return result;
            
        }
        else if(firstStatus == 1 || finalStatus == 1)
        {
            result = CC4000_ACQUIRING;
            return result;
        }
        else if(firstStatus == 0 && finalStatus == 0)
        {
            result = CC4000_INITIALIZING;
            return result;
        }              
    }
    return CC4000_STATUS_FAIL;
}

