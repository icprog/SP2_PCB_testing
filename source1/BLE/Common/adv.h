/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: adv.h
*
* Abstract: This is the header file for adv.c, which contains various enum
*           and functions definitions.
*
*******************************************************************************/

#ifndef __ADV_H__
#define __ADV_H__

#include "types.h"

UINT8 iDevices_AdvDataGetDataSize(UINT8 *advData);
UINT8 iDevices_AdvDataGetRemainingBytes(UINT8 *advData);
int iDevices_InitAdvData(UINT8 *advData, UINT8 *scanRspData);
int iDevices_AdvDataSetLocalName(UINT8 *buf, UINT8 *localName, int len, BOOL shortName);
int iDevices_AdvDataSetServiceUUID(UINT8 *buf, UINT8 *uuid, int len, BOOL more);
int iDevices_AdvDataSetMfgData(UINT8 *buf, UINT8 *data, int len, UINT16 company, BOOL encryptedMfgData);
int iDevices_AdvDataSetCOD(UINT8 *buf, UINT8 *cod);

#endif // __ADV_H__
