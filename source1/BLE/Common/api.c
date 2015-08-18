/*
 * api.c
 *
 *  Created on: May 29, 2014
 *      Author: user
 */


/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: api.c
*
* Abstract: This file contains the main API definitions for the iDevices FW.
*           The functions convert the minimal input values into the custom
*           message format defined over the HDLC-based UART protocol.
*
*******************************************************************************/

//#include <string.h>
//#include <stdlib.h>

#include <mqx.h>
#include <bsp.h>
#include "config.h"
#include "BLE/Common/api.h"
#include "msg.h"
#include "os.h"
#include "Bulk_transfer.h"

//#define MAX_DATA_SIZE 22
//#define MAX_ADV_SIZE 31

static int _msgId;
static UINT8 _rxBuf[MAX_DATA_SIZE];
static UINT8 _rxBufLen;
static idevCb_t _cbTable;

uint_8 idevice_ready =NO;

static UINT16 _mtHandle;
static CHAR_UPDATED_CB _mtCb;

int _HandleRxMsg(UINT8 *data_p, int len);

int iDevices_Init(idevCb_t *cb_p)
{
	int status = IDEV_SUCCESS;
    if (cb_p) {
        _cbTable = *cb_p;
    }
    registerUARTCallback(_HandleRxMsg);
    return status;
}

int iDevices_Reset(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_RESET;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    return status;
}

int iDevices_FactoryReset(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_FACTORY_RESET;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    return status;
}

int iDevices_Sleep(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_SLEEP;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    return status;
}

int iDevices_RuntimeSleepMode(BOOL enable)
{
    if ((enable < 0) || (enable > 1))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    msg_t msg;
    msg.hdr.msgId = _msgId++;
    msg.hdr.cmd = CMD_RUNTIME_SLEEP_MODE;
    msg.data = (UINT8)enable;
    sendData((UINT8 *)&msg, sizeof(msg));
    READ_WAIT();
    if (_rxBuf[0]) {
        status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
    }
    return status;
}

int iDevices_SetBaudrate(UINT32 baudrate)
{
    if (!((baudrate >= 9600) && (baudrate <= 115200))) {
        return IDEV_ERR_PARAM_OUT_OF_RANGE;
    }

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT32);
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_SET_BAUDRATE;
    UINT8 *data_p = &msg_p->data;
    *((UINT32 *)data_p) = LE_SWAP_32(baudrate);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);  
    return status;
}

int iDevices_GetBDAddr(UINT8 *bdAddr)
{
    if (!bdAddr)
        return IDEV_ERR_NULL_POINTER;

    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_GET_BD_ADDR;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBufLen != 6) {
            return IDEV_ERR_GENERAL_ERROR;
        }
        MEMCPY(bdAddr, _rxBuf, _rxBufLen);
    }
    return status;
}

// if advData is NULL, default adv values are generated within iDevices FW
int iDevices_SetAdvData(UINT8 *advData, int len, BOOL encryptedMfgData)
{
    if ((len < 0) || (len > MAX_ADV_SIZE))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_SET_ADV_DATA;
    UINT8 *data_p = &(msg_p->data);
    *(data_p++) = (UINT8)encryptedMfgData;
    if (advData && (len > 0)) {
        MEMCPY(data_p, advData, len);
    }
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

// NOTE: value saved to EEPROM, and doesn't take affect until next reboot
// This is a limitation of the Broadcom stack, and will remain this way in
// this library until fixed in the stack.
int iDevices_SetScanRspData(UINT8 *scanRspData, int len)
{
    if ((len < 0) || (len > MAX_ADV_SIZE))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_SET_SCAN_RSP_DATA;
    UINT8 *data_p = &(msg_p->data);
    if (scanRspData && (len > 0)) {
        MEMCPY(data_p, scanRspData, len);
    }
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

int iDevices_SetUndirAdvParam(UINT16 hiInterval, UINT16 hiDuration, UINT16 loInterval, UINT16 loDuration)
{
    // 0xa0 is min for non-conn adv
    if ((hiInterval < 0x0020) || (hiInterval > 0x4000))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    if ((hiDuration < 0) || (hiDuration > 3600)) // limit to an hour
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    // 0xa0 is min for non-conn adv
    if ((loInterval < 0x20) || (loInterval > 0x4000))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    if ((loDuration < 0) || (loDuration > 3600)) // limit to an hour
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT16)+sizeof(UINT16)+sizeof(UINT16)+sizeof(UINT16);
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_SET_UNDIR_ADV_PARAM;
    UINT16 *data_p = (UINT16 *)(&msg_p->data);
    *(data_p++) = LE_SWAP_16(hiInterval);
    *(data_p++) = LE_SWAP_16(hiDuration);
    *(data_p++) = LE_SWAP_16(loInterval);
    *(data_p++) = LE_SWAP_16(loDuration);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

int iDevices_SetAdvEnable(int enable, int isConnectable)
{
    if ((enable < 0) || (enable > 1))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+sizeof(UINT8);
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_ADV_ENABLE;
    UINT8 *data_p = (UINT8 *)(&msg_p->data);
    *(data_p++) = (UINT8)enable;
    *(data_p++) = (UINT8)isConnectable;
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

int iDevices_SetTimedAdv(int enable)
{
    if ((enable < 0) || (enable > 1))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    msg_t msg;
    msg.hdr.msgId = _msgId++;
    msg.hdr.cmd = CMD_SET_TIMED_ADV;
    msg.data = (UINT8)enable;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

int iDevices_GetAdvState(UINT8 *state)
{
    if (!state)
        return IDEV_ERR_NULL_POINTER;

    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_GET_ADV_STATE;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBufLen != 1) {
            return IDEV_ERR_GENERAL_ERROR;
        }
        MEMCPY(state, _rxBuf, _rxBufLen);
    }
    return IDEV_SUCCESS;
}

int iDevices_GetConnState(UINT8 *state)
{
    if (!state)
        return IDEV_ERR_NULL_POINTER;

    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_GET_CONN_STATE;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBufLen != 1) {
            return IDEV_ERR_GENERAL_ERROR;
        }
        MEMCPY(state, _rxBuf, _rxBufLen);
    }
    return IDEV_SUCCESS;
}

// FIXME: Broadcom stack doesn't give valid return code for this asynchronous request, thus we return success
//        if we delivered the parameters to the HCI layer, but have no way of verifying if they were accepted
//        other than calling iDevices_GetConnParams() afterwards to check the current values
int iDevices_UpdateConnParams(UINT16 minInterval, UINT16 maxInterval, UINT16 slaveLatency, UINT16 timeoutMultiplier)
{
    // units are 2x timeslots (minInterval*1.25ms)
    if ((minInterval < 6) || (minInterval > 3200))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    // units are 2x timeslots (maxInterval*1.25ms)
    if ((maxInterval < 6) || (maxInterval > 3200))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    // units are in 10's of ms (timeoutMultiplier*10ms)
    if ((timeoutMultiplier < 10) || (timeoutMultiplier > 3200))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    if ((slaveLatency < 0) || (slaveLatency > (((timeoutMultiplier*10)/maxInterval)-1)))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT16)+sizeof(UINT16)+sizeof(UINT16)+sizeof(UINT16);
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_UPDATE_CONN_PARAMS;
    UINT8 *data_p = &msg_p->data;
    *((UINT16 *)data_p) = LE_SWAP_16(minInterval);
    data_p+=sizeof(UINT16);
    *((UINT16 *)data_p) = LE_SWAP_16(maxInterval);
    data_p+=sizeof(UINT16);
    *((UINT16 *)data_p) = LE_SWAP_16(slaveLatency);
    data_p+=sizeof(UINT16);
    *((UINT16 *)data_p) = LE_SWAP_16(timeoutMultiplier);
    data_p+=sizeof(UINT16);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

int iDevices_GetConnParams(UINT16 *connInterval, UINT16 *slaveLatency, UINT16 *timeout)
{
    if (!connInterval || !slaveLatency || !timeout)
        return IDEV_ERR_NULL_POINTER;

    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_GET_CONN_PARAMS;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBufLen != 6) {
            return IDEV_ERR_GENERAL_ERROR;
        }
        UINT16 *data_p = (UINT16 *)(&_rxBuf[0]);
        *connInterval = LE_SWAP_16(*(data_p++));
        *slaveLatency = LE_SWAP_16(*(data_p++));
        *timeout      = LE_SWAP_16(*(data_p++));
    }
    return IDEV_SUCCESS;
}

int iDevices_Disconnect(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_DISCONNECT;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

// NOTE: AES key is big-endian
int iDevices_SetEncryptionKey(UINT8 *key)
{
    if (!key)
        return IDEV_ERR_NULL_POINTER;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+16;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_SET_ENCRYPTION_KEY;
    MEMCPY(&msg_p->data, key, 16);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

int iDevices_SetAuthenticationEnable(int enable)
{
    if ((enable < 0) || (enable > 1))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    msg_t msg;
    msg.hdr.msgId = _msgId++;
    msg.hdr.cmd = CMD_AUTH_ENABLE;
    msg.data = (UINT8)enable;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

int iDevices_SetFCCTxMode(UINT8 mode)
{
    if ((mode < FCC_TX_OFF) || (mode > FCC_TX_HIGH))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    msg_t msg;
    msg.hdr.msgId = _msgId++;
    msg.hdr.cmd = CMD_SET_FCC_TX_MODE;
    msg.data = (UINT8)mode;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

// synchronous read
int iDevices_GattReadHandle(UINT16 handle, UINT8 *buf, int bufLen, int *rxBufLen)
{
    if (!buf)
        return IDEV_ERR_NULL_POINTER;

    if ((bufLen < 0) || (bufLen > MAX_DATA_SIZE))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT16);
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_READ_VAR;
    UINT8 *data_p = &msg_p->data;
    *((UINT16 *)data_p) = LE_SWAP_16(handle);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        // if buffer len is shorter than data returned, only copy in "bufLen" bytes
        if (bufLen < _rxBufLen) {
            *rxBufLen = bufLen;
        } else {
            *rxBufLen = _rxBufLen;
        }
        MEMCPY(buf, _rxBuf, *rxBufLen);
    }
    FREE(msg_p);
    return status;
}

int iDevices_GattWriteHandle(UINT16 handle, UINT8 *buf, int len)
{
    if (!buf)
        return IDEV_ERR_NULL_POINTER;

    if ((len < 0) || (len > MAX_DATA_SIZE))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT16)+sizeof(UINT16)+len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_WRITE_VAR;
    UINT8 *data_p = &msg_p->data;
    *((UINT16 *)data_p) = LE_SWAP_16(handle);
    data_p+=2;
    UINT16 dataLen = (UINT16)len;
    *((UINT16 *)data_p) = LE_SWAP_16(dataLen);
    data_p+=2;
    MEMCPY(data_p, buf, len);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        if(READ_WAIT() == -1)
        {
        	FREE(msg_p);
        	return -1;
        }
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    FREE(msg_p);
    return status;
}

// synchronous call
// UUID is expected to be in Little-Endian format
int iDevices_GattAddService(UINT8 *svc_uuid, UINT8 uuid_len, UINT16 *handle)
{
    if (!svc_uuid)
        return IDEV_ERR_NULL_POINTER;

    if ((uuid_len != 2) && (uuid_len != 16))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+uuid_len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_ADD_SRVC;
    UINT8 *data_p = &msg_p->data;
    *(data_p++) = uuid_len;
    // convert to LE
    MEMCPY(data_p, svc_uuid, uuid_len);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (handle) {
            *handle = LE_SWAP_16(*((UINT16 *)_rxBuf));
        }
    }
    FREE(msg_p);
    return IDEV_SUCCESS;
}

// synchronous call
// UUID is expected to be in Little-Endian format
int iDevices_GattAddChar(UINT8 *char_uuid, UINT8 uuid_len, UINT8 perm, UINT8 prop, UINT16 *handle)
{
    if (!char_uuid)
        return IDEV_ERR_NULL_POINTER;

    if ((uuid_len != 2) && (uuid_len != 16))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+sizeof(UINT8)+sizeof(UINT8)+uuid_len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_ADD_CHAR;
    UINT8 *data_p = &msg_p->data;
    *(data_p++) = perm;
    *(data_p++) = prop;
    *(data_p++) = uuid_len;
    MEMCPY(data_p, char_uuid, uuid_len);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (handle) {
            *handle = LE_SWAP_16(*((UINT16 *)_rxBuf));
        }
    }
    FREE(msg_p);
    return IDEV_SUCCESS;
}

// synchronous call
// UUID is expected to be in Little-Endian format
int iDevices_GattAddCharVal(UINT8 *char_uuid, UINT8 uuid_len, UINT8 perm, UINT8 *value, UINT16 len, UINT16 *handle)
{
    if (!char_uuid || !value)
        return IDEV_ERR_NULL_POINTER;

    if ((uuid_len != 2) && (uuid_len != 16))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+sizeof(UINT8)+sizeof(UINT16)+uuid_len+len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_ADD_CHAR_VAL;
    UINT8 *data_p = &msg_p->data;
    *(data_p++) = (perm | ((uuid_len == 16) ? PERM_SERVCIE_UUID_128 : 0x0));
    *(data_p++) = uuid_len;
    MEMCPY(data_p, char_uuid, uuid_len);
    data_p+=uuid_len;
    MEMCPY(data_p, &len, 2);
    data_p+=sizeof(UINT16);
    MEMCPY(data_p, value, len);
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (handle) {
            *handle = LE_SWAP_16(*((UINT16 *)_rxBuf));
        }
    }
    FREE(msg_p);
    return IDEV_SUCCESS;
}

// synchronous call
int iDevices_GattAddCharDesc(UINT16 desc_uuid, UINT8 encrypted, UINT8 *value, UINT8 len, UINT16 *handle)
{
    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT16)+sizeof(UINT8)+sizeof(UINT8)+len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_ADD_CHAR_DESC;
    UINT8 *data_p = &msg_p->data;
    *(data_p++) = (UINT8)(desc_uuid & 0xff);
    *(data_p++) = (UINT8)(desc_uuid >> 8);
    *(data_p++) = encrypted;
    *(data_p++) = len;
    if (len && value) {
        MEMCPY(data_p, value, len);
    }
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (handle) {
            *handle = LE_SWAP_16(*((UINT16 *)_rxBuf));
        }
    }
    FREE(msg_p);
    return IDEV_SUCCESS;
}

int iDevices_GattCommitNewDb(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_COMMIT_NEW_DB;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

int iDevices_GattEraseDb(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_ERASE_DB;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

int iDevices_GattSaveDb(void)
{
    int status = IDEV_SUCCESS;
    hdr_t msg;
    msg.msgId = _msgId++;
    msg.cmd = CMD_SAVE_DB;
    status = sendData((UINT8 *)&msg, sizeof(msg));
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (_rxBuf[0]) {
            status = (IDEV_ERR_MODULE_RC_BASE + _rxBuf[0]);
        }
    }
    return status;
}

// synchronous call
// UUID is expected to be in Little-Endian format
int iDevices_GattGetHandleFromUUID(UINT8 *uuid, UINT8 uuid_len, UINT16 *handle)
{
    if (!uuid)
        return IDEV_ERR_NULL_POINTER;

    if ((uuid_len != 2) && (uuid_len != 16))
        return IDEV_ERR_PARAM_OUT_OF_RANGE;

    int status = IDEV_SUCCESS;
    int msgSize = sizeof(hdr_t)+sizeof(UINT8)+uuid_len;
    msg_t *msg_p = (msg_t *)MALLOC(msgSize);
    msg_p->hdr.msgId = _msgId++;
    msg_p->hdr.cmd = CMD_GATT_GET_HANDLE_FOR_UUID;
    UINT8 *data_p = &msg_p->data;
    *(data_p++) = uuid_len;
    if (uuid_len && uuid) {
        MEMCPY(data_p, uuid, uuid_len);
    }
    status = sendData((UINT8 *)msg_p, msgSize);
    if (status == IDEV_SUCCESS) {
        READ_WAIT();
        if (handle) {
            *handle = LE_SWAP_16(*((UINT16 *)_rxBuf));
        }
    }
    FREE(msg_p);
    return IDEV_SUCCESS;
}

// only handle one for now
int iDevices_RegisterInternalCallback(UINT16 handle, CHAR_UPDATED_CB cb_p)
{
    _mtHandle = handle;
    _mtCb = cb_p;
    return IDEV_SUCCESS;
}

int _HandleRxMsg(UINT8 *buf, int len)
{
    //DEBUG("_HandleRxMsg():");
    hdr_t *hdr_p = (hdr_t *)buf;

    switch (hdr_p->cmd)
    {
        case CMD_READY:
        	idevice_ready =YES;
            printf("\n\niDevices Module READY\n\n");
            break;
        case CMD_GET_BD_ADDR_RSP:
        {
            msg_t *msg_p = (msg_t *)buf;
            UINT8 *data_p = &msg_p->data;
            MEMCPY(_rxBuf, data_p, 6);
            _rxBufLen = 6;
            READ_POST();
            break;
        }
       case CMD_ADV_CHANGED:
        {
            msg_t *msg_p = (msg_t *)buf;
            
           
            if((msg_p->data == 1) && Test_progress_status == NO)
            {
            	lwgpio_int_enable(&BLE_host_interrupt, TRUE );	
            }
            ble_transmission_status = !(msg_p->data);
            
            DEBUG("\nCMD_ADV_CHANGED: %d", msg_p->data);
            if (_cbTable.event_cb) {
                _cbTable.event_cb(IDEV_ADV_CHANGED_EVENT, msg_p->data);
            }
            break;
        }
        case CMD_CONN_CHANGED:
        {
            msg_t *msg_p = (msg_t *)buf;
//            if(msg_p->data == 0)
//            {
//            	lwgpio_int_enable(&BLE_host_interrupt, TRUE );	
//            }
//            ble_transmission_status = msg_p->data;       
            DEBUG("\nCMD_CONN_CHANGED: %d", msg_p->data);
            if (_cbTable.event_cb) {
                _cbTable.event_cb(IDEV_CONN_CHANGED_EVENT, msg_p->data);
            }
            break;
        }
        case CMD_GET_STATE_RSP:
        {
            msg_t *msg_p = (msg_t *)buf;
            _rxBuf[0] = msg_p->data;
            _rxBufLen = 1;
            READ_POST();
            break;
        }
        case CMD_GATT_READ_VAR_RSP:
        {
            msg_t *msg_p = (msg_t *)buf;
            UINT8 *data_p = &msg_p->data;
            UINT16 handle = LE_SWAP_16(*((UINT16 *)data_p));
            data_p+=sizeof(UINT16);
            UINT16 dataLen = LE_SWAP_16(*((UINT16 *)data_p));
            data_p+=sizeof(UINT16);
            // restrict size of rx data
            if (dataLen > MAX_DATA_SIZE) 
            {
                dataLen = MAX_DATA_SIZE;
            }
            //DEBUG("\nCMD_GATT_READ_VAR_RSP: hdl=0x%04x, dataLen=%d", handle, dataLen);
            MEMCPY(_rxBuf, data_p, dataLen);
            _rxBufLen = (UINT8)dataLen;
            READ_POST();
            break;
        }
        case CMD_GATT_VAR_UDATED:
        {
            msg_t *msg_p = (msg_t *)buf;
            UINT8 *data_p = &msg_p->data;
            UINT16 handle = LE_SWAP_16(*((UINT16 *)data_p));
            data_p+=sizeof(UINT16);
            UINT16 dataLen = LE_SWAP_16(*((UINT16 *)data_p));
            data_p+=sizeof(UINT16);
            // restrict size of rx data
            if (dataLen > MAX_DATA_SIZE) {
                dataLen = MAX_DATA_SIZE;
            }
            DEBUG("\nCMD_GATT_VAR_UDATED: hdl=0x%04x, dataLen=%d\n", handle, dataLen);
            MEMCPY(_rxBuf, data_p, dataLen);
            _rxBufLen = (UINT8)dataLen;
            if (handle == _mtHandle) {
                _mtCb(handle, _rxBuf, _rxBufLen);
            } else {
                if (_cbTable.char_updated_cb) {
                    _cbTable.char_updated_cb(handle, _rxBuf, _rxBufLen);
                }
            }
            break;
        }
        case CMD_GATT_ADD_RSP:
        {
            gatt_add_rsp_t *rsp_p = (gatt_add_rsp_t *)buf;
            UINT16 handle = LE_SWAP_16(rsp_p->handle);
            DEBUG("\nCMD_GATT_ADD_RSP: hdl=0x%04x\n", handle);
            MEMCPY(_rxBuf, &(rsp_p->handle), sizeof(UINT16));
            _rxBufLen = sizeof(UINT16);
            READ_POST();
            break;
        }
        case CMD_RESPONSE:
        {
            msg_t *rsp_p = (msg_t *)buf;
            UINT8 rc = rsp_p->data;
            DEBUG("\nCMD_RESPONSE: 0x%02x \n", rc);
            MEMCPY(_rxBuf, &(rsp_p->data), sizeof(UINT8));
            _rxBufLen = sizeof(UINT8);
            READ_POST();
            break;
        }
        default:
            DEBUG("\n\nUnknown CMD rcvd: %d", hdr_p->cmd);
            break;
    }

    return 0;
}
