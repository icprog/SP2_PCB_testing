/*
 * calib.c
 *
 *  Created on: Dec 8, 2014
 *      Author: Rahul
 */

#include "common_headers.h"
#include "matrix_operations.h"


extern void Write_IRDMS_Data(void);
extern void Write_Pressure_Data(void);
extern void Write_ROS_Data(void);
extern void Write_GPS_Data(uint_32  sec);
extern void Write_MAG_Data(void);
extern void Write_ACC_Data(void);
extern void Update_Calib_Buff();

//This variable is incremented and decremented according to navigation key 
volatile uint_8 IRDMS_Condition_selection = 0;
volatile uint_8 Pressure_Condition_selection = 0;
volatile uint_8 ROS_Condition_selection = 0;
volatile uint_8 Accelerometer_Condition_selection = 0;
volatile uint_8 Magnetometer_Condition_selection = 0;
volatile uint_8 Acc_reading_status = 0;


CalibTable IRDMS_Calib_Table[] = {

		//  condition       min voltage,    max voltage	
		{ 	"COVERED",      	1.2,        	1.4,	5.5,	INCOMPLETE }, 
		{ 	"POLE LENGTH", 		0.3, 		 	0.6,	5.5,	INCOMPLETE }, 

};

CalibTable Pressure_Calib_Table[] = {

		//  condition       min voltage,    max voltage	
		{ 	"0.0 N",      	0.45,        	0.75,	5.5,	INCOMPLETE }, 
		{ 	"1.0 N", 		0.0,     		0.5,	5.5,	INCOMPLETE }, //NOTE: The bounds for all but the first value are differene
		{	"3.0 N",    	0.0,   	 	    0.7,	5.5,	INCOMPLETE }, //TODO: these values are very open, narrow bounds when tips are better charaterized.
		{  	"9.0 N",      	0.0,     		1.5,	5.5,	INCOMPLETE }, 
		{ 	"18.0 N", 		0.0,        	3.3,	5.5,	INCOMPLETE }, 
		//TODO:- Change NUM_OF_PRESSURE_CONDITION in calib.h
};

CalibTable ROS1_Calib_Table[] = {
		
		//  condition       min voltage,    max voltage	
		{ 	"COVERED",      	0.19,  			0.25,	5.5,	INCOMPLETE }, 
		{ 	"UNCOVERED", 		3.00,  			3.30,	5.5,	INCOMPLETE }, 
		//TODO:- Change NUM_OF_ROS_CONDITION in calib.h
};

CalibTable ROS2_Calib_Table[] = {
		
		//  condition       min voltage,    max voltage	
		{ 	"COVERED",      	0.19,  			0.25,	5.5,	INCOMPLETE }, 
		{ 	"UNCOVERED", 		3.00,  			3.30,	5.5,	INCOMPLETE }, 
		//TODO:- Change NUM_OF_ROS_CONDITION in calib.h
};

CalibTable Accelerometer_Calib_Table[] = {
		
		//  condition   min voltage,    max voltage	
		{ 	"0°",      	-1.0,  			1.0,	180.0,	INCOMPLETE }, 
		{ 	"30°", 		29.0,  			31.0,	180.0,	INCOMPLETE }, 
		{ 	"60°",      59.0,  			61.0,	180.0,	INCOMPLETE }, 
		{ 	"90°", 		89.0,  			91.0,	180.0,	INCOMPLETE }, 

//#define NUM_OF_ACC_CONDITION	(4)

};

CalibTable Magnetometer_Calib_Table[] = {
		
		//  condition       min voltage,    max voltage	
		{ 	"0°N",      	354.0,  		6.0,	500.0,	INCOMPLETE }, 
		{ 	"90°E", 		84.0,  		96.0,	500.0,	INCOMPLETE }, 
		{ 	"180°S",      	174.0,  	186.0,	500.0,	INCOMPLETE }, 
		{ 	"270°W", 		264.0,  	276.0,	500.0,	INCOMPLETE }, 

//#define NUM_OF_MAG_CONDITION	(4)

};


void transform_raw_acc(void)
{
	double ax,ay,az;
	read_accelerometer_data();
	
	printf("Ax=%f, Ay=%f, Az=%f", Ax, Ay, Az);
	ax = (double) Ax;
	ay = (double) Ay;
	az = (double) Az;

	Gx = (float) ((ACC_Data.data.ACC00*ax)+(ACC_Data.data.ACC10*ay)+(ACC_Data.data.ACC20*az)+ACC_Data.data.ACC30);
	Gy = (float) ((ACC_Data.data.ACC01*ax)+(ACC_Data.data.ACC11*ay)+(ACC_Data.data.ACC21*az)+ACC_Data.data.ACC31);
	Gz = (float) ((ACC_Data.data.ACC02*ax)+(ACC_Data.data.ACC12*ay)+(ACC_Data.data.ACC22*az)+ACC_Data.data.ACC32);
	
	
	
}
void acc_transform_wrapper(void)
{
	printf("acc_tranform_wrapper \n");
	//generate w matrix from saved accel values
	struct matrix_struct w;
	w.m = 6;
	w.n = 4;
	for(int i = 0; i < w.m; i++)
	{
		for(int j = 0; j < w.n; j++)
		{
			w.mat[i][j] = (float)Acc_Calib1.Acc_Calib_Write_Buf[i][j];
		}
	}
	//generate known Y matrix
	struct matrix_struct Y;
	Y.m = 6;
	Y.n = 3;
	Y.mat[0][0] = 0;
	Y.mat[0][1] = 0;
	Y.mat[0][2] = 1;
	Y.mat[1][0] = 0;
	Y.mat[1][1] = 0;
	Y.mat[1][2] = -1;
	Y.mat[2][0] = 0;
	Y.mat[2][1] = 1;
	Y.mat[2][2] = 0;
	Y.mat[3][0] = 0;
	Y.mat[3][1] = -1;
	Y.mat[3][2] = 0;
	Y.mat[4][0] = 1;
	Y.mat[4][1] = 0;
	Y.mat[4][2] = 0;
	Y.mat[5][0] = -1;
	Y.mat[5][1] = 0;
	Y.mat[5][2] = 0;
	
	struct matrix_struct acc_transform_matrix;
	calculate_acc_transform(&w, &Y, &acc_transform_matrix);
	
	//populate the ACC data structure //TODO this structure is a bit bulky
	ACC_Data.data.ACC00 = acc_transform_matrix.mat[0][0];
	ACC_Data.data.ACC01 = acc_transform_matrix.mat[0][1];
	ACC_Data.data.ACC02 = acc_transform_matrix.mat[0][2];
	ACC_Data.data.ACC10 = acc_transform_matrix.mat[1][0];
	ACC_Data.data.ACC11 = acc_transform_matrix.mat[1][1];
	ACC_Data.data.ACC12 = acc_transform_matrix.mat[1][2];
	ACC_Data.data.ACC20 = acc_transform_matrix.mat[2][0];
	ACC_Data.data.ACC21 = acc_transform_matrix.mat[2][1];
	ACC_Data.data.ACC22 = acc_transform_matrix.mat[2][2];
	ACC_Data.data.ACC30 = acc_transform_matrix.mat[3][0];
	ACC_Data.data.ACC31 = acc_transform_matrix.mat[3][1];
	ACC_Data.data.ACC32 = acc_transform_matrix.mat[3][2];
	
	Write_Acc_Calib_Dat(); //write ACC values to flash
	printf("accelration calibration complete \n");
	
}

/*
 * This function uses least squares to solve Y = w*X
 * X = [w^T*w]^-1 *w^T*Y
 */
void calculate_acc_transform(struct matrix_struct * w, struct matrix_struct * Y, struct matrix_struct * X)
{
	X->m = 4;
	X->n = 3;
	printf("w: \n");
	for(int i = 0; i < w->m; i++)
	{
		for(int j = 0; j < w->n; j++)
		{
			printf("%f ", w->mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	struct matrix_struct wt;
	transpose(w, &wt);	
	printf("wt: \n");
	for(int i = 0; i < wt.m; i++)
	{
		for(int j = 0; j < wt.n; j++)
		{
			printf("%f ", wt.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	struct matrix_struct wtw;
	m_mult(&wt, w, &wtw);
	printf("w^T * w: \n");
	for(int i = 0; i < wtw.m; i++)
	{
		for(int j = 0; j < wtw.n; j++)
		{
			printf("%f ", wtw.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	struct matrix_struct inv_wtw;
	inverse(&wtw, &inv_wtw);
	printf("inv(w^T * w): \n");
	for(int i = 0; i < inv_wtw.m; i++)
	{
		for(int j = 0; j < inv_wtw.n; j++)
		{
			printf("%20.12f ", inv_wtw.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	struct matrix_struct inv_wtw_wt;
	m_mult(&inv_wtw, &wt, &inv_wtw_wt);
	printf("inv(w' * w)*w': \n");
	for(int i = 0; i < inv_wtw_wt.m; i++)
	{
		for(int j = 0; j < inv_wtw_wt.n; j++)
		{
			printf("%20.12f ", inv_wtw_wt.mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	m_mult(&inv_wtw_wt, Y, X);
	
	printf("X matrix (ACC constants): \n");
	for(int i = 0; i < X->m; i++)
	{
		for(int j = 0; j < X->n; j++)
		{
			printf("%20.12f ", X->mat[i][j]);
		}
		printf("\n");
	}
	
}

/*-----------------------------------------------------------------------------
 *  Function:     calibrate_IRDMS
 *  Brief:        calibrate_IRDMS. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
uint_8 calibrate_IRDMS(void)
{
	
	uint_32 IRDMS_result = 0;
	uint_32 sample_count = 0;
	ADC_RESULT_STRUCT adc_out;
	float IRDMS_voltage = 0.0;
	int i = 0;
	
	ui_timer_start(500);


	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		if (read(ir_sens, &adc_out,sizeof(adc_out)))
		{
			IRDMS_result = IRDMS_result + adc_out.result;
			sample_count++;
		}
	}
	IRDMS_result  = (IRDMS_result /sample_count);
	IRDMS_voltage = (IRDMS_result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
		
	if((IRDMS_voltage >= IRDMS_Calib_Table[IRDMS_Condition_selection].min_voltage) && (IRDMS_voltage <= IRDMS_Calib_Table[IRDMS_Condition_selection].max_voltage))
	{
		IRDMS_Calib_Table[IRDMS_Condition_selection].curr_voltage = IRDMS_voltage;
		IRDMS_Calib_Table[IRDMS_Condition_selection].Calib_status = COMPLETED;
		display_IRDMS_Calibration();
		Time_Delay_Sleep(2500);
		IRDMS_Condition_selection++;
		IRDMS_Condition_selection = IRDMS_Condition_selection % NUM_OF_IRDMS_CONDITION;	
		
		for(i =0; i < NUM_OF_IRDMS_CONDITION; i++)
		{
			if(IRDMS_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}					
		return COMPLETED;	
		
	}
	else
	{
		IRDMS_Calib_Table[IRDMS_Condition_selection].Calib_status = INCOMPLETE;
//		IRDMS_Calib_Table[IRDMS_Condition_selection].curr_voltage = 5.5;
//		Calib_status[IRDMS] = INCOMPLETE;
		return OUT_OF_RANGE;
	}
	
}

/*-----------------------------------------------------------------------------
 *  Function:     calibrate_Pressure
 *  Brief:        calibrrates the Pressure Sensor
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
uint_8 calibrate_Pressure_Sensor(void)
{
	uint_32 Pressure_result = 0;
	uint_32 sample_count = 0;
	ADC_RESULT_STRUCT adc_out;
	float Pressure_voltage = 0.0, diff_Pressure_voltage = 0.0;

	ui_timer_start(500);

	//take an average of pressure adc values
	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		if (read(force_sens, &adc_out,sizeof(adc_out)))
		{
			Pressure_result = Pressure_result + adc_out.result;
			sample_count++;
		}
	}
	Pressure_result  = (Pressure_result /sample_count);
	Pressure_voltage = (Pressure_result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;

	if((Pressure_Calib_Table[0].curr_voltage < 5.5)) //meaning that the first calibration at 0.0N has been done
	{
		if(Pressure_Condition_selection != 0)
		{
			diff_Pressure_voltage = Pressure_voltage - Pressure_Calib_Table[0].curr_voltage;
		}
		else
		{
			diff_Pressure_voltage = Pressure_voltage;
		}
	}
	else if(Pressure_Condition_selection != 0)
	{
		Pressure_Calib_Table[Pressure_Condition_selection].Calib_status = INCOMPLETE;
		Pressure_Calib_Table[Pressure_Condition_selection].curr_voltage = 5.5;
		return OUT_OF_RANGE;  
	}
	else 
	{
		diff_Pressure_voltage = Pressure_voltage;
	}

	if((diff_Pressure_voltage >= Pressure_Calib_Table[Pressure_Condition_selection].min_voltage) && 
			(diff_Pressure_voltage <= Pressure_Calib_Table[Pressure_Condition_selection].max_voltage))
	{
		Pressure_Calib_Table[Pressure_Condition_selection].curr_voltage = Pressure_voltage;
		Pressure_Calib_Table[Pressure_Condition_selection].Calib_status = COMPLETED;
		display_Pressure_sensor_Calibration();

		Time_Delay_Sleep(2500);
		Pressure_Condition_selection++;
		Pressure_Condition_selection = Pressure_Condition_selection % NUM_OF_PRESSURE_CONDITION; 

		for(int i =0; i < NUM_OF_PRESSURE_CONDITION; i++)
		{
			if(Pressure_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}

		return COMPLETED; 
	}
	else
	{
		Pressure_Calib_Table[Pressure_Condition_selection].Calib_status = INCOMPLETE;
		//  Pressure_Calib_Table[Pressure_Condition_selection].curr_voltage = 5.5;
		return OUT_OF_RANGE;
	}
}


/*-----------------------------------------------------------------------------
 *  Function:     calibrate_IRDMS
 *  Brief:        calibrate_IRDMS. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
uint_8 calibrate_ROS_Sensors(void)
{	
	uint_32 ROS1_result = 0;
	float 	ROS1_voltage = 0.0;
	uint_32 ROS2_result = 0;
	float 	ROS2_voltage = 0.0;
	uint_32 sample_count1 = 0;
	uint_32 sample_count2 = 0;
	ADC_RESULT_STRUCT adc_out;
	boolean ros1_condition;
	boolean ros2_condition;

	
	//collect ROS samples and calculate voltage
	ui_timer_start(500);
	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		if (read(ros_sens1, &adc_out,sizeof(adc_out)))
		{
			ROS1_result = ROS1_result + adc_out.result;
			sample_count1++;
		}
		if (read(ros_sens2, &adc_out,sizeof(adc_out)))
		{
			ROS2_result = ROS2_result + adc_out.result;
			sample_count2++;
		}
	}
	ROS1_result  = (ROS1_result /sample_count1);
	ROS1_voltage = (ROS1_result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;
	ROS2_result  = (ROS2_result /sample_count2);
	ROS2_voltage = (ROS2_result) * RAW_DATA_TO_VOLTAGE_MULTIPLIER;

	//TODO: this function could be made smarter by seperating failing of ROS1 and ROS2 independently
    ros1_condition = (ROS1_voltage >= ROS1_Calib_Table[ROS_Condition_selection].min_voltage) && 
			(ROS1_voltage <= ROS1_Calib_Table[ROS_Condition_selection].max_voltage);
	ros2_condition = (ROS2_voltage >= ROS2_Calib_Table[ROS_Condition_selection].min_voltage) && 
			(ROS2_voltage <= ROS2_Calib_Table[ROS_Condition_selection].max_voltage);
	printf("ROS condition: %d \n", ROS_Condition_selection);
	printf("max: %f, min: %f \n", ROS1_Calib_Table[ROS_Condition_selection].max_voltage, ROS1_Calib_Table[ROS_Condition_selection].min_voltage);
	printf("ros1 cond: %d, ros2 cond: %d \n", ros1_condition, ros2_condition);
	if(ros1_condition && ros2_condition)
	{
		ROS1_Calib_Table[ROS_Condition_selection].curr_voltage = ROS1_voltage;
		ROS1_Calib_Table[ROS_Condition_selection].Calib_status = COMPLETED;
		ROS2_Calib_Table[ROS_Condition_selection].curr_voltage = ROS2_voltage;
		ROS2_Calib_Table[ROS_Condition_selection].Calib_status = COMPLETED;
		display_ROS_Calibration();
		
		Time_Delay_Sleep(2500);
		ROS_Condition_selection++;
		ROS_Condition_selection = ROS_Condition_selection % NUM_OF_ROS_CONDITION;	
		
		for(int i =0; i < NUM_OF_ROS_CONDITION; i++)
		{
			if(ROS1_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
			if(ROS2_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}
					
		return COMPLETED;	
	}
	else
	{
		ROS1_Calib_Table[ROS_Condition_selection].Calib_status = INCOMPLETE;
		ROS2_Calib_Table[ROS_Condition_selection].Calib_status = INCOMPLETE;
//		ROS_Calib_Table[ROS_Condition_selection].curr_voltage = 5.5;
		return OUT_OF_RANGE;
	}
	
}


uint_8 calibrate_Accelerometer2(void)
{	
	float pitch, roll;
	float slope;
	float angle_sum = 0.0;
	int sample_count = 0.0;
	ui_timer_start(500);
	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		get_euler_angles(&roll, &pitch);
		angle_sum += pitch;
		sample_count++;			
	}
	
	slope = angle_sum/sample_count;
	printf("sum= %f, count=%d, avg=&f", angle_sum, sample_count, slope);
	if((slope >= Accelerometer_Calib_Table[Accelerometer_Condition_selection].min_voltage) && 
			(slope <= Accelerometer_Calib_Table[Accelerometer_Condition_selection].max_voltage))
	{
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage = slope;
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_status = COMPLETED;
		Acc_reading_status = 1;
		display_Accelerometer_Calibration();
		
		Time_Delay_Sleep(2500);
		Accelerometer_Condition_selection++;
		Accelerometer_Condition_selection = Accelerometer_Condition_selection % NUM_OF_ACC_CONDITION;	
		
		for(int i =0; i < NUM_OF_ACC_CONDITION; i++)
		{
			if(Accelerometer_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}
					
		return COMPLETED;	
	}
	else
	{
		printf("\nSlope %f\n", slope);
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_status = INCOMPLETE;
//		Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage = 180.0;
		return OUT_OF_RANGE;
	}
}
/*-----------------------------------------------------------------------------
 *  Function:     calibrate_IRDMS
 *  Brief:        calibrate_IRDMS. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
uint_8 calibrate_Accelerometer(void)
{	
	char tempString[16];
	char aspect[4];
	uint_16 magnetic_heading = 0xffff;
	int_16  tempAngle = 0xffff;	
	uint_16 sample_count = 0;
	int_16 	Final_Angle = 0;
	
	memset(tempString,0x00,16);
	
	ui_timer_start(500);
	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		if(get_slope_measurement(&tempAngle, aspect, &magnetic_heading) == 0)
		{
			Final_Angle = Final_Angle + tempAngle;
			sample_count++;			
		}
	}
	
	Final_Angle = (((float)(Final_Angle/sample_count)) + 0.5);

	if((Final_Angle >= Accelerometer_Calib_Table[Accelerometer_Condition_selection].min_voltage) && 
			(Final_Angle <= Accelerometer_Calib_Table[Accelerometer_Condition_selection].max_voltage))
	{
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage = Final_Angle;
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_status = COMPLETED;
		Acc_reading_status = 1;
		display_Accelerometer_Calibration();
		
		Time_Delay_Sleep(2500);
		Accelerometer_Condition_selection++;
		Accelerometer_Condition_selection = Accelerometer_Condition_selection % NUM_OF_ACC_CONDITION;	
		
		for(int i =0; i < NUM_OF_ACC_CONDITION; i++)
		{
			if(Accelerometer_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}
					
		return COMPLETED;	
	}
	else
	{
		printf("\nFinal Angle %d\n", Final_Angle);
		Accelerometer_Calib_Table[Accelerometer_Condition_selection].Calib_status = INCOMPLETE;
//		Accelerometer_Calib_Table[Accelerometer_Condition_selection].curr_voltage = 180.0;
		return OUT_OF_RANGE;
	}
	
}

/*-----------------------------------------------------------------------------* 
 * Function:    comp
 * Brief:       This function return the difference between 2 numbers and used as a supporting function for Q sort  
 * Parameter:   two numbers
 * Return:      difference  between two numbers
 -----------------------------------------------------------------------------*/
int comp(const void * a, const void * b)
{
	return (*(uint16_t*) b - *(uint16_t*) a);
}

/*-----------------------------------------------------------------------------
 *  Function:     calibrate_Magnetometer
 *  Brief:        calibrate_Magnetometer. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
uint_8 calibrate_Magnetometer(void)
{	
	char 	tempString[16];
	char 	aspect[4];
	uint_16 magnetic_heading = 0xffff;
	int_16  tempAngle = 0xffff;	
	uint_16 sample_count = 0;
	uint_32 Final_magnetic_heading = 0;
	uint_16 mag_heading_buff[100];
	
	memset(tempString,0x00,16);
	memset(mag_heading_buff,0x00,100);
	
	ui_timer_start(500);
	while(Check_UI_Timer_Timeout() != TIME_OUT)
	{
		if(get_slope_measurement(&tempAngle, aspect, &magnetic_heading) == 0)
		{
			mag_heading_buff[sample_count++] = magnetic_heading;
//			Final_magnetic_heading = Final_magnetic_heading + magnetic_heading;
//			sample_count++;			
		}
	}
	
	qsort(mag_heading_buff, sample_count, sizeof(uint_16), comp);
	Final_magnetic_heading = mag_heading_buff[(sample_count / 2)];
		
	if(((Final_magnetic_heading >= Magnetometer_Calib_Table[Magnetometer_Condition_selection].min_voltage) && 
			(Final_magnetic_heading <= Magnetometer_Calib_Table[Magnetometer_Condition_selection].max_voltage))
			|| 
			((Magnetometer_Condition_selection == 0) && 
					(((Final_magnetic_heading >= Magnetometer_Calib_Table[Magnetometer_Condition_selection].min_voltage) && 
							(Final_magnetic_heading <= 359)) || 
							((Final_magnetic_heading <= Magnetometer_Calib_Table[Magnetometer_Condition_selection].max_voltage) && 
									(Final_magnetic_heading >= 0)))
				))
	{
		Magnetometer_Calib_Table[Magnetometer_Condition_selection].curr_voltage = Final_magnetic_heading;
		Magnetometer_Calib_Table[Magnetometer_Condition_selection].Calib_status = COMPLETED;
//		Acc_reading_status = 1;
		display_Magnetometer_Calibration();
		
		Time_Delay_Sleep(2500);
		Magnetometer_Condition_selection++;
		Magnetometer_Condition_selection = Magnetometer_Condition_selection % NUM_OF_MAG_CONDITION;	
		
		for(int i =0; i < NUM_OF_MAG_CONDITION; i++)
		{
			if(Magnetometer_Calib_Table[i].Calib_status != COMPLETED)
			{
				return INCOMPLETE;
			}
		}
					
		return COMPLETED;	
	}
	else
	{
		printf("\nFinal Angle %d\n", Final_magnetic_heading);
		Magnetometer_Calib_Table[Magnetometer_Condition_selection].Calib_status = INCOMPLETE;
//		Magnetometer_Calib_Table[Magnetometer_Condition_selection].curr_voltage = 500.0;
		return OUT_OF_RANGE;
	}
	
}
/*-----------------------------------------------------------------------------
 *  Function:     print_pressure_calib_data
 *  Brief:        print_pressure_calib_data. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void print_calib_data(CalibTable *Sensor_Calib_Table,uint_8 No_of_calib_condition)
{
	
	printf("Max\t\tMin\n");
	for(int i =0; i< No_of_calib_condition; i++)
	{
		printf("%f\t%f\n",Sensor_Calib_Table[i].min_voltage,Sensor_Calib_Table[i].max_voltage);
		
	}

}


/*-----------------------------------------------------------------------------
 *  Function:     Read_IRDMS_calib_data
 *  Brief:        Read_IRDMS_calib_data. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Read_calib_data(const char file_name[32],CalibTable *Sensor_Calib_Table)
{
	MQX_FILE_PTR calib_fd_ptr; 
	char temp_buff[50] = {0};
    uint_8 i = 0;
    
	/* Opening settings file from SD card*/
	calib_fd_ptr = fopen(file_name, "r");
	if (calib_fd_ptr == NULL)
	{
		printf("Calibration Data file %s not found\n",file_name);
		return ;	
	}
	else
	{	
		while(fgets(temp_buff,sizeof(temp_buff),calib_fd_ptr))
		{
			if(strlen(temp_buff) > 1)		
			{
//				printf("%s\n",temp_buff);
				sscanf(temp_buff,"%f,%f",&Sensor_Calib_Table[i].min_voltage,&Sensor_Calib_Table[i].max_voltage);	
				i++;
				memset(temp_buff,0x0,sizeof(temp_buff));
			}
		}
		fclose(calib_fd_ptr);
		printf("\nRead %s calib. data success\n",file_name);
		return ;	
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     print_pressure_calib_data
 *  Brief:        print_pressure_calib_data. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void print_pressure_calib_data(void)
{
	printf("\n Pressure Calib Data\n");
	printf("Max\tMin\n");
	for(int i =0; i< NUM_OF_PRESSURE_CONDITION; i++)
	{
		printf("%f\t%f\n",Pressure_Calib_Table[i].min_voltage,Pressure_Calib_Table[i].max_voltage);
		
	}

}



/*-----------------------------------------------------------------------------
 *  Function:     calibrate_IRDMS
 *  Brief:        calibrate_IRDMS. 
 *  Parameter:    None
 *  Return:       None
-----------------------------------------------------------------------------*/
void Write_IRDMS_Data(void)
{
	FLOAT_TO_CHAR conv;
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];
	char Data_Buff[30];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_IRD_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		for(int i =0; i < NUM_OF_IRDMS_CONDITION; i++)
		{	
			conv.var_float	= IRDMS_Calib_Table[i].curr_voltage;
			memcpy(&Data_Buff[i*4],conv.var_char_float,4);
//			Write_Calib(conv.var_char_float,IRDMS_ADDR_LOC,4);
			fprintf(Ser_fd_ptr, "%s = ",IRDMS_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",IRDMS_Calib_Table[i].curr_voltage);
		}
		memcpy(&Calib_Flash_Buf[IRDMS*20],Data_Buff,4*NUM_OF_IRDMS_CONDITION);
		Write_All_Calib_Dat();
//		Write_Calib(Data_Buff,IRDMS_ADDR_LOC,4*NUM_OF_IRDMS_CONDITION);
		fclose(Ser_fd_ptr);	
	}
}

void Write_Pressure_Data(void)
{
	FLOAT_TO_CHAR conv;
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];
	char Data_Buff[30];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_PRE_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		for(int i =0; i < NUM_OF_PRESSURE_CONDITION; i++)
		{	
			conv.var_float	= Pressure_Calib_Table[i].curr_voltage;
			memcpy(&Data_Buff[i*4],conv.var_char_float,4);
//			Write_Calib(conv.var_char_float,PRESS_ADDR_LOC,4);
			fprintf(Ser_fd_ptr, "%s = ",Pressure_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",Pressure_Calib_Table[i].curr_voltage);
		}
		memcpy(&Calib_Flash_Buf[PRESSURE*20],Data_Buff,4*NUM_OF_PRESSURE_CONDITION);
		Write_All_Calib_Dat();
//		Write_Calib(Data_Buff,PRESS_ADDR_LOC,4*NUM_OF_PRESSURE_CONDITION);
		fclose(Ser_fd_ptr);	
	}
}

void Write_ROS_Data(void)
{
	FLOAT_TO_CHAR ROS1_conv;
	FLOAT_TO_CHAR ROS2_conv;
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];
	char Data_Buff[30];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_ROS_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		for(int i =0; i < NUM_OF_ROS_CONDITION; i++)
		{	
			ROS1_conv.var_float	= ROS1_Calib_Table[i].curr_voltage;
			ROS2_conv.var_float	= ROS2_Calib_Table[i].curr_voltage;

			memcpy(&Data_Buff[(i*4)*2],ROS1_conv.var_char_float,4);
			memcpy(&Data_Buff[(i*4)*2 + 4],ROS2_conv.var_char_float,4);

//			Write_Calib(conv.var_char_float,ROS_ADDR_LOC,4);
			fprintf(Ser_fd_ptr, "ROS1 %s = ",ROS1_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",ROS1_Calib_Table[i].curr_voltage);
			fprintf(Ser_fd_ptr, "ROS2 %s = ",ROS2_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",ROS2_Calib_Table[i].curr_voltage);
		}
		memcpy(&Calib_Flash_Buf[ROS*20],Data_Buff,4*2*NUM_OF_ROS_CONDITION); //the * 2 is for 2 sensors

		Write_All_Calib_Dat();
//		Write_Calib(Data_Buff,ROS_ADDR_LOC,4*NUM_OF_ROS_CONDITION);
		fclose(Ser_fd_ptr);	
	}
}

void Write_GPS_Data(uint_32  sec)
{
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_GPS_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		fprintf(Ser_fd_ptr, "GPS lock time = %d sec\n",sec);
		fclose(Ser_fd_ptr);	
	}
}

void Write_ACC_Data(void)
{
	FLOAT_TO_CHAR conv;
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];
	char Data_Buff[30];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_ACC_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		for(int i =0; i < NUM_OF_ACC_CONDITION; i++)
		{	
			conv.var_float	= Accelerometer_Calib_Table[i].curr_voltage;
			memcpy(&Data_Buff[i*4],conv.var_char_float,4);
//			Write_Calib(conv.var_char_float,ROS_ADDR_LOC,4);
			fprintf(Ser_fd_ptr, "%s = ",Accelerometer_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",Accelerometer_Calib_Table[i].curr_voltage);
		}
		memcpy(&Calib_Flash_Buf[ACCELEROMETER*20],Data_Buff,4*NUM_OF_ACC_CONDITION);
		Write_All_Calib_Dat();
//		Write_Calib(Data_Buff,ACC_ADDR_LOC,4*NUM_OF_ACC_CONDITION);
		fclose(Ser_fd_ptr);	
	}
}

void Write_MAG_Data(void)
{
	FLOAT_TO_CHAR conv;
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[50];
	char Data_Buff[30];

	memset(Ser_File_Name,0x00,50);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_MAG_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{
		for(int i =0; i < NUM_OF_MAG_CONDITION; i++)
		{	
			conv.var_float	= Magnetometer_Calib_Table[i].curr_voltage;
			memcpy(&Data_Buff[i*4],conv.var_char_float,4);
//			Write_Calib(conv.var_char_float,ROS_ADDR_LOC,4);
			fprintf(Ser_fd_ptr, "%s = ",Magnetometer_Calib_Table[i].Calib_condition);
			fprintf(Ser_fd_ptr, "%f\n",Magnetometer_Calib_Table[i].curr_voltage);
		}
		memcpy(&Calib_Flash_Buf[MAGNETOMETER*20],Data_Buff,4*NUM_OF_MAG_CONDITION);
		Write_All_Calib_Dat();
//		Write_Calib(Data_Buff,MAG_ADDR_LOC,4*NUM_OF_MAG_CONDITION);
		fclose(Ser_fd_ptr);	
	}
}

void Update_Calib_Buff()
{
	FLOAT_TO_CHAR conv;
	char Data_Buff[30];
	
	Read_All_Calib_Dat();
	
	// Reading IRDMS data from flash
//	Read_Calib(Data_Buff,IRDMS_ADDR_LOC,4*NUM_OF_IRDMS_CONDITION);
	
	if((Calib_Flash_Buf[IRDMS*20]==0xFF)&&(Calib_Flash_Buf[(IRDMS*20)+1]==0xFF)&&
	   (Calib_Flash_Buf[(IRDMS*20)+2]==0xFF)&&(Calib_Flash_Buf[(IRDMS*20)+3]==0xFF))
	{
		// no data in flash
	}
	else
	{
		// setting status
		memcpy(Data_Buff,&Calib_Flash_Buf[IRDMS*20],4*NUM_OF_IRDMS_CONDITION);
		Calib_status[IRDMS_CALIB] = 1;
		for(int i =0; i < NUM_OF_IRDMS_CONDITION; i++)
		{	
			memcpy(conv.var_char_float,&Data_Buff[i*4],4);
			IRDMS_Calib_Table[i].curr_voltage = conv.var_float;
		}
	}
	
	// Reading PRESS_ADDR_LOC data from flash
//	Read_Calib(Data_Buff,PRESS_ADDR_LOC,4*NUM_OF_PRESSURE_CONDITION);
	
	if((Calib_Flash_Buf[PRESSURE*20]==0xFF)&&(Calib_Flash_Buf[(PRESSURE*20)+1]==0xFF)&&
	   (Calib_Flash_Buf[(PRESSURE*20)+2]==0xFF)&&(Calib_Flash_Buf[(PRESSURE*20)+3]==0xFF))
	{
		// no data in flash
	}
	else
	{
		// setting status
		memcpy(Data_Buff,&Calib_Flash_Buf[PRESSURE*20],4*NUM_OF_PRESSURE_CONDITION);
		Calib_status[PRESSURE_CALIB] = 1;
		for(int i =0; i < NUM_OF_PRESSURE_CONDITION; i++)
		{	
			memcpy(conv.var_char_float,&Data_Buff[i*4],4);
			Pressure_Calib_Table[i].curr_voltage = conv.var_float;
		}
	}
	
	// Reading ROS_ADDR_LOC data from flash
//	Read_Calib(Data_Buff,ROS_ADDR_LOC,4*NUM_OF_ROS_CONDITION);
	
	if((Calib_Flash_Buf[ROS*20]==0xFF)&&(Calib_Flash_Buf[(ROS*20)+1]==0xFF)&&
	   (Calib_Flash_Buf[(ROS*20)+2]==0xFF)&&(Calib_Flash_Buf[(ROS*20)+3]==0xFF))
	{
		// no data in flash
	}
	else
	{
		// setting status
		memcpy(Data_Buff,&Calib_Flash_Buf[ROS*20],4*2*NUM_OF_ROS_CONDITION); //the * 2 is for 2 sensors
		Calib_status[ROS_CALIB] = 1;
		for(int i =0; i < NUM_OF_ROS_CONDITION; i++)
		{	
			memcpy(conv.var_char_float,&Data_Buff[(i*4)*2],4);
			ROS1_Calib_Table[i].curr_voltage = conv.var_float;
			memcpy(conv.var_char_float,&Data_Buff[(i*4)*2 + 4],4);
			ROS2_Calib_Table[i].curr_voltage = conv.var_float;
		}
	}
	
	// Reading ACC_CONDITION data from flash
//	Read_Calib(Data_Buff,ACC_ADDR_LOC,4*NUM_OF_ACC_CONDITION);
	
	if((Calib_Flash_Buf[ACCELEROMETER*20]==0xFF)&&(Calib_Flash_Buf[(ACCELEROMETER*20)+1]==0xFF)&&
	   (Calib_Flash_Buf[(ACCELEROMETER*20)+2]==0xFF)&&(Calib_Flash_Buf[(ACCELEROMETER*20)+3]==0xFF))
	{
		// no data in flash
	}
	else
	{
		// setting status
		memcpy(Data_Buff,&Calib_Flash_Buf[ACCELEROMETER*20],4*NUM_OF_ACC_CONDITION);
		Calib_status[ACCELEROMETER_CALIB] = 1;
		for(int i =0; i < NUM_OF_ACC_CONDITION; i++)
		{	
			memcpy(conv.var_char_float,&Data_Buff[i*4],4);
			Accelerometer_Calib_Table[i].curr_voltage = conv.var_float;
		}
	}
	
	// Reading MAG_CONDITION data from flash
//	Read_Calib(Data_Buff,MAG_ADDR_LOC,4*NUM_OF_MAG_CONDITION);
	
	if((Calib_Flash_Buf[MAGNETOMETER*20]==0xFF)&&(Calib_Flash_Buf[(MAGNETOMETER*20)+1]==0xFF)&&
	   (Calib_Flash_Buf[(MAGNETOMETER*20)+2]==0xFF)&&(Calib_Flash_Buf[(MAGNETOMETER*20)+3]==0xFF))
	{
		// no data in flash
	}
	else
	{
		// setting status
		memcpy(Data_Buff,&Calib_Flash_Buf[MAGNETOMETER*20],4*NUM_OF_MAG_CONDITION);
		Calib_status[MAGNETOMETER_CALIB] = 1;
		for(int i =0; i < NUM_OF_MAG_CONDITION; i++)
		{	
			memcpy(conv.var_char_float,&Data_Buff[i*4],4);
			Magnetometer_Calib_Table[i].curr_voltage = conv.var_float;
		}
	}
}

