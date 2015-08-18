/*
 * Ble_uart.c
 *
 *  Created on: May 28, 2014
 *      Author: user
 */


#include "Ble_uart.h"
#include "os.h"
#include "api.h"
#include "common_headers.h"
//#include <Windows.h>

#define PPPINITFCS16    0xffff  /* Initial FCS value */
#define PPPGOODFCS16    0xf0b8  /* Good final FCS value */

int osUartWrite(UINT8 byte);
int osUartRead(UINT8 *byte_p);

#define UART_WRITE(x) osUartWrite(x)
#define UART_READ(x)  osUartRead(x)

uint_16 BLE_tx_buf_count = 0;

UART_MemMapPtr ble_sci_ptr;	
static uint_8 uart_wait_flag = 0;

LWEVENT_STRUCT    BLE_process_event;
LWEVENT_STRUCT    BLE_txn_event;

static void Wait_for_BLE_process_event (void);

/****************************************************************
 * Checksum code
 ****************************************************************/
/*
 * FCS lookup table as calculated by the table generator.
 */
const UINT16 fcstab[256] = {
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};



/*
 * Calculate a new fcs given the current fcs and the new data.
 */
static UINT16 _pppfcs16(UINT16 fcs, unsigned char *cp, int len)
{
	while (len--)
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

	return (fcs);
}

#define PUART_MAX_MSG_SIZE   (64)
#define PUART_READ_UART_LEN  (((PUART_MAX_MSG_SIZE+2)*2)+1+1)
#define PUART_MAX_MSG_LEN    (PUART_MAX_MSG_SIZE+1)

UINT8 buf_tx[PUART_READ_UART_LEN];

enum {
	RX_IDLE = 0,
	RX_GATHER_DATA,
	RX_PROCESS_DATA,
} rxState_e;

PUART_CB_FUNC _cbFuncPtr;

#ifndef CONFIGURABLE_SLEEP_MODE
static BOOL _wakeUpModuleHandshake = YES; // hardcode to YES for now
#else
static BOOL _wakeUpModuleHandshake = NO; // default to NO
#endif

#define BLE_CHANNEL "ittyd:"	// UART3 
MQX_FILE_PTR ble_uart_fd = NULL;




/*-----------------------------------------------------------------------------* 
 * Function:    Ble_uart_init
 * Brief:       This function initialize uart for BLE module
 * Parameter:   None
 * Return:      None
 -----------------------------------------------------------------------------*/
uint_8 Ble_uart_init(void)
{
//	printf("Initialising UART BLE ...\n");	
	
	char flags =IO_SERIAL_NON_BLOCKING;	

	printf("\nInitialising UART BLE ...\n");	
	
	ble_uart_fd  = fopen( BLE_CHANNEL, NULL );  
	
	if( ble_uart_fd == NULL )
	{
		/* device could not be opened */
		printf("\n UART BLE init Failed..!");
		return 1;
	}   
	
	ioctl(   ble_uart_fd, IO_IOCTL_SERIAL_SET_FLAGS, &flags);	
	_nvic_int_init(INT_UART3_RX_TX, 2, TRUE );
	_int_install_isr(INT_UART3_RX_TX, handle_PUART_RxIntCb, ble_uart_fd);
	_nvic_int_enable(INT_UART3_RX_TX);
	_bsp_serial_io_init(3, IO_PERIPHERAL_PIN_MUX_ENABLE);
	
	ioctl(   ble_uart_fd, IO_IOCTL_SERIAL_SET_BAUD, &BLE_curr_baud);
//	printf("\nBLE uart Baud rate %d",BLE_curr_baud);	
//	
//	printf("\n UART BLE init Success..!");	
	return 0;	
	
	//DES get base address for UART0	
}

uint_8 Ble_uart_deinit(void)
{
	if (MQX_OK == fclose(ble_uart_fd))
		{
			printf("\n BLE uart de-inint Success..\n");
			return 0;
		}
		else
		{
			printf("\n BLE uart de-inint Error..\n");
			return 1;
		}
}

int BLE_UartWrite(void)
{
	UART_MemMapPtr ble_sci_ptr_tx;		
	
	ble_sci_ptr_tx = _bsp_get_serial_base_address(3);
	
	/* Wait until space is available in the FIFO */
	while(!( (ble_sci_ptr_tx->S1) & UART_S1_TDRE_MASK))
	{
		
	}
	
	/* Send the character */
	ble_sci_ptr_tx->D = buf_tx[0];
	
	/*Enable Tx interrupt*/
	ble_sci_ptr_tx->C2 |= UART_C2_TIE_MASK;

	return 1;
}




int osUartWrite(UINT8 byte)
{
	
//	write( ble_uart_fd, &byte, 1);	
//	
//	
//	   KUART_INFO_STRUCT_PTR                  sci_info_ptr;
//	   UART_MemMapPtr                         sci_ptr;
//
//	   sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
//	   sci_ptr = sci_info_ptr->SCI_PTR;
//
//	   while (!(sci_ptr->S1 & UART_S1_TDRE_MASK)) {
//	      /* Wait while buffer is full */
//	   } /* Endwhile */
//
//	   sci_ptr->D = c;
//	   sci_info_ptr->TX_CHARS++;
//
//	   /* Enable transmit iterrupt */
//	   sci_ptr->C2 |= UART_C2_TIE_MASK;
	   

	UART_MemMapPtr ble_sci_ptr_tx;		
	ble_sci_ptr_tx = _bsp_get_serial_base_address(3);
	
	/* Wait until space is available in the FIFO */
	while(!( (ble_sci_ptr_tx->S1) & UART_S1_TDRE_MASK))
	{
		
	}
	/* Send the character */
	ble_sci_ptr_tx->D =byte;
	
	/*Enable Tx interrupt*/
	ble_sci_ptr_tx->C2 |= UART_C2_TIE_MASK;

	return 1;
}


// returns 0 on failure, and 1 on success
int osUartRead(UINT8 *byte_p)
{

	if(read(ble_uart_fd,byte_p,1) <= 0)
	{
		return 0;
	}
	return 1;
}


static void _processData(UINT8 *data_p, int len)
{
	//DEBUG("processData():");

	// remove byte stuffing
	UINT8 buf[PUART_MAX_MSG_LEN];

	int rIndex=0;
	int wIndex=0;
	while (rIndex < len) {
		if (data_p[rIndex] == 0x7d) {
			rIndex++;
			buf[wIndex++] = (data_p[rIndex++]^0x20);
		} else {
			buf[wIndex++] = data_p[rIndex++];
		}
	}

	UINT16 calcFcs = _pppfcs16(PPPINITFCS16, buf, wIndex-2);
	UINT16 fcs = _pppfcs16(PPPINITFCS16, buf, wIndex);
	if (fcs == PPPGOODFCS16) {
		//DEBUG("processData(): FCS16 is Good");

		// call user handler
		if (_cbFuncPtr) {
			_cbFuncPtr(buf, wIndex-2);
		}
	} else {
#if 0
		int i;
		printf("_processData():");
		for (i = 0; i < wIndex; i++) {
			printf("%02x ", buf[i]);
		}
		printf("\n");
#endif
		DEBUG("processData(): FCS16 BAD: calcFcs = 0x%04x, fcs = 0x%04x", calcFcs^0xffff, fcs);
	}
}

void registerUARTCallback(PUART_CB_FUNC userfn)
{
	_cbFuncPtr = userfn;
}

#ifdef CONFIGURABLE_SLEEP_MODE
int enableWakeUpModuleHandshake(BOOL enable)
{
	_wakeUpModuleHandshake = enable;
}
#endif


int _performWakeUpModuleHandshake(void)
{
	// set pin high
//	DEBUG("\nsetting WAKE_UP_MODULE_PIN = 1");
	SET_WAKUP_MODULE_PIN(ON);
//    Time_Delay_Sleep(30);
	int ctsActive = 0;
	uint_32 j= 10000000;
//	_time_delay(50);
	// wait here for 1ms to allow host to wake up before sending UART data	
	while ((!ctsActive) && ( j > 0)) 
	{
		ctsActive = POLL_WAKUP_HOST_PIN();
//		j--;
//		ctsActive = 1;
	}
	
	// set pin low
//	DEBUG("\nsetting WAKE_UP_MODULE_PIN = 0");
	SET_WAKUP_MODULE_PIN(OFF);

	if (!ctsActive) {
		DEBUG("\nctsActive TIMEOUT, not sending message...");
		return -1;
	} else {
//		DEBUG("\nresponse within %d uS\n", (10000-j)*50);
	}
	return 0;
}

int sendData(UINT8 *data_p, int len)
{
	uart_wait_flag = 1;
	DEBUG("\nsendData():");
//	UINT8 buf[PUART_READ_UART_LEN];
	buf_tx[0] = 0x7e;
	// calculate FCS prior to byte stuffing
	UINT16 fcs = _pppfcs16(PPPINITFCS16, data_p, len)^0xffff;
	int bufIdx = 1;
	int i;
	for (i = 0; i < len+2; i++) {
		UINT8 data = data_p[i];
		if (i == len) {
			data = (fcs & 0xff);
		} else if (i == len+1) {
			data = (fcs>>8);
		}
		if ((data == 0x7e) || (data == 0x7d)) {
			buf_tx[bufIdx++] = 0x7d;
			buf_tx[bufIdx++] = (data^0x20);
		} else {
			buf_tx[bufIdx++] = data;
		}
	}
	buf_tx[bufIdx++] = 0x7e;

	if (_wakeUpModuleHandshake) {
		if (_performWakeUpModuleHandshake() < 0) {
			return IDEV_ERR_MODULE_NOT_READY;
		}
	}

	BLE_tx_buf_count = bufIdx;
	BLE_UartWrite();
	
//	for (i = 0; i < bufIdx; i++) {
////		printf("%02X ", buf[i]);
////		printf( (char *)buf[i]);
//		UART_WRITE(buf[i]);
//	}
////	printf("\n");

	
	return IDEV_SUCCESS;
}



static UINT8 _rxBuf[PUART_READ_UART_LEN];
static int _rxState = RX_IDLE;
static int _rxIndex = 0;
static int _TxIndex = 1;

void handle_PUART_RxIntCb(pointer )
{
	//DEBUG("handle_PUART_RxIntCb(): rxState = %d", _rxState);
	//DEBUG("s = %d", _rxState);
	UINT8 buf;
	
	
	ble_sci_ptr = _bsp_get_serial_base_address(3);
	
	if(( (ble_sci_ptr->S1) & UART_S1_TDRE_MASK) && BLE_tx_buf_count)
	{
		
		ble_sci_ptr->D = buf_tx[_TxIndex++];
		if(_TxIndex >= BLE_tx_buf_count)
		{
			ble_sci_ptr->C2 = (ble_sci_ptr->C2 & ~UART_C2_TIE_MASK);
			_TxIndex = 1;
			BLE_tx_buf_count = 0;
			buf = ble_sci_ptr->S1;
		}
//		buf = ble_sci_ptr->S1;

	}
	else
	{
		buf = ble_sci_ptr->S1; //DES read status register.  Part of two step process to clear the interrupt flag. 
		buf = ble_sci_ptr->D; 

		switch(_rxState) {
		case RX_IDLE:

			//DEBUG("RX_IDLE: puart_read(): buf = 0x%02x", buf);
			if (buf == 0x7e) {
				_rxIndex = 0; // reset buffer index
				_rxState = RX_GATHER_DATA;
			} 
			else {
				//DEBUG("RX_IDLE: received 0x%02x, try again...", buf);
			}
			// let's go again

			break;
		case RX_GATHER_DATA:
		{
			//DEBUG("RX_GATHER_DATA: puart_read(): buf = 0x%02x", buf);
			if (buf == 0x7e) {
				if (_rxIndex < 3) { // check for a minimum of 3 bytes (1 byte msg + 2 byte FCS)
					_rxIndex = 0; // assume we hit another start flag, and start over
				} else {
					_rxState = RX_PROCESS_DATA;

///					_processData(_rxBuf, _rxIndex);
//					_rxState = RX_IDLE;
//##################################################################
					_lwevent_set(&BLE_process_event, SW_EVENT_MASK);

				}
				// let's go again
				break;
			} else {
				_rxBuf[_rxIndex++] = buf;
			}

			break;
		}
		case RX_PROCESS_DATA:
			//		_processData(_rxBuf, _rxIndex);
			//		_rxState = RX_IDLE;
			// let's go again to make sure the FIFO is cleaned out
			break;
		default:
			DEBUG("invalid rxState = %d", _rxState);
			break;
		}
	}
	

	
}


//void handle_PUART_RxIntCb(void)
//{
//	//DEBUG("handle_PUART_RxIntCb(): rxState = %d", _rxState);
//	//DEBUG("s = %d", _rxState);
//	UINT8 buf;
//	int done;
//	do {
//		done = 1;
//		switch(_rxState) {
//		case RX_IDLE:
//			if (UART_READ(&buf)) {
//				//DEBUG("RX_IDLE: puart_read(): buf = 0x%02x", buf);
//				if (buf == 0x7e) {
//					_rxIndex = 0; // reset buffer index
//					_rxState = RX_GATHER_DATA;
//				} else {
//					//DEBUG("RX_IDLE: received 0x%02x, try again...", buf);
//				}
//				done = 0; // let's go again
//			}
//			break;
//		case RX_GATHER_DATA:
//		{
//			while(UART_READ(&buf)) {
//				//DEBUG("RX_GATHER_DATA: puart_read(): buf = 0x%02x", buf);
//				if (buf == 0x7e) {
//					if (_rxIndex < 3) { // check for a minimum of 3 bytes (1 byte msg + 2 byte FCS)
//						_rxIndex = 0; // assume we hit another start flag, and start over
//					} else {
//						_rxState = RX_PROCESS_DATA;
//					}
//					done = 0; // let's go again
//					break;
//				} else {
//					_rxBuf[_rxIndex++] = buf;
//				}
//			}
//			break;
//		}
//		case RX_PROCESS_DATA:
//			_processData(_rxBuf, _rxIndex);
//			_rxState = RX_IDLE;
//			done = 0; // let's go again to make sure the FIFO is cleaned out
//			break;
//		default:
//			DEBUG("invalid rxState = %d", _rxState);
//			break;
//		}
//	} while (!done);
//}


/*-----------------------------------------------------------------------------* 
 * Function:    Wait_for_BLE_process_event
 * Brief:       This function waits until a BLE event.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Wait_for_BLE_process_event (void)
{
	_mqx_uint   app_event_value;

	/* Wait for button press */
	_lwevent_wait_ticks (&BLE_process_event, (SW_EVENT_MASK), FALSE, 0);

	/* Copy event masks into temporary variable */
	app_event_value = _lwevent_get_signalled();

	/* If button pressed or switch expires, moving to next clock configuration */
	if (app_event_value & SW_EVENT_MASK)
	{		
//	       printf("\nBLE_process_event Generated\n");
	}
	
}


/*-----------------------------------------------------------------------------* 
 * Function:    Wait_for_BLE_process_event
 * Brief:       This function waits until a BLE event.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
static void Wait_for_BLE_txn_event (void)
{
	_mqx_uint   app_event_value;

	/* Wait for button press */
	_lwevent_wait_ticks (&BLE_txn_event, (SW_EVENT_MASK), FALSE, 0);

	/* Copy event masks into temporary variable */
	app_event_value = _lwevent_get_signalled();

	/* If button pressed or switch expires, moving to next clock configuration */
	if (app_event_value & SW_EVENT_MASK)
	{		
//	       printf("\nBLE_process_event Generated\n");
	}
	
}


/*-----------------------------------------------------------------------------* 
 * Function:    BLE_Process_Task
 * Brief:       This Tasks reads BLE data.
 * Parameter:   Task ID
 * Return:      None
 -----------------------------------------------------------------------------*/
void BLE_Process_Task(uint_32)
{
	char ble_conn_stat = 0;
	
//	TIME_STRUCT start_time, end_time, diff_time;
	
    /* Create global event */
    if (_lwevent_create(&BLE_process_event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }
    
	while(1)
	{
		Wait_for_BLE_process_event();

		if(BLE_host_wakeup_flag == YES)
		{	
			BLE_host_wakeup_flag = NO;
			_rxState = RX_IDLE;	

			if(Core_clock_selection == LOW_POWER_MODE)
			{
				run_mode_for_BLE();				
			}

			
			Prev_State_of_Screen = State_of_Screen;			
			State_of_Screen = UI_BLUETOOTH_MAIN;
//			lwgpio_set_value(&SD_EN, LWGPIO_VALUE_HIGH);
							//Next_display_creat_function
		    ble_transmission_status = BLUETOOTH_DEVICE_CONNECTED;
			
		}
		else
		{
			_processData(_rxBuf, _rxIndex);	//		_nvic_int_enable(INT_UART3_RX_TX);	
			_rxState = RX_IDLE;
			BLE_host_wakeup_flag = NO;
		}

		
	}

}

int osUartWait(void)
{	
	uint32_t ctr=0;
	
	while(1)
	{		
		if(uart_wait_flag == 0)
		{
			break;
		}
		else
		{
			if(ctr++ >= 2000000)
			{				
				ctr = 0;
				printf("Timeout\n");
				return -1;
			}
		}
	}	
	return 0;
	
}

int  osUartPost(void)
{
	uart_wait_flag = 0;
	return 0;
}
