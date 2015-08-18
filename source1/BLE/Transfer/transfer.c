/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: transfer.c
*
* Abstract: This file contains the main API definitions for bulk transfers.
*           Several GATT chars are created, and APIs are included to help hide
*           the details of it's implementation, leaving only a clean interface
*           to customer facing APIs.
*
*******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "BLE/Common/api.h"
#include "BLE/Common/msg.h"
#include "BLE/Common/os.h"
#include "Bulk_transfer.h"
#include "common_headers.h"

#define UUID128(x) {0x45, 0xdd, 0x67, 0x92, 0xbf, 0x42, 0x2e, 0xa4, 0x77, 0x4e, 0x36, 0xe7, ((UINT8)(x & 0x00ff)), ((UINT8)(x >> 8)), 0xe7, 0xd7}

static TRANSFER_CB _transferCb_p;

static UINT16 _rxBufHandle = 0x0000;
static UINT16 _txBufHandle = 0x0000;

static UINT8 *_rxData;
static UINT32 _rxDataMaxSize;
static UINT32 _rxDataWriteIndex;
static UINT32 _rxDataSize;
static UINT32 _rxDataBytesRcvd;

int _transferCallback(UINT16 handle, UINT8 *buf, int len);

int iDevices_createTransferService(void)
{
	
#define DEFAULT_CONF 1
	
    UINT16 rxHandle;
    // iDevices Mass transfer service
    UINT8 srvc_uuid[16] = UUID128(0x0000);
    iDevices_GattAddService((UINT8 *)srvc_uuid, 16, &rxHandle);

    // Tx buffer
    UINT8 tx_buffer_char_uuid[16] = UUID128(0x0001);
    
#if !DEFAULT_CONF
    
 iDevices_GattAddChar((UINT8 *)tx_buffer_char_uuid, 16, PERM_READABLE, 0, &rxHandle);
#else
 iDevices_GattAddChar((UINT8 *)tx_buffer_char_uuid, 16, PERM_READABLE, PROP_NOTIFY, &rxHandle);
#endif

    UINT8 tx_buffer_val[22];
    memset(tx_buffer_val, 0, 22);
    iDevices_GattAddCharVal((UINT8 *)tx_buffer_char_uuid, 16, 0x0, tx_buffer_val, 22, &rxHandle);
    _txBufHandle = rxHandle; // store away for later
    iDevices_GattAddCharDesc(0x2902, 0, NULL, 0, &rxHandle);

    // Rx buffer
    UINT8 rx_buffer_char_uuid[16] = UUID128(0x0002);
    
#if !DEFAULT_CONF 
	iDevices_GattAddChar((UINT8 *)rx_buffer_char_uuid, 16, PERM_READABLE, PROP_WRITE_CMD, &rxHandle);
#else	
	iDevices_GattAddChar((UINT8 *)rx_buffer_char_uuid, 16, PERM_READABLE, PROP_WRITE_REQ, &rxHandle); // WriteWithRsp
#endif
	
    UINT8 rx_buffer_val[20];
    memset(rx_buffer_val, 0, 20);
    

#ifndef DEFAULT_CONF    
	iDevices_GattAddCharVal((UINT8 *)rx_buffer_char_uuid, 16, 0x0|PERM_VARIABLE_LENGTH
                                                                 |PERM_AUTH_WRITABLE
																 |PERM_WRITE_CMD, rx_buffer_val, 20, &rxHandle);

#else
    iDevices_GattAddCharVal((UINT8 *)rx_buffer_char_uuid, 16, 0x0|PERM_VARIABLE_LENGTH
                                                                 |PERM_AUTH_WRITABLE
                                                                 |PERM_WRITE_REQ
                                                                 |PERM_RELIABLE_WRITE, rx_buffer_val, 20, &rxHandle);
#endif
    _rxBufHandle = rxHandle; // store away for later
    
    iDevices_GattCommitNewDb();
    return IDEV_SUCCESS;
}

int iDevices_initTransferService(UINT16 maxBufferSize, TRANSFER_CB transferCb)
{
    _rxDataMaxSize = maxBufferSize;
    _rxData = (UINT8 *)malloc(maxBufferSize);
    memset(_rxData, 0, maxBufferSize);

    _transferCb_p = transferCb;

    UINT16 handle;
    UINT8 tx_buffer_char_uuid[16] = UUID128(0x0001);
    iDevices_GattGetHandleFromUUID(tx_buffer_char_uuid, 16, &handle);

//	iDevices_createTransferService();

    // if service doesn't already exist, create it
    if (handle == 0x0000) {
        iDevices_createTransferService();
    }

    // retrieve handles if needed
    if (!_txBufHandle) {
        UINT8 tx_buffer_char_uuid[16] = UUID128(0x0001);
        iDevices_GattGetHandleFromUUID(tx_buffer_char_uuid, 16, &handle);
        _txBufHandle = handle;
    }

    if (!_rxBufHandle) {
        UINT8 rx_buffer_char_uuid[16] = UUID128(0x0002);
        iDevices_GattGetHandleFromUUID(rx_buffer_char_uuid, 16, &handle);
        _rxBufHandle = handle;
    }

    iDevices_RegisterInternalCallback(_rxBufHandle, _transferCallback);
    return IDEV_SUCCESS;
}

int iDevices_sendBulkData(UINT8 *buf, UINT32 len)
{
//TODO:	Modify txLen if necessary
    int txLen = 20;
//	int txLen = 15;
    int txBytesLeft = len;
    UINT8 msg[22];
    memset(msg, 0, 22);

    msg[0] = TRANSFER_START;
    *((UINT32 *)(&msg[1])) = len;
    if(iDevices_GattWriteHandle(_txBufHandle, msg, 5)== -1)
    {
    	return -1;
    }

    msg[0] = TRANSFER_DATA;
    while (txBytesLeft) {
        if (txBytesLeft < 20) txLen = txBytesLeft;
        msg[1] = txLen;
        memcpy(&msg[2], &buf[len-txBytesLeft], txLen);
        if(iDevices_GattWriteHandle(_txBufHandle, msg, txLen+2) == -1)
        {
        	return -1;
        }
        txBytesLeft-=txLen;
        
		 if(Button_Press_Status == LEFT_BUTTON_PRESSED)
		 {
//			 printf("\nFile Sending cancelled\n");
			 Button_Press_Status = BUTTON_PRESS_NOT_PENDING;
			 msg[0] = TRANSFER_STOP;
			 iDevices_GattWriteHandle(_txBufHandle, msg, 1);
			 return 1;
		 }
		 if( ble_transmission_status == BLUETOOTH_DEVICE_NOT_CONNECTED)
		 {
			 return -1;
		 }
    }

    msg[0] = TRANSFER_STOP;

	if(iDevices_GattWriteHandle(_txBufHandle, msg, 1) == -1)
	{
		return -1;
	}

    return IDEV_SUCCESS;
}

int _transferCallback(UINT16 handle, UINT8 *buf, int len)
{
	len = len-2;
	
    if (buf[0] == TRANSFER_START) {
        _rxDataWriteIndex = 0;
        _rxDataBytesRcvd = 0;
        _rxDataSize = LE_SWAP_32(*((UINT32 *)&buf[1]));
        DEBUG("starting file of %d bytes\n", _rxDataSize);
        if (_transferCb_p) {
            _transferCb_p(TRANSFER_START, NULL, _rxDataSize);
        }
    } else if (buf[0] == TRANSFER_DATA) {
        int bytesLeft = _rxDataMaxSize - _rxDataWriteIndex;
        if (bytesLeft > (len-2)) {
            memcpy(&_rxData[_rxDataWriteIndex], &buf[2], len-2);
            _rxDataWriteIndex += (len-2);
            _rxDataBytesRcvd+=(len-2);
        } else {
            memcpy(&_rxData[_rxDataWriteIndex], &buf[2], bytesLeft);
            _rxDataWriteIndex += (bytesLeft);
            _rxDataBytesRcvd+=(bytesLeft);
            if (_transferCb_p) {
                if (_rxDataWriteIndex)
                _transferCb_p(TRANSFER_DATA, _rxData, _rxDataWriteIndex);
            }
            _rxDataWriteIndex = 0;
            memcpy(&_rxData[_rxDataWriteIndex], &buf[2], len-2-bytesLeft);
            _rxDataWriteIndex += (len-2-bytesLeft);
            _rxDataBytesRcvd += (len-2-bytesLeft);
        }
#if 1
        int i;
        DEBUG("TRANSFER_DATA(%d bytes):", len-2);
        for (i = 0; i < len-2; i++) {
            DEBUG("%02x ", buf[2+i]);
        }
        DEBUG("\n");
#endif
    } else if (buf[0] == TRANSFER_STOP) {
        DEBUG("done receiving file\n");
        if (_transferCb_p) {
            _transferCb_p(TRANSFER_STOP, _rxData, _rxDataWriteIndex);
        }
    }

    return 0;
}

void Clear_BLE_rxn_buff(void)
{
	free(_rxData);
//	if(_rxData == NULL)
//	{
//		printf("\nBLE rxn Buffer free Success..\n");
//	}
//	else
//	{ 	
//		printf("\nBLE rxn Buffer free Failed..!\n");
//	}
}
