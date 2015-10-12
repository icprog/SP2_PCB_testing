
/**HEADER********************************************************************
 *
 * $FileName: adc_k70.c$
 * $Version : 3.8.35.0$
 * $Date    : Nov-10-2013$
 *
 * Comments:
 *
 *   This file contains the source for the ADC program.
 *   Four channels are running for pressure,depth,QRD and Battery sensor.
 *
 *END************************************************************************/

#include "common_headers.h"
#include "adc_mk70.h"

#define ADC_VREF_EN_PIN   		(GPIO_PORT_C|GPIO_PIN8)
#define ADC_VREF_EN_PIN_MUX  	(LWGPIO_MUX_C8_GPIO)
#define BAT_MON_EN_PIN   		(GPIO_PORT_A|GPIO_PIN15)
#define BAT_MON_EN_PIN_MUX  	(LWGPIO_MUX_A15_GPIO)

#define ENABLE_ROS2				(1)

LWGPIO_STRUCT ADC_VREF_EN;
LWGPIO_STRUCT BAT_MON_EN;

MQX_FILE_PTR 		fd_adc0,fd_adc1,fd_adc2 ,ir_sens,force_sens,ros_sens1,ros_sens2,batt_sens,batt_current,batt_temp;	
uint_8 Batt_Adc_Not_Need_Flag=0;

/* ADC device init struct */
const ADC_INIT_STRUCT adc_init = {
		ADC_RESOLUTION_DEFAULT,    /* resolution */
};


/* Distance sensor channel #1 init struct */
const ADC_INIT_CHANNEL_STRUCT ir_sensor_para = 
{

		ADC1_SOURCE_AD1 ,  // ADC1DP1 = D29
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED, /*ADC_CHANNEL_MEASURE_LOOP runs continuously after IOCTL trigger */
		5,           /* number of samples in one run sequence */
		5,            /* time offset from trigger point in us */
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL
		//			&evn,
		//			0x01            /* mask of event to be set */
#endif
};


/* Force channel #2 init struct */
const ADC_INIT_CHANNEL_STRUCT force_sensor_para = 
{
		ADC2_SOURCE_AD0, // ADC2DP0 = ?
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED,//ADC_CHANNEL_MEASURE_ONCE | ADC_CHANNEL_START_NOW, /* one sequence is sampled after fopen */
		5,             /* number of samples in one run sequence */
		10,         	/* time offset from trigger point in us */
		80,         	/* period in us (= 0.6 sec) */
		0x10000,        /* scale range of result (not used now) */
		5,             /* circular buffer size (sample count) */
		MY_TRIGGER,     /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL
#endif
};

/* Battery channel #3 init struct */
const ADC_INIT_CHANNEL_STRUCT battery_voltage_para = 
{
		ADC1_SOURCE_AD16,	// ADC1SE16 = 
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED,//ADC_CHANNEL_MEASURE_ONCE | ADC_CHANNEL_START_NOW, /* one sequence is sampled after fopen */
		5,             /* number of samples in one run sequence */
		15,         	/* time offset from trigger point in us */
		80,         	/* period in us (= 0.6 sec) */
		0x10000,        /* scale range of result (not used now) */
		5,             /* circular buffer size (sample count) */
		MY_TRIGGER,     /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL
#endif
};

/* ROS 1 channel #4 init struct */
const ADC_INIT_CHANNEL_STRUCT optical_sensor_para = 
{

		ADC0_SOURCE_AD0 ,  // ADC0DP0
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED, /*ADC_CHANNEL_MEASURE_LOOP runs continuously after IOCTL trigger */
		5,           /* number of samples in one run sequence */
		20,            /* time offset from trigger point in us */
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL

#endif
};

/* QRD channel #5 init struct */
const ADC_INIT_CHANNEL_STRUCT ROS_2_para = 
{

		ADC0_SOURCE_AD3 ,  // ADC0DP0
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED, /*ADC_CHANNEL_MEASURE_LOOP runs continuously after IOCTL trigger */
		5,           /* number of samples in one run sequence */
		30,            /* time offset from trigger point in us */
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL

#endif
};

/* Current Sens channel #5 init struct */
const ADC_INIT_CHANNEL_STRUCT Batt_current_para = 
{

		ADC0_SOURCE_AD23 ,  // ADC0DP0
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED, /*ADC_CHANNEL_MEASURE_LOOP runs continuously after IOCTL trigger */
		5,           /* number of samples in one run sequence */
		10,            /* time offset from trigger point in us */  //TODO: given same as optical_sensor_para 30, Change if needed.
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL

#endif
};

/* QRD channel #5 init struct */
const ADC_INIT_CHANNEL_STRUCT Batt_temp_para = 
{

		ADC1_SOURCE_AD23  ,  // ADC0DP0
		ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED, /*ADC_CHANNEL_MEASURE_LOOP runs continuously after IOCTL trigger */
		5,           /* number of samples in one run sequence */
		25,            /* time offset from trigger point in us */ //TODO: given same as battery_voltage_para 30, Change if needed.
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL

#endif
};

void Stop_PDB_Timer(void);

void Battery_Monitor_Init()
{
	if (!lwgpio_init(&BAT_MON_EN, BAT_MON_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&BAT_MON_EN, BAT_MON_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_LOW);
}

void Battery_Monitor_Enable()
{
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_HIGH);
}

void Battery_Monitor_Disable()
{
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_LOW);
}

void ADC_Vref_Init()
{
	if (!lwgpio_init(&ADC_VREF_EN, ADC_VREF_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&ADC_VREF_EN, ADC_VREF_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&ADC_VREF_EN, LWGPIO_VALUE_LOW);
}

void ADC_Vref_Enable()
{
	lwgpio_set_value(&ADC_VREF_EN, LWGPIO_VALUE_HIGH);
}


void ADC_Vref_Disable()
{
	lwgpio_set_value(&ADC_VREF_EN, LWGPIO_VALUE_LOW);
}


/*-----------------------------------------------------------------------------
 *  Function:     ADC_deinit
 *  Brief:        De initialise all ADC channels
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void ADC_deinit(void)
{
	Batt_Adc_Not_Need_Flag = 1;
    /* Stops ADC channel 0 */
	ioctl(fd_adc0, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	/* Stops ADC channel 1 */
	ioctl(fd_adc1, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	/* Stops ADC channel 1 */
	ioctl(fd_adc2, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	/* Stops PDB Timer to avoid further Triggering ADC channels */
//	Stop_PDB_Timer();
	
	/********************************* De init Battery Sensor channel **************************************/
	if(batt_sens != NULL)
	{
		if(MQX_OK !=fclose(batt_sens))
		{
			printf("\n Battery Voltage sensor close Error\n");
		}
	}
	if(batt_current != NULL)
	{
		if(MQX_OK !=fclose(batt_current))
		{
			printf("\n Battery Current sensor close Error\n");
		}
	}
	if(batt_temp != NULL)
	{
		if(MQX_OK !=fclose(batt_temp))
		{
			printf("\n Battery Temperature sensor close Error\n");
		}
	}
	
	/********************************** De init optical Sensor channel*************************************/
	if(ros_sens1 != NULL)
	{    
		if(MQX_OK !=fclose(ros_sens1))
		{
			printf("\nros_sens1 close Error\n");
		}
	}
#if ENABLE_ROS2	
	if(ros_sens2  != NULL)
	{    
		if(MQX_OK != fclose(ros_sens2))
		{
			printf("\nros_sens2 close Error\n");
		}
	}
#endif	

	/**********************************De init Distance Sensor channel*************************************/
	if(ir_sens != NULL)
	{
		if(MQX_OK !=fclose(ir_sens))
		{
			printf("\nir_sens close Error\n");
		}
	}

	/********************************** De init Force Sensor channel*************************************/
	if(force_sens!=NULL)
	{
		if(MQX_OK !=fclose(force_sens))
		{
			printf("\nforce sensor close Error\n");
		}
	}
	/*********************************** Close ADC Module 0 ************************************/
	if(MQX_OK !=fclose(fd_adc0))
	{
		printf("\nADC0 close Error\n");
	}

	/*********************************** Close ADC Module 1 ************************************/
	if(MQX_OK !=fclose(fd_adc1))
	{
		printf("\nADC1 close Error\n");
	}
	

	/*********************************** Close ADC Module 2 ************************************/
	if(MQX_OK !=fclose(fd_adc2))
	{
		printf("\nADC2 close Error\n");
	}
	
	Battery_Monitor_Disable();
	ADC_Vref_Disable();
	
}  

void Battery_ADC_Init()
{
	Batt_Adc_Not_Need_Flag = 0;
	Battery_Monitor_Init();
	Battery_Monitor_Enable();	
	
	ADC_Vref_Init();
	ADC_Vref_Enable();
	
//################################### Opening ADC modules #################################	
	//	printf("Opening ADC 1  device ...");
	fd_adc1 = fopen(ADC1, (const char*)&adc_init);
	if(fd_adc1  != NULL)
	{    
		//		printf("done\n");
	}
	else
	{    
		printf("ADC1 failed\n");
		_task_block();
	}

	//	printf("Opening ADC 0  device ...");
	fd_adc0 = fopen(ADC0, (const char*)&adc_init);
	if(fd_adc0  != NULL)
	{    
		//		printf("done\n");
	}
	else
	{    
		printf("ADC0 failed\n");
		_task_block();
	}

//################################### Opening Battery ADC channels #################################
	
//	printf("Opening  Battery sensor channel  ...");
	batt_sens = fopen(ADC1 "twenty two", (const char*)&battery_voltage_para); /* adc:2 is running now */	
	if(batt_sens != NULL)
	{    
	}
	else
	{    
		printf("Battery Voltage ADC pin init failed\n");
	}

	//printf("Opening Battery Current sensor channel  ...");
	batt_current = fopen(ADC0 "twenty three", (const char*)&Batt_current_para);
	if(ros_sens1 != NULL)
	{    
	}
	else
	{    
		printf("Battery Current ADC pin init failed\n");
	}

	//	printf("Opening Battery Temperature sensor channel  ...");
	batt_temp = fopen(ADC1 "twenty three", (const char*)&Batt_temp_para); /* adc:2 is running now */	
	if(batt_sens != NULL)
	{    
	}
	else
	{    
		printf("Battery Temperature ADC pin init failed\n");
	}
	
	printf("Triggering Battery channel...");
	ioctl(fd_adc0, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	ioctl(fd_adc1, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	
}


void Battery_ADC_Deinit()
{
    /* Stops ADC channel 0 */
	ioctl(fd_adc0, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	ioctl(fd_adc1, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	/* Stops PDB Timer to avoid further Triggering ADC channels */
//	Stop_PDB_Timer();
	
	if(batt_sens != NULL)
	{
		if(MQX_OK !=fclose(batt_sens))
		{
			printf("\n Battery Voltage sensor close Error\n");
		}
	}
	if(batt_current != NULL)
	{
		if(MQX_OK !=fclose(batt_current))
		{
			printf("\n Battery Current sensor close Error\n");
		}
	}
	if(batt_temp != NULL)
	{
		if(MQX_OK !=fclose(batt_temp))
		{
			printf("\n Battery Temperature sensor close Error\n");
		}
	}
/***************	Closing ADC modules	*****************************/	
	if(MQX_OK !=fclose(fd_adc1))
	{
		printf("\nADC1 close Error\n");
	}
	
	if(MQX_OK !=fclose(fd_adc0))
	{
		printf("\nADC1 close Error\n");
	}
	
	Batt_Adc_Not_Need_Flag = 1;
	Battery_Monitor_Disable();
	ADC_Vref_Disable();
}

/*-----------------------------------------------------------------------------
 *  Function:     ADC_Init
 *  Brief:        initialise Battery sensor ADC channel and start adc. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void ADC_Init(void)
{
	Batt_Adc_Not_Need_Flag = 0;
	Battery_Monitor_Init();
	Battery_Monitor_Enable();
	
	ADC_Vref_Init();
	ADC_Vref_Enable();
		
//	printf("Opening ADC 1 device ...");
	fd_adc1 = fopen(ADC1, (const char*)&adc_init);
	if(fd_adc1 != NULL)
	{    
	}
	else
	{    
		printf("ADC1 failed\n");
		_task_block();
	}
	/********************************************/

//	printf("Opening ADC 0  device ...");
	fd_adc0 = fopen(ADC0, (const char*)&adc_init);
	if(fd_adc0  != NULL)
	{    
	}
	else
	{    
		printf("ADC0 failed\n");
		_task_block();
	}
	
	
//	printf("Opening ADC 2  device ...");
	fd_adc2 = fopen(ADC2, (const char*)&adc_init);
	if(fd_adc2  != NULL)
	{    
	}
	else
	{    
		printf("ADC2 failed\n");
		_task_block();
	}
		
		
//	printf("Opening IRDM Sensor channel  ...");
	ir_sens = fopen(ADC1 "one", (const char*)&ir_sensor_para);
	if(ir_sens != NULL)
	{    
	}
	else
	{    
		printf("IRDM failed\n");
	}

//	printf("Opening Force sensor channel  ...");
	force_sens = fopen(ADC2 "zero", (const char*)&force_sensor_para); /* adc:2 is running now */	
	if(force_sens != NULL)
	{    
	}
	else
	{    
		printf("Force failed\n");
	}
	
/***************Configuring Reflective Object sensor*********************/
	

		
//	printf("Opening Reflective Object sensor 1 channel  ...");
	ros_sens1= fopen(ADC0 "zero", (const char*)&optical_sensor_para);
	if(ros_sens1 != NULL)
	{    
	}
	else
	{    
		printf("ros_sens1 init failed\n");
	}
		
	//	printf("Opening Reflective Object sensor 2 channel  ...");
	ros_sens2= fopen(ADC0 "three", (const char*)&ROS_2_para);
	if(ros_sens2 == NULL)
	{    
		printf("ros_sens2 init failed\n");
	}
	
/***************Configuring Battery ADC pins*********************/ 
	
//	printf("Opening  Battery sensor channel  ...");
	batt_sens = fopen(ADC1 "twenty two", (const char*)&battery_voltage_para); /* adc:2 is running now */	
	if(batt_sens != NULL)
	{    
	}
	else
	{    
		printf("Battery Voltage ADC init failed\n");
	}
	//	printf("Opening Battery Current sensor channel  ...");
	batt_current = fopen(ADC0 "twenty three", (const char*)&Batt_current_para);
	if(ros_sens1 != NULL)
	{    
	}
	else
	{    
		printf("Battery Current ADC init failed\n");
	}
	//	printf("Opening Battery Temperature sensor channel  ...");
	batt_temp = fopen(ADC1 "twenty three", (const char*)&Batt_temp_para); /* adc:2 is running now */	
	if(batt_sens != NULL)
	{    
	}
	else
	{    
		printf("Battery failed\n");
	}
	

		
	printf("Triggering all channels...\n");
	ioctl(fd_adc0, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	ioctl(fd_adc1, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	ioctl(fd_adc2, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
}  

/*-----------------------------------------------------------------------------
 *  Function:     Start_ADC
 *  Brief:        Configure and starts Force,QRD and depth sensor ADC channel. 
 	              Stops Battery ADC channel.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
//void Start_ADC(void)
//{   
////    Batt_Adc_Not_Need_Flag=1;
//
//	/********************************** Stops Battery Sensor channel*************************************/
//    
//
//    
////	ioctl(fd_adc0, ADC_IOCTL_STOP_CHANNEL, (pointer) MY_TRIGGER);
////	if(MQX_OK==fclose(batt_sens))
////	{
////		batt_sens=NULL;
////		printf("\n Battery sensor closed succesfully\n");
////	}
////	else
////	{
////		printf("\n Battery sensor close Error\n");
////	}
//	
//	/********************************** Configure optical Sensor channel*************************************/
//	printf("Opening IRDM Sensor channel  ...");
//	ir_sens = fopen(ADC1 "zero", (const char*)&ir_sensor_para);
//	if(ir_sens != NULL)
//	{    
//		printf("done, prepared to start by trigger\n");
//	}
//	else
//	{    
//		printf("failed\n");
//	}
//
//	/********************************** Configure QRD Sensor channel*************************************/
//	printf("Opening Reflective Object sensor channel  ...");
//	optical_sens= fopen(ADC1 "sixteen", (const char*)&optical_sensor_para);
//	if(optical_sens != NULL)
//	{    
//		printf("done, prepared to start by trigger\n");
//	}
//	else
//	{    
//		printf("failed\n");
//	}
//	
//	/********************************** Configure Force Sensor channel*************************************/
//	printf("Opening Force sensor channel  ...");
//	force_sens = fopen(ADC0 "zero", (const char*)&force_sensor_para); /* adc:2 is running now */	
//	if(force_sens != NULL)
//	{    
//		printf("done, one sequence started automatically\n");
//	}
//	else
//	{    
//		printf("failed\n");
//	}
//
//	
//	/********************************** Configure Battery Sensor channel*************************************/	
//	
//	printf("Opening  Battery sensor channel  ...");
//	batt_sens = fopen(ADC1 "thirteen", (const char*)&battery_sensor_para); /* adc:2 is running now */	
//	if(batt_sens != NULL)
//	{    
//		printf("done, one sequence started automatically\n");
//	}
//	else
//	{    
//		printf("failed\n");
//	}
//    
////	Batt_Adc_Not_Need_Flag=0;
////	ioctl(fd_adc0, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
////	printf("Triggering Battery sensor ...");
//    
//	/********************************** Trigger Force,depth and QRD Sensor*************************************/
//	ioctl(fd_adc1, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
//	ioctl(fd_adc0, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
//}


/*-----------------------------------------------------------------------------
 *  Function:     Stop_ADC
 *  Brief:        Configure and starts Battery channel. Stops Force,QRD and depth sensor channels.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Stop_ADC(void)
{

	ioctl(fd_adc0, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	ioctl(fd_adc1, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	
	
		ioctl(fd_adc0, ADC_IOCTL_STOP_CHANNEL, (pointer) MY_TRIGGER);
		if(MQX_OK==fclose(batt_sens))
		{
			batt_sens=NULL;
			printf("\n Battery sensor closed successfully\n");
		}
		else
		{
			printf("\n Battery sensor close Error\n");
		}
	
	/***********************************************************************/
	if(MQX_OK==fclose(ros_sens1))
	{
		ros_sens1=NULL;
		printf("\noptical_sens closed successfully\n");
	}
	else
	{
		printf("\noptical_sens close Error\n");
	}
	
	/***********************************************************************/
	if(MQX_OK==fclose(ir_sens))
	{
		ir_sens=NULL;	
		printf("\nir_sens closed succesfully\n");
	}
	else
	{
		printf("\nir_sens close Error\n");
	}
	
	/***********************************************************************/
	if(MQX_OK==fclose(force_sens))
	{
		force_sens=NULL;	
		printf("\nforce sensor closed succesfully\n");
	}
	else
	{
		printf("\nforce sensor close Error\n");
	}

}



/*-----------------------------------------------------------------------------
 *  Function:     Read_Battery
 *  Brief:        Provides voltage across battery 
 *  Parameter:    None
 *  Return:       Battery Voltage in Hex format
-----------------------------------------------------------------------------*/
uint_16 Read_Battery(void)
{
	static uint_16 Adc_Prev=0;
	static uint_8  adc_check=0;

	uint_32 adc_res=0;
	
	uint_8 	i=0;
	uint_8 sample_count=0;

	ADC_RESULT_STRUCT adc_out;

	/********************************* Takes the average values of Battery Voltage***********************************/
	if(Batt_Adc_Not_Need_Flag == 0)
	{
		for(i=0;i<5;)
		{
			
			if (read(batt_sens, &adc_out,sizeof(adc_out)))
			{
//				printf("Battery Sensor ADC = %d ",adc_out.result);
				adc_res+= adc_out.result;	
				sample_count++;
				i++;
			}
		}
		if(sample_count!=0)
		{
			adc_res/=sample_count;
			adc_check=0;
		}
		else
		{
			adc_check++;
			if(adc_check>4)
			{
				adc_check=0;
				return 0;
			}

			return Adc_Prev;
		}
		Adc_Prev=adc_res;
	
		/*********************** Returns current battery voltage *****************************/
		return (adc_res);
	}
	else
	{
		/*********************** Returns previous battery voltage *****************************/
		return Adc_Prev;
	}

}

/*-----------------------------------------------------------------------------
 *  Function:     Stop_PDB_Timer
 *  Brief:        Stops PDB timer to avoid further triggering of ADC channels. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Stop_PDB_Timer(void)
{
    /* Disable PDB Timer*/
    PDB0_SC &= ~PDB_SC_PDBEN_MASK;
    /* Disable PDB Interrupt*/
    PDB0_SC &= ~PDB_SC_PDBIE_MASK;
}

/*-----------------------------------------------------------------------------
 *  Function:     Start_PDB_Timer
 *  Brief:        Starts PDB timer to  trigger ADC channels. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Start_PDB_Timer(void)
{
    /* Clearing seq.error Flag*/
    PDB0_CH0S =0x00;
    /* Enables PDB Timer and Interrupt*/
    PDB0_SC |= PDB_SC_PDBEN_MASK | PDB_SC_PDBIE_MASK ;
    /* Restarts PDB*/
    PDB0_SC |= PDB_SC_SWTRIG_MASK ;

}

/*-----------------------------------------------------------------------------
 *  Function:     Test_ADC
 *  Brief:        Displays all ADC channels output. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Test_ADC(void)
{
	printf("\n***********STARTING ADC TEST***********\n");
	uint8_t adc_result = 0;
	uint_16 raw_pressure = 0, raw_depth = 0,raw_ros1 = 0, raw_ros2 = 0,raw_Batt_volt = 0,
				raw_Batt_curr = 0,raw_Batt_temp = 0;
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("ADC TEST",strlen("ADC TEST"));
	_time_delay(1000);
	Draw_string_new(10,80,"STARTING ADC TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	power_rail_enable();
	
	for(int i = 0; i < 10;i++)
	{
		adc_result += Get_ADC_Outputs(&raw_pressure,&raw_depth,&raw_ros1,&raw_ros2,
						&raw_Batt_volt,&raw_Batt_curr,&raw_Batt_temp);	
	}
	if(adc_result == 0)
	{
		Draw_string_new(10,200,"ADC TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
	}
	else
	{
		Draw_string_new(10,200,"ADC TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
	}
	printf("\n***********COMPLETED BUZZER TEST***********\n");
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	

}
