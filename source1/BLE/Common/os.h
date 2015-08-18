/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO IDEVICES LLC
* ------------------------------------------------------------------------------
*
* Copyright (c) 2013 iDevices, LLC
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: os.h
*
* Abstract: This is the header file for os.c, which is intended to house custom
*           versions of low level functions specific to your target platform.
*
*******************************************************************************/

#ifndef __OS_H__
#define __OS_H__

//#include <stdio.h>

#include <mqx.h>
#include <bsp.h>
#include "config.h"
#include <stdlib.h>
#include "types.h"

//
// Assume we are compiled under Little Endian system.
#define BE_SWAP_16(var)  (( UINT16)( ((var) << 8) | (((var) >> 8) & 0xff) ) )
#define LE_SWAP_16(var)  (var)
#define BE_SWAP_32(var)  (     ((var & 0x000000FF)<<24)  \
                           |   ((var & 0x0000FF00)<<8)   \
                           |   ((var & 0x00FF0000)>>8)   \
                           |  ((var & 0xFF000000)>>24)   \
                         )
#define LE_SWAP_32(var)    (var)

#define SAMPLE 1


/*
 * set to local GPIO function to set/clear wake up module pin: int osWakeUpModulePin(int state)
 */
#if SAMPLE
int osSetWakeUpModulePin(int state);
#define SET_WAKUP_MODULE_PIN(x) osSetWakeUpModulePin(x)
#else
#define SET_WAKUP_MODULE_PIN(x) (0)
#endif

/*
 * set to local GPIO function to poll wake up host pin: int osPollWakeUpHostPin(void)
 */
#if SAMPLE
int osPollWakeUpHostPin(void);
#define POLL_WAKUP_HOST_PIN() osPollWakeUpHostPin()
#else
#define POLL_WAKUP_HOST_PIN() (0)
#endif

/*
 * set to local debug printf-style function: void some_func(fmt,...)
 */
#if SAMPLE
#define DEBUG //printf
#else
#define DEBUG(fmt, ...) (0)
#endif

/*
 * set to local mutex lock wrapper function: void wait(void)
 */
//#if SAMPLE
//int osUartWait(void);
//#define READ_WAIT() osUartWait()
////#define READ_WAIT() ;   //_time_delay(10)
//#else
//#define READ_WAIT() (0)
//#endif

/*
 * set to local uS sleep function: usleep(int durationInUs)
 */
#define Sleep _time_delay

#if SAMPLE
int osUsleep(int durationInUs);
#define USLEEP(x) osUsleep(x)
#else
#define USLEEP(x) (0)
#endif

/*
 * set to local mutex unlock wrapper function: void post(void)
 */
//#if SAMPLE
//int osUartPost(void);
////#define READ_POST() osUartPost()
//#define READ_POST() ;
//#else
//#define READ_POST() (0)
//#endif

#if SAMPLE
void *osMalloc(int size);
#define MALLOC(x) malloc(x)
#else
#define MALLOC(x) (0)
#endif

#if SAMPLE
void osFree(void *ptr);
#define FREE(x) free(x)
#else
#define FREE(x) (0)
#endif

#if SAMPLE
void *osMemset(void *dst, int val, int size);
#define MEMSET(x,y,z) memset(x,y,z)
#else
#define MEMSET(x,y,z) (0)
#endif

#if SAMPLE
void *osMemcpy(void *dst, void *src, int size);
#define MEMCPY(x,y,z) memcpy(x,y,z)
#else
#define MEMCPY(x,y,z) (0)
#endif

#if SAMPLE
#define MEMCMP(x,y,z) memcmp(x,y,z)
#else
#define MEMCMP(x,y,z) (0)
#endif

int osInit(void);
int osSetPollTimeout(int timeout);

#endif // __OS_H__
