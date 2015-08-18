/*
 * adc_k70.h
 *
 *  Created on: Jan 7, 2014
 *      Author: user
 */

#ifndef ADC_K70_H_
#define ADC_K70_H_


/************************* DEFINES CONSTANTS **********************************/

#define ADC1 					"adc1:" /* must be #1 as the inputs are wired to ADC 1 */
#define ADC0 					"adc0:" 
#define ADC2 					"adc2:" 

#define ADC_VREF_EN_PIN   		(GPIO_PORT_C|GPIO_PIN8)
#define ADC_VREF_EN_PIN_MUX  	(LWGPIO_MUX_C8_GPIO)
#define BAT_MON_EN_PIN   		(GPIO_PORT_A|GPIO_PIN15)
#define BAT_MON_EN_PIN_MUX  	(LWGPIO_MUX_A15_GPIO)
#define MY_TRIGGER 				ADC_PDB_TRIGGER

/************************* GLOBAL VARIABLES **********************************/
extern MQX_FILE_PTR 			ir_sens,force_sens,ros_sens1,ros_sens2,batt_sens,batt_current,batt_temp;

/*************************GLOBAL FUNCTION DEFINITIONS ************************/

void 	ADC_Init();
void	ADC_deinit(void);
void 	Battery_ADC_Init();
void 	Battery_ADC_Deinit();
float 	Read_Battery_Capacity(void);

#endif /* ADC_K70_H_ */
