/**HEADER********************************************************************
 *
 * Copyright (c) 2012 Freescale Semiconductor;
 * All Rights Reserved
 *
 ***************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************
 *
 * $FileName: main.c$
 * Comments:
 *
 *   This file contains the source for a simple example of an
 *   application that writes Memory LCD using the SPI driver.
 * 
 *
 *END************************************************************************/

#include "common_headers.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

char 	 BUSY=FALSE,Read_LSM=0;
LWGPIO_STRUCT PWR_5V_RAIL,KILL_CONTROL;


LWEVENT_STRUCT                  app_event,Lsm_Event;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
		/* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
//		{ BLE_PROCESS_TASK,BLE_Process_Task,6000,7,"BLE_Process_Task",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK,0,0},
//		{ TEST_TASK, Test_task, 2000,	8, "Main",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK, 0,0},
//		{ 2, Lsm_Task, 2000,	8, "Lsm",MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK, 0,0},
		{ UI_TASK,	 UI_Task, 15000,9,"User_Interface", MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK,0,0	},
		{ 0 }
};

void wait_for_event (void)
{
    _mqx_uint   app_event_value;

    /* Wait for button press */
    _lwevent_wait_ticks (&app_event, (SW_EVENT_MASK), FALSE, 0);

      /* Copy event masks into temporary variable */
     app_event_value = _lwevent_get_signalled();

    /* If button pressed or switch expires, moving to next clock configuration */
    if (app_event_value & SW_EVENT_MASK)
    {
//        printf("\nProcessing Display Event Generated\n");
    }
}

void wait_for_Lsm_event (void)
{
    _mqx_uint   app_event_value;

    /* Wait for button press */
    _lwevent_wait_ticks (&Lsm_Event, (SW_EVENT_MASK), FALSE, 0);

      /* Copy event masks into temporary variable */
     app_event_value = _lwevent_get_signalled();

    /* If button pressed or switch expires, moving to next clock configuration */
    if (app_event_value & SW_EVENT_MASK)
    {
//        printf("\nProcessing Display Event Generated\n");
    }
}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_init
 *  Brief:        Configuring the 5V power rail enable pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_init(void)
{
	
	if (!lwgpio_init(&PWR_5V_RAIL, PWR_RAIL_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}
	
	lwgpio_set_functionality(&PWR_5V_RAIL, PWR_RAIL_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_LOW);

}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_enable
 *  Brief:        Enable  the 5V power rail
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_enable(void)
{
	
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     power_rail_disable
 *  Brief:        Disable  the 5V power rail
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void power_rail_disable(void)
{
	lwgpio_set_value(&PWR_5V_RAIL, LWGPIO_VALUE_LOW);
}


void Kill_Controller_Init()
{
	if (!lwgpio_init(&KILL_CONTROL, KILL_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");

	}
	
	lwgpio_set_functionality(&KILL_CONTROL, KILL_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_LOW);
}


void Kill_ON_Controller()
{
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_HIGH);
}

void Kill_OFF_Controller()
{
	lwgpio_set_value(&KILL_CONTROL, LWGPIO_VALUE_LOW);
}

void Test_task (uint_32)
{
    /* Create global event */
    if (_lwevent_create(&app_event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }
	while (1)
	{
		wait_for_event();
		LCD_Updating_Flag=1;
		display_New_Snow_profile_processing();
		LCD_Updating_Flag=0;
	}
}
unsigned int lsm_test_ctr=0;
void Lsm_Task(uint_32)
{
    /* Create global event */
    if (_lwevent_create(&Lsm_Event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }
	while (1)
	{
		wait_for_Lsm_event();
		lsm_test_ctr++;
		LSM_Updating_Flag=1;
	    read_magnetometer_data();
	    read_accelerometer_data();
		LSM_Updating_Flag=0;
	}
}
