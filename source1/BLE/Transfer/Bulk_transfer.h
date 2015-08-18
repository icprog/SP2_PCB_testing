/*
 * Bulk_transfer.h
 *
 *  Created on: May 28, 2014
 *      Author: user
 */

#ifndef BULK_TRANSFER_H_
#define BULK_TRANSFER_H_

#include <mqx.h>
#include <bsp.h>
#include <mfs.h>
#include <fio.h>
#include "config.h"
#include "BLE/Common/api.h"
#include "BLE/Common/os.h"
#include "BLE/Common/adv.h"
#include "BLE/Common/types.h"

typedef int (*TRANSFER_CB)(int,UINT8*,int);

typedef enum {
    TRANSFER_START = 0x50,
    TRANSFER_DATA,
    TRANSFER_STOP,
} tx_cmd_e;

typedef struct {
    UINT8  cmd;
    UINT32 len;
} tx_start_msg_t;

typedef struct {
    UINT8  cmd;
    UINT8 len;
    UINT8 data[20];
} tx_data_msg_t;

typedef struct {
    UINT8  cmd;
} tx_stop_msg_t;

// TRANSFER_START Format
//    1        4
// +-----+-----------+
// | CMD | DATA_SIZE |
// +-----+-----------+

// TRANSFER_DATA Format
//    1     1     1 - 20 (NOTE: 18 for Rx)
// +-----+-----+-----------+
// | CMD | LEN |   DATA    |
// +-----+-----+-----------+

// TRANSFER_STOP Format
//    1
// +-----+
// | CMD |
// +-----+

int iDevices_initTransferService(UINT16 maxBufferSize, TRANSFER_CB transferCb);
int iDevices_sendBulkData(UINT8 *buf, UINT32 len);
int BLE_Advertising(uint_8);
extern uint_8 Ble_Module_init(void);
extern void Stop_BLE_bulk_transfer(void);
extern void Clear_BLE_rxn_buff(void);
extern void Reset_BLE(void);
extern void Enable_BLE(void);
extern void Disable_BLE(void);
extern uint_8 Ble_set_baudrate(void);



extern  LWGPIO_STRUCT        		BLE_host_interrupt;
#define BLE_INT_PIN     		    (GPIO_PORT_C | GPIO_PIN19)
#define BLE_INT_PIN_MUX_IRQ   		(LWGPIO_MUX_C19_GPIO)

extern  LWGPIO_STRUCT      			BLE_wakeup_pin;
#define BLE_WAKEUP_PIN    			(GPIO_PORT_C | GPIO_PIN18)
#define BLE_WAKEUP_PIN_MUX_IRQ   	(LWGPIO_MUX_C18_GPIO)

extern  LWGPIO_STRUCT      			BLE_reset_pin;
#define BLE_RESET_PIN      			(GPIO_PORT_C | GPIO_PIN15)
#define BLE_RESET_PIN_MUX_IRQ   	(LWGPIO_MUX_C15_GPIO)

extern  uint_8 						Ble_wakeup;
extern  uint_8 						BLE_host_wakeup_flag;
extern  uint_8 						Ble_powerup_stat;
extern  uint_8 						Test_progress_status;


extern char    Query_list_name[16];
extern uint_32 BLE_curr_baud;

#endif /* BULK_TRANSFER_H_ */
