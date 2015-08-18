/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: api.h
*
* Abstract: This is the header file for api.c, which contains various enum
*           and functions definitions.
*
*******************************************************************************/

#ifndef __API_H__
#define __API_H__

#include <mqx.h>
#include <bsp.h>
#include "config.h"

#include "types.h"
#include "Ble_uart.h"

//This variable is adjusted according to bluetooth status
extern volatile uint_8 ble_transmission_status;

// Characteristic Descriptor definitions
enum BLE_UUID_CHAR_DESCR
{
    UUID_CHAR_DESCR_CHAR_EXT_PROP = 0x2900,
    UUID_CHAR_DESCR_CHAR_USER_DESCR = 0x2901,
    UUID_CHAR_DESCR_CLIENT_CHAR_CFG = 0x2902,
    UUID_CHAR_DESCR_SERVER_CHAR_CCFG = 0x2903,
    UUID_CHAR_DESCR_CHAR_PRES_FORMAT = 0x2904,
    UUID_CHAR_DESCR_CHAR_AGG_FORMAT = 0x2905,
    UUID_CHAR_DESCR_VALID_RANGE = 0x2906,
    UUID_CHAR_DESCR_EXT_REPORT_REF = 0x2907,
    UUID_CHAR_DESCR_REPORT_REF = 0x2908,
};

// Permission bits
#define PERM_MASK                             (0x7f)

#define PERM_VARIABLE_LENGTH                  ((UINT8)((UINT8)0x1<<0))
#define PERM_READABLE                         ((UINT8)((UINT8)0x1<<1))
#define PERM_WRITE_CMD                        ((UINT8)((UINT8)0x1<<2))
#define PERM_WRITE_REQ                        ((UINT8)((UINT8)0x1<<3))
#define PERM_AUTH_READABLE                    ((UINT8)((UINT8)0x1<<4))
#define PERM_RELIABLE_WRITE                   ((UINT8)((UINT8)0x1<<5))
#define PERM_AUTH_WRITABLE                    ((UINT8)((UINT8)0x1<<6))
#define PERM_SERVCIE_UUID_128                 ((UINT8)((UINT8)0x1<<7))

#define PERM_WRITABLE  (PERM_WRITE_CMD | PERM_WRITE_REQ| PERM_AUTH_WRITABLE)

// Property bits
#define PROP_BROADCAST                        ((UINT8)((UINT8)0x1<<0))
#define PROP_READABLE                         ((UINT8)((UINT8)0x1<<1))
#define PROP_WRITE_CMD                        ((UINT8)((UINT8)0x1<<2))
#define PROP_WRITE_REQ                        ((UINT8)((UINT8)0x1<<3))
#define PROP_NOTIFY                           ((UINT8)((UINT8)0x1<<4))
#define PROP_INDICATE                         ((UINT8)((UINT8)0x1<<5))
#define PROP_SIGNED_WRITES                    ((UINT8)((UINT8)0x1<<6))
#define PROP_EXTENDED_PROP                    ((UINT8)((UINT8)0x1<<7))

// ADV flag values
enum ADV_FLAG_VALUES
{
    ADV_FLAGS = 0x01,
    ADV_SERVICE_UUID16_MORE = 0x02,
    ADV_SERVICE_UUID16_COMP = 0x03,
    ADV_SERVICE_UUID32_MORE = 0x04,
    ADV_SERVICE_UUID32_COMP = 0x05,
    ADV_SERVICE_UUID128_MORE = 0x06,
    ADV_SERVICE_UUID128_COMP = 0x07,
    ADV_LOCAL_NAME_SHORT = 0x08,
    ADV_LOCAL_NAME_COMP = 0x09,
    ADV_TX_POWER_LEVEL = 0x0A,
    ADV_CLASS_OF_DEVICE = 0x0D,
    ADV_SIMPLE_PAIRING_HASH_C = 0x0E,
    ADV_SIMPLE_PAIRING_RANDOMIZER_R = 0x0F,
    ADV_TK_VALUE = 0x10,
    ADV_OOB_FLAGS = 0x11,
    ADV_SLAVE_CONNECTION_INTERVAL_RANGE = 0x12,
    ADV_SERVICE_UUID16 = 0x14,
    ADV_SERVICE_UUID128 = 0x15,
    ADV_SERVICE_DATA = 0x16,
    ADV_SERVICE_TARGET_PUBLIC_ADR = 0x17,
    ADV_SERVICE_TARGET_RANDOM_ADR = 0x18,
    ADV_MANUFACTURER_DATA = 0xFF,
};

// ADV_FLAGS bits
#define LE_LIMITED_DISCOVERABLE     0x01 // LE Limited Discoverable Mode
#define LE_GENERAL_DISCOVERABLE     0x02 // LE General Discoverable Mode
#define BR_EDR_NOT_SUPPORTED        0x04 // BR/EDR Not Supported
#define SIMUL_LE_BR_EDR_CONTROLLER  0x08 // Simultaneous LE and BR/EDR to Same Device Capable (Controller)
#define SIMUL_LE_BR_EDR_HOST        0x10 // Simultaneous LE and BR/EDR to Same Device Capable (Host)

#define MAX_DATA_SIZE (22)
#define MAX_ADV_SIZE  (31)

#define NO  (0)
#define YES (1)

#define POWER_OFF	(0)
#define POWER_ON	(1)
#define RESTART   	(2)

#define OFF (0)
#define ON  (1)

typedef enum {
    IDEV_SUCCESS                    = 0,
    IDEV_ERR_BASE_ERROR             = 0x80000000,
    IDEV_ERR_MODULE_RC_BASE         = 0x80008000, // see module_rc_e in msg.h
    IDEV_ERR_GENERAL_ERROR          = IDEV_ERR_BASE_ERROR,
    IDEV_ERR_PARAM_OUT_OF_RANGE,
    IDEV_ERR_PARAM_TOO_LONG,
    IDEV_ERR_NULL_POINTER,
    IDEV_ERR_MODULE_NOT_READY,
} rc_e;

typedef enum {
    IDEV_ADV_CHANGED_EVENT  = 0,
    IDEV_CONN_CHANGED_EVENT,
} event_e;

typedef int (*CHAR_UPDATED_CB)(UINT16,UINT8*,int);
typedef int (*EVENT_CB)(int,int);

typedef struct {
    CHAR_UPDATED_CB char_updated_cb;
    EVENT_CB        event_cb;
} idevCb_t;

extern uint_8 idevice_ready;

int iDevices_Init(idevCb_t *cb_p);
int iDevices_Reset(void);
int iDevices_FactoryReset(void);
int iDevices_Sleep(void);
int iDevices_RuntimeSleepMode(BOOL enable);
int iDevices_SetBaudrate(UINT32 baudrate);
int iDevices_GetBDAddr(UINT8 *bdAddr);
int iDevices_SetAdvData(UINT8 *advData, int len, BOOL encryptedMfgData);
int iDevices_SetScanRspData(UINT8 *scanRspData, int len);
int iDevices_SetAdvEnable(int enable, int isConnectable);
int iDevices_SetTimedAdv(int enable);
int iDevices_GetAdvState(UINT8 *state);
int iDevices_GetConnState(UINT8 *state);
int iDevices_UpdateConnParams(UINT16 minInterval, UINT16 maxInterval, UINT16 slaveLatency, UINT16 timeout);
int iDevices_GetConnParams(UINT16 *connInterval, UINT16 *slaveLatency, UINT16 *timeout);
int iDevices_Disconnect(void);
int iDevices_SetEncryptionKey(UINT8 *key);
int iDevices_SetAuthenticationEnable(int enable);
int iDevices_SetFCCTxMode(UINT8 mode);
int iDevices_GattReadHandle(UINT16 handle, UINT8 *buf, int bufLen, int *rxBufLen);
int iDevices_GattWriteHandle(UINT16 handle, UINT8 *buf, int len);
int iDevices_GattAddService(UINT8 *svc_uuid, UINT8 uuid_len, UINT16 *handle);
int iDevices_GattAddChar(UINT8 *char_uuid, UINT8 uuid_len, UINT8 perm, UINT8 prop, UINT16 *handle);
int iDevices_GattAddCharVal(UINT8 *char_uuid, UINT8 uuid_len, UINT8 perm, UINT8 *value, UINT16 len, UINT16 *handle);
int iDevices_GattAddCharDesc(UINT16 desc_uuid, UINT8 encrypted, UINT8 *value, UINT8 len, UINT16 *handle);
int iDevices_GattCommitNewDb(void);
int iDevices_GattEraseDb(void);
int iDevices_GattSaveDb(void);
int iDevices_GattGetHandleFromUUID(UINT8 *uuid, UINT8 uuid_len, UINT16 *handle);
int iDevices_RegisterInternalCallback(UINT16 handle, CHAR_UPDATED_CB cb_p);

#endif // __API_H__
