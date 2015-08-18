/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: msg.h
*
* Abstract: This file contains enums and structures that define the interface
*           between the host processor and the custom iDevices FW.
*
*******************************************************************************/

#ifndef __MSG_H__
#define __MSG_H__

#include "types.h"

// return codes for CMD_RESPONSE
typedef enum {
    RC_SUCCESS = 0,
    RC_GENERAL_FAILURE,
    RC_BAD_PARAM,
    RC_INVALID_HANDLE,
    RC_PARAM_TOO_LONG,
} module_rc_e;

// data vals for CMD_ADV_CHANGED
#define ADV_OFF (0)
#define ADV_ON  (1)
#define ADV_HI  (2)
#define ADV_LO  (3)

// data vals for CMD_CONN_CHANGED
#define CONN_UP   (1)
#define CONN_DOWN (0)

// data vals for CMD_SET_FCC_TX_MODE
#define FCC_TX_OFF  (0)
#define FCC_TX_LOW  (1)
#define FCC_TX_MED  (2)
#define FCC_TX_HIGH (3)

typedef enum {
    CMD_READY = 0,              // 0
    CMD_RESET,
    CMD_FACTORY_RESET,
    CMD_SLEEP,
    CMD_SET_BAUDRATE,           // 0x04
    CMD_GET_BD_ADDR,
    CMD_SET_UNDIR_ADV_PARAM,
    CMD_SET_DIR_ADV_PARAM,
    CMD_SET_ADV_DATA,           // 0x08
    CMD_SET_SCAN_RSP_DATA,
    CMD_SET_TIMED_ADV,
    CMD_ADV_ENABLE,
    CMD_ADV_CHANGED,            // 0x0c
    CMD_GET_ADV_STATE,
    CMD_CONN_CHANGED,
    CMD_GET_CONN_STATE,
    CMD_UPDATE_CONN_PARAMS,     // 0x10
    CMD_DISCONNECT,
    CMD_SET_ENCRYPTION_KEY,
    CMD_AUTH_ENABLE,
    CMD_GATT_READ_VAR,          // 0x14
    CMD_GATT_WRITE_VAR,
    CMD_GATT_VAR_UDATED,
    CMD_COMMIT_NEW_DB,
    CMD_PRINT_DB,               // 0x18
    CMD_ERASE_DB,
    CMD_SAVE_DB,
    CMD_GATT_ADD_SRVC,
    CMD_GATT_ADD_CHAR,          // 0x1c
    CMD_GATT_ADD_CHAR_VAL,
    CMD_GATT_ADD_CHAR_DESC,
    CMD_GATT_GET_HANDLE_FOR_UUID,
    CMD_GET_CONN_PARAMS,        // 0x20
    CMD_RUNTIME_SLEEP_MODE,
    CMD_SET_FCC_TX_MODE,
    CMD_RESPONSE = 0x80,
    CMD_GET_BD_ADDR_RSP,
    CMD_GET_STATE_RSP,
    CMD_GATT_READ_VAR_RSP,
    CMD_GATT_ADD_RSP,           // 0x84 - used as response for all GATT "add" requests
    CMD_GET_CONN_PARAMS_RSP,
} cmd_e;

typedef struct {
    UINT8 msgId;
    UINT8 cmd;
} hdr_t;

typedef struct {
    hdr_t hdr;
    UINT8 data;
} msg_t;

typedef struct {
    hdr_t  hdr;
    UINT16 handle;
} gatt_add_rsp_t;

#if 0
typedef struct {
    hdr_t  hdr;
    UINT16 handle;
} gatt_read_var_msg_t;

typedef struct {
    hdr_t  hdr;
    UINT16 handle;
    UINT16 len;
    UINT8  data[len];
} gatt_read_var_msg_rsp_t;

#define gatt_var_updated_t gatt_read_var_msg_rsp_t

typedef struct {
    hdr_t  hdr;
    UINT16 handle;
    UINT16 len;
    UINT8  data[len];
} gatt_write_var_msg_t;

typedef struct {
    hdr_t  hdr;
    UINT8  uuid_len; // need to use 16-bit to maintain byte alignment
    UINT8  uuid[2 or 16];
} gatt_add_service_msg_t;

typedef struct {
    hdr_t  hdr;
    UINT8  perm;
    UINT8  prop;
    UINT8  uuid_len; // need to use 16-bit to maintain byte alignment
    UINT8  uuid[2 or 16];
} gatt_add_char_msg_t;

typedef struct {
    hdr_t  hdr;
    UINT8  perm;
    UINT8  uuid_len
    UINT8  uuid[2 or 16];
    UINT16 len;
    UINT8  val[len];
} gatt_add_char_val_msg_t;

typedef struct {
    hdr_t  hdr;
    UINT16 desc_uuid;
    UINT8  encrypted;
    UINT8  len
    UINT8  val[len];
} gatt_add_char_desc_msg_t;
#endif

#endif // __MSG_H__
