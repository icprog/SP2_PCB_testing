/******************************************************************************
 *
 *     Filename:       $power_mode.c
 *     Created on:     $Date: Mar 25, 2014    (11:25:04 AM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file
 *
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

//#include <mqx.h>
//#include <bsp.h>
//#include "Power_mode.h"
//#include "spi_lcd.h"
//#include "Gps.h"
//#include "adc_k60.h"
//#include "Hw_Init.h"
#include "common_headers.h"
#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! MQX_ENABLE_LOW_POWER
#error This application requires MQX_ENABLE_LOW_POWER to be defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#if ! MQX_USE_IDLE_TASK
#error This application requires MQX_USE_IDLE_TASK to be defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define TEST_CHANNEL   "spi2:"
volatile char run_mode_flag = 1;
volatile char low_power_mode_flag = 1;

#define OSC_EN_PIN			(GPIO_PORT_A|GPIO_PIN19)
#define OSC_EN_PIN_MUX		(LWGPIO_MUX_A19_GPIO)

extern void OSC_EN_init(void);
extern void OSC_Enable(void);
extern void OSC_Disable(void);

LWGPIO_STRUCT OSC_EN;

/*-----------------------------------------------------------------------------
 *  Function:     OSC_EN_init
 *  Brief:        Configuring the External 50Mhz osc enable pin
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void OSC_EN_init(void)
{

	if (!lwgpio_init(&OSC_EN, OSC_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&OSC_EN, OSC_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&OSC_EN, LWGPIO_VALUE_HIGH);

}

/*-----------------------------------------------------------------------------
 *  Function:     OSC_Enable
 *  Brief:        Enable 50Mhz osc
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void OSC_Enable(void)
{
	lwgpio_set_value(&OSC_EN, LWGPIO_VALUE_HIGH);
}

/*-----------------------------------------------------------------------------
 *  Function:     OSC_Disable
 *  Brief:        Disable 50Mhz osc
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void OSC_Disable(void)
{

	lwgpio_set_value(&OSC_EN, LWGPIO_VALUE_LOW);

}


/*-----------------------------------------------------------------------------
 *  Function:     low_power_mode_on
 *  Brief:        Set the low power mode enable flag
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void low_power_mode_on(void)
{
    low_power_mode_flag = 1;
    Sleep_timer_start(60000);
}

/*-----------------------------------------------------------------------------
 *  Function:     low_power_mode_off
 *  Brief:        Reset the low power mode enable flag
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void low_power_mode_off(void)
{
    low_power_mode_flag = 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     De_Init_HardWare_When_Battery_Low
 *  Brief:        deinitialise hardware modules when battery is low
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void De_Init_HardWare_When_Battery_Low(void)
{
    
//  Write_Data_file_information_To_Sdcard();
    gps_deinit();
    Lsm303_deinit();
//  Tmp006_deinit();
//    ADC_deinit();
    ui_timer_de_init();
    Sd_Card_deinit();
    
//    printf("\nVLPR  mode\n");
//    printf("\nChanging frequency to 2 MHz.\n");
//    if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2MHZ))
//    {
//        printf("Cannot change clock configuration");
//    }
//    
//    Core_clock_selection = LOW_POWER_MODE;
//    _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2MHZ);
//    _lpm_set_operation_mode(LPM_OPERATION_MODE_WAIT);
    
}

/*-----------------------------------------------------------------------------
 *  Function:     VLPR_mode
 *  Brief:        Enter device in to Very low power run mode
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void VLPR_mode(void)
{
	Core_clock_selection = LOW_POWER_MODE;
    ui_timer_de_init();
    printf("\nVLPR  mode\n");
    /* Switch clock configuration */
    if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_1))
    {
      printf("Cannot change clock configuration");
      _task_block();
    }
    
    printf("\nChanging frequency to 2 MHz.\n");
    if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2MHZ))
    {
        printf("Cannot change clock configuration");
    }
//    _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2MHZ);
    _lpm_set_operation_mode(LPM_OPERATION_MODE_WAIT);
    Disable_All_Button_ISR();
    Button_Init();
    ui_timer_init();
    run_mode_flag = 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     run_mode
 *  Brief:        Enter device in to normal run mode
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void run_mode(void)
{
	
	ui_timer_de_init();
	_lpm_set_operation_mode(LPM_OPERATION_MODE_RUN);
	PORTE_PCR24= 0x00;PORTE_PCR25= 0x00;   

	if (CM_ERR_OK != _lpm_set_clock_configuration(4))
	{
		printf("Cannot change clock configuration");
		_task_block();
	}
	
	printf("\nrun mode success\n");
	Disable_All_Button_ISR();
	Button_Init();
	ui_timer_init();
	
	Core_clock_selection = IDLE_MODE_MODE;
}


/*-----------------------------------------------------------------------------
 *  Function:     run_mode_for_BLE
 *  Brief:        Enter device in to normal run mode
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void run_mode_for_BLE(void)
{
	
//	ui_timer_de_init();
	_lpm_set_operation_mode(LPM_OPERATION_MODE_RUN);
	PORTE_PCR24= 0x00;PORTE_PCR25= 0x00;   

//	_time_get(&start_time);
	
	if (CM_ERR_OK != _lpm_set_clock_configuration(4))
	{
		printf("Cannot change clock configuration");
		_task_block();
	}
	ui_timer_de_init();
//	_time_get(&end_time);
//	_time_diff(&start_time, &end_time, &diff_time);
//	printf("\nTime : %ld sec %ld millisec\n",diff_time.SECONDS, diff_time.MILLISECONDS);
	
//	printf("\nrun mode success\n");
	Disable_All_Button_ISR();
	Button_Init();
	ui_timer_init();
	
	Core_clock_selection = IDLE_MODE_MODE;
}
/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
