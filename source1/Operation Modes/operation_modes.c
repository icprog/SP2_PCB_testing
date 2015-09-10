/*
 * operation_modes.c
 *
 *  Created on: Jul 1, 2014
 *      Author: user
 */


#include <mqx.h>
#include <bsp.h>
#include <psp.h>
#include "operation_modes.h"
#include "spi_lcd.h"
#include "config.h"




/*-----------------------------------------------------------------------------* 
 * Function:    Switch_acquisition_mode
 * Brief:       This function will run the core at high clock frequency.  
 * Parameter:   None
 * Return:      None 
 -----------------------------------------------------------------------------*/
void Switch_acquisition_mode(void)
{
//	lcd_display_deinit();
	
    if (MQX_OK == fclose(spifd))
    {
        printf("\nSP2 closed succesfully\n");
    }
    else
    {
        printf("\nSP2 close Error\n");
    }
	//	De_init_lcd_SPI();
		Core_clock_selection = ACQUISITION_MODE;	
		
		/* De_init UI timer to sinc with 120MHz clock configuration */
		ui_timer_de_init();
		
		/* Switch clock configuration */
		if (CM_ERR_OK != _lpm_set_clock_configuration(0))
		{
			printf("Cannot change clock configuration");
			_task_block();
		}
		
	    /* ddr init */
	//	ddr_init();
		
	//	Set_lcd_buad_rate(LCD_BAUD_AT_ACQUISITION);
		
		/* Change lcd buad rate for acquisition */
		Upadate_lcd_settings(LCD_BAUD_AT_ACQUISITION);
		
		/* Init UI timer to sinc with 120MHz clock configuration */
		ui_timer_init();
		
		printf("Initialising LCD...\n");
//		spi_init(); /*Init SPI configuration*/
//		lcd_clear(); /* Clearing LCD */
//		buff_clear();
		
	//	/* Init HW timer for acquisition */
//	    Init_Hw_Timer();
		
		Set_lcd_buad_rate(LCD_BAUD_AT_ACQUISITION);
	        
}


/*-----------------------------------------------------------------------------* 
 * Function:    Switch_acquisition_mode
 * Brief:       This function will run the core at high clock frequency.  
 * Parameter:   None
 * Return:      None 
 -----------------------------------------------------------------------------*/
void Switch_to_idle_mode(void)
{
	Core_clock_selection = IDLE_MODE_MODE;
	
//		De_init_lcd_SPI();
//		lcd_display_deinit();
		
	    if (MQX_OK == fclose(spifd))
	    {
	        printf("\nSP2 closed succesfully\n");
	    }
	    else
	    {
	        printf("\nSP2 close Error\n");
	    }
		/* disable ddr clock */
	//	printf("\nDisabling DDR..\n");
	//	ddr_disable();
		
		/* De_init UI timer to sinc with 24MHz clock configuration */
	//	printf("\nDe_init UI timer..\n");
		ui_timer_de_init();

		/* De_init HW timer  */
	//	printf("\nDe_init HW timer..\n");
//		De_Init_Hw_Timer();

		/* Switch clock configuration */
		if (CM_ERR_OK != _lpm_set_clock_configuration(2))
		{
			printf("Cannot change clock configuration");
			_task_block();
		}
		Time_Delay_Sleep(10);
		/* Switch clock configuration */
		if (CM_ERR_OK != _lpm_set_clock_configuration(4))
		{
			printf("Cannot change clock configuration");
			_task_block();
		}

		
		
		/* Init UI timer to sinc with 24MHz clock configuration */
		ui_timer_init();
				
		/* Change lcd buad rate for idle */
		Upadate_lcd_settings(LCD_BAUD_AT_IDLE);
		printf("Initialising LCD...\n");
//		spi_init(); /*Init SPI configuration*/
//		lcd_clear(); /* Clearing LCD */
//		buff_clear();
		
		Set_lcd_buad_rate(LCD_BAUD_AT_IDLE);
}


void Run_in_idle_mode(void)
{
	Core_clock_selection = IDLE_MODE_MODE;
	/* Switch clock configuration */
	if (CM_ERR_OK != _lpm_set_clock_configuration(2))
	{
		printf("Cannot change clock configuration");
		_task_block();
	}

	/* Switch clock configuration */
	if (CM_ERR_OK != _lpm_set_clock_configuration(4))
	{
		printf("Cannot change clock configuration");
		_task_block();
	}
}

