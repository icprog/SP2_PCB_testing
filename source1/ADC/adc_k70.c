
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
#include "Sensor_Configuration.h"
#include "batt_level_algorithm.h"

#define ADC_VREF_EN_PIN   		(GPIO_PORT_C|GPIO_PIN8)
#define ADC_VREF_EN_PIN_MUX  	(LWGPIO_MUX_C8_GPIO)
#define BAT_MON_EN_PIN   		(GPIO_PORT_A|GPIO_PIN15)
#define BAT_MON_EN_PIN_MUX  	(LWGPIO_MUX_A15_GPIO)

LWGPIO_STRUCT ADC_VREF_EN;
LWGPIO_STRUCT BAT_MON_EN;

MQX_FILE_PTR 		fd_adc0,fd_adc1,fd_adc2 ,ir_sens,force_sens,ros_sens1,batt_sens,batt_current,batt_temp,ros_sens2;	
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
		25,            /* time offset from trigger point in us */
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
		5,         	/* time offset from trigger point in us */
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
		10,           /* number of samples in one run sequence */
		15,            /* time offset from trigger point in us */
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
		25,            /* time offset from trigger point in us */
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
		5,            /* time offset from trigger point in us */  //TODO: given same as optical_sensor_para 30, Change if needed.
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
		15,            /* time offset from trigger point in us */ //TODO: given same as battery_voltage_para 30, Change if needed.
		80,          /* period in us (= 0.3 sec) */
		0x10000,      /* scale range of result (not used now) */
		5,           /* circular buffer size (sample count) */
		MY_TRIGGER,   /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
		NULL

#endif
};

/************************* FUNCTION DEFINITIONS ************************/

void Stop_PDB_Timer(void);
static void Read_Battery_Data(float *,float *,float *);

/*-----------------------------------------------------------------------------
 *  Function:     Battery_Monitor_Init
 *  Brief:        This functions initialise Battery monitor GPIO pins.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Battery_Monitor_Init()
{
	if (!lwgpio_init(&BAT_MON_EN, BAT_MON_EN_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
	{
		printf("Initializing GPIO with associated pins failed.\n");
	}

	lwgpio_set_functionality(&BAT_MON_EN, BAT_MON_EN_PIN_MUX); /*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_LOW);
}

/*-----------------------------------------------------------------------------
 *  Function:     Battery_Monitor_Enable
 *  Brief:        This functions Enables Battery monitor GPIO pins.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Battery_Monitor_Enable()
{
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_HIGH);
}

/*-----------------------------------------------------------------------------
 *  Function:     Battery_Monitor_Disable
 *  Brief:        This functions Disables Battery monitor GPIO pins.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Battery_Monitor_Disable()
{
	lwgpio_set_value(&BAT_MON_EN, LWGPIO_VALUE_LOW);
}

/*-----------------------------------------------------------------------------
 *  Function:     ADC_Vref_Init
 *  Brief:        This functions Initialise the ADC reference GPIO pins.
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
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
	
#if ENABLE_ROS2	
	//	printf("Opening Reflective Object sensor 2 channel  ...");
	ros_sens2= fopen(ADC0 "three", (const char*)&ROS_2_para);
	if(ros_sens2 == NULL)
	{    
		printf("ros_sens2 init failed\n");
	}
#endif
	
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

	
		
//	printf("Triggering all channels...");
	ioctl(fd_adc0, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	ioctl(fd_adc1, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
	ioctl(fd_adc2, ADC_IOCTL_FIRE_TRIGGER, (pointer) MY_TRIGGER);
}  


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
	ioctl(fd_adc2, IOCTL_ADC_STOP_CHANNELS, (pointer) MY_TRIGGER);
	
	
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
		printf("\nros_sens1 closed successfully\n");
	}
	else
	{
		printf("\nros_sens1 close Error\n");
	}
	
#if ENABLE_ROS2		
	if(MQX_OK==fclose(ros_sens2))
	{
		ros_sens1=NULL;
		printf("\nros_sens2 closed successfully\n");
	}
	else
	{
		printf("\nros_sens2 close Error\n");
	}
#endif
	
	/***********************************************************************/
	if(MQX_OK==fclose(ir_sens))
	{
		ir_sens=NULL;	
		printf("\nir_sens closed successfully\n");
	}
	else
	{
		printf("\nir_sens close Error\n");
	}
	
	/***********************************************************************/
	if(MQX_OK==fclose(force_sens))
	{
		force_sens=NULL;	
		printf("\nforce sensor closed successfully\n");
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
static void Read_Battery_Data(float *volt,float *curr,float *temp)
{
	uint_32 Volt_adc_res = 0;
	uint_32 Curr_adc_res = 0;
	uint_32 Temp_adc_res = 0;
	
	static float  Volt_Prev = 2.9;
	static float  Curr_Prev = 0.75;
	static float  Temp_Prev = 1.40;
	
	uint8_t Voltsample_count = 0;
	uint8_t Currsample_count = 0;
	uint8_t Tempsample_count = 0;
	
	ADC_RESULT_STRUCT adc_out;	
	
	/* Reading 5 samples of Battery parameters*/
	Time_Delay_Sleep(5);
	for(int i=0;i<5;i++)
	{
//		_time_delay(2);
		adc_out.result = 0;
		if (read(batt_sens, &adc_out,sizeof(adc_out)))
		{
			Volt_adc_res += adc_out.result;	
			Voltsample_count++;
		}
		
		adc_out.result = 0;
		if (read(batt_current, &adc_out,sizeof(adc_out)))
		{
			Curr_adc_res += adc_out.result;	
			Currsample_count++;
		}
		
		adc_out.result = 0;
		if (read(batt_temp, &adc_out,sizeof(adc_out)))
		{
			Temp_adc_res += adc_out.result;	
			Tempsample_count++;
		}
	}
	
	/* Average the 5 samples*/
	if( (Voltsample_count != 0) && ( Currsample_count != 0)  && (Tempsample_count != 0))
	{
		Volt_adc_res /= Voltsample_count;
		Curr_adc_res /= Currsample_count;
		Temp_adc_res /= Tempsample_count;
		*volt = Volt_Prev = Volt_adc_res * BATTERY_VOLTAGE_MULTIPILER;
		*curr = Curr_Prev = Curr_adc_res * BATTERY_VOLTAGE_MULTIPILER;
		*temp = Temp_Prev = Temp_adc_res * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
	}
	else
	{
		*volt = Volt_Prev; 
		*curr = Curr_Prev;
		*temp = Temp_Prev;			
	}
	
	*curr = (*curr/200)/(0.05); //The amplifier used for the current sense resistor is the Texas Instruments INA216A4RSWR. 
								//This has a gain of 200. The sense resistor is 0.05 Ohms.So the Current Value = Output/200/0.05

	*temp = ((1.8583 - *temp)/11.67) * 1000;  // Temp –40 to +85°C 

}

/*-----------------------------------------------------------------------------
 *  Function:     Read_Battery_Capacity
 *  Brief:        Calculates battery capacity from Raw battery data. 
 *  Parameter:    None
 *  Return:       Battery Capacity
-----------------------------------------------------------------------------*/
float Read_Battery_Capacity(void)
{
	float volt = 0.0;
	float curr = 0.0;
	float temp = 0.0;
	float Battery_Capacity = 0.0;
	float capacityBin = 0.0;
	
	Read_Battery_Data(&volt,&curr,&temp);
	triBattInterp( temp, curr, volt, &Battery_Capacity,&capacityBin);
	return Battery_Capacity;
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

