/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: adv.c
*
* Abstract: This file contains the API definitions specific to configuring
*           advertising parameters.
*
*******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "api.h"
#include "msg.h"
#include "os.h"


UINT8 iDevices_AdvDataGetDataSize(UINT8 *advData)
{
    BOOL done = NO;
    UINT8 advDataIdx = 0;

    while (!done) {
        UINT8 entryLen = advData[advDataIdx];
        if (entryLen) {
            if (entryLen < (MAX_ADV_SIZE - advDataIdx)) {
                advDataIdx += (entryLen+1);
            } else {
                done = YES;
                return 0xff; // guarantees subsequent range check will fail
            }
        } else {
            done = YES;
        }
    }

    return advDataIdx;
}

UINT8 iDevices_AdvDataGetRemainingBytes(UINT8 *advData)
{
    return (MAX_ADV_SIZE - iDevices_AdvDataGetDataSize(advData));
}

int iDevices_InitAdvData(UINT8 *advData, UINT8 *scanRspData)
{
    if (!advData) {
        return IDEV_ERR_NULL_POINTER;
    }

    if (scanRspData) {
        MEMSET(scanRspData, 0, MAX_ADV_SIZE);
    }

    MEMSET(advData, 0, MAX_ADV_SIZE);

    int i = 0;
    advData[i++] = 0x2; // length.
    advData[i++] = ADV_FLAGS;
    advData[i++] = (LE_GENERAL_DISCOVERABLE | BR_EDR_NOT_SUPPORTED); // 0x06

    return IDEV_SUCCESS;
}

int iDevices_AdvDataSetLocalName(UINT8 *buf, UINT8 *localName, int len, BOOL shortName)
{
    if (!buf || !localName) {
        return IDEV_ERR_NULL_POINTER;
    }

    int advDataIdx = iDevices_AdvDataGetDataSize(buf);
    if (advDataIdx >= (MAX_ADV_SIZE - len+2)) {
        return IDEV_ERR_PARAM_TOO_LONG;
    }

    buf[advDataIdx++] = len+1; // Length
    buf[advDataIdx++] = shortName ? ADV_LOCAL_NAME_SHORT : ADV_LOCAL_NAME_COMP;
    MEMCPY(&buf[advDataIdx], localName, len);
    advDataIdx+=len;

    return IDEV_SUCCESS;
}

int iDevices_AdvDataSetServiceUUID(UINT8 *buf, UINT8 *uuid, int len, BOOL more)
{
    if (!buf || !uuid) {
        return IDEV_ERR_NULL_POINTER;
    }

    int advDataIdx = iDevices_AdvDataGetDataSize(buf);
    if (advDataIdx >= (MAX_ADV_SIZE - len+2)) {
        return IDEV_ERR_PARAM_TOO_LONG;
    }

    buf[advDataIdx++] = len+1; // Length
    if (len == 2) {
        buf[advDataIdx++] = more ? ADV_SERVICE_UUID16_MORE : ADV_SERVICE_UUID16_COMP;
    } else if (len == 16) {
        buf[advDataIdx++] = more ? ADV_SERVICE_UUID128_MORE : ADV_SERVICE_UUID128_COMP;
    }
    MEMCPY(&buf[advDataIdx], uuid, len);
    advDataIdx+=len;

    return IDEV_SUCCESS;
}

int iDevices_AdvDataSetMfgData(UINT8 *buf, UINT8 *data, int len, UINT16 company, BOOL encrypted)
{
    if (!buf || !data) {
        return IDEV_ERR_NULL_POINTER;
    }

    int advDataIdx = iDevices_AdvDataGetDataSize(buf);
    if (encrypted) {
        if ((advDataIdx >= (MAX_ADV_SIZE - 19)) || (len > 16)){
            return IDEV_ERR_PARAM_TOO_LONG;
        }
    } else {
        if (advDataIdx >= (MAX_ADV_SIZE - len+4)) {
            return IDEV_ERR_PARAM_TOO_LONG;
        }
    }

    buf[advDataIdx++] = encrypted ? 19 : (len+3); // Length
    buf[advDataIdx++] = ADV_MANUFACTURER_DATA;
    buf[advDataIdx++] = ((UINT8)(company & 0x00ff));
    buf[advDataIdx++] = ((UINT8)((company & 0xff00)>>8));
    MEMCPY(&buf[advDataIdx], data, len);
    if (encrypted) {
        advDataIdx+=16;
    } else {
        advDataIdx+=len;
    }

    return IDEV_SUCCESS;
}

int iDevices_AdvDataSetCOD(UINT8 *buf, UINT8 *cod)
{
    if (!buf || !cod) {
        return IDEV_ERR_NULL_POINTER;
    }

    int advDataIdx = iDevices_AdvDataGetDataSize(buf);
    if (advDataIdx >= (MAX_ADV_SIZE - 5)) {
        return IDEV_ERR_PARAM_TOO_LONG;
    }

    buf[advDataIdx++] = 4; // Length
    buf[advDataIdx++] = ADV_CLASS_OF_DEVICE;
    MEMCPY(&buf[advDataIdx], cod, 3);
    advDataIdx+=3;

    return IDEV_SUCCESS;
}
