/*
 * Ble_uart.h
 *
 *  Created on: May 28, 2014
 *      Author: user
 */

#ifndef BLE_UART_H_
#define BLE_UART_H_


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include "config.h"
#include "types.h"

extern MQX_FILE_PTR ble_uart_fd;
extern uint_8 Ble_uart_init(void);
extern uint_8 Ble_uart_deinit(void);
extern LWEVENT_STRUCT    BLE_process_event;
extern LWEVENT_STRUCT    BLE_txn_event;

typedef INT32 (*PUART_CB_FUNC)(UINT8*,int);

extern void registerUARTCallback(PUART_CB_FUNC userfn);
extern int  sendData(UINT8 *data_p, int len);
extern void handle_PUART_RxIntCb(pointer user_isr_ptr);
//extern void handle_PUART_RxIntCb(void);
extern void BLE_Process_Task(uint_32);
extern void BLE_Transmission_Task(uint_32);


int osUartWait(void);
int osUartPost(void);

#define READ_WAIT() osUartWait()
#define READ_POST() osUartPost()


#ifdef CONFIGURABLE_SLEEP_MODE
int enableWakeUpModuleHandshake(BOOL enable);
#endif


#endif /* BLE_UART_H_ */
