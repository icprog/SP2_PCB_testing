/*
 * adc_k70.h
 *
 *  Created on: Jan 7, 2014
 *      Author: user
 */

#ifndef ADC_K70_H_
#define ADC_K70_H_
#define ADC1 "adc1:" /* must be #1 as the inputs are wired to ADC 1 */
#define ADC0 "adc0:" 
#define ADC2 "adc2:" 
#define MY_TRIGGER ADC_PDB_TRIGGER
extern MQX_FILE_PTR 			ir_sens,force_sens,ros_sens1,ros_sens2,batt_sens,batt_current,batt_temp;
extern void ADC_Init();
extern void ADC_deinit(void);
extern uint_16 Read_Battery(void);
void Battery_ADC_Init();
void Battery_ADC_Deinit();
void Test_ADC(void);
extern const ADC_INIT_CHANNEL_STRUCT force_sensor_para;
extern const ADC_INIT_STRUCT adc_init;
#endif /* ADC_K70_H_ */
