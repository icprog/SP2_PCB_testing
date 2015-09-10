/******************************************************************************
 *
 *     Filename:       $lsm303_polled.c
 *     Created on:     $Date: Mar 24, 2014    (2:27:29 PM)
 *     Revision:       $1
 *     Author:         $GadgEon
 *
 *     Description:    $This file contains the functions to configure i2c0
 *                     $and lsm303dlhc device. 
 *     Copyright (C) 2014  GadgEon System Pvt Ltd - http://www.gadgeon.com
 *
 *****************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include <math.h>
#include "lsm303.h"
#include "vector.h"
#include "config.h"
#include "Sd_Card.h"
#include "common_headers.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

static void disable_magnetometer(void);
static void disable_acceleroometer(void);
static void enable_magnetometer(void);
static void enable_acceleroometer(void);
static void LSM303D_Init(void);

static LWGPIO_STRUCT Lsm303D_ACC_pin,Lsm303D_MAG_pin;

ACC_CALIB Acc_Calib1;
LWGPIO_STRUCT Lsm303_TMP_pin;
/*Values to set the lsm303 device registers*/
uchar Lsm_commands[] = { 0x77, 0x00, 0x1C, 0x20, 0x00 };
uchar Lsm_Test_commands[] = { 0x97, 0x00, 0x1C, 0x20, 0x00 };
/*Values to enable the lsm303 device*/
uchar Lsm_enable_commands[] = { 0x77, 0x01 };
uchar Lsm_enable_Test_commands[] = { 0x97, 0x00 };
/*Values to disable the lsm303 device*/
uchar Lsm_disable_commands[] = { 0x00, 0x03 };
char Slope_Measurment_Result_Aspect[4];
int_16 Slope_Measurment_Result_Slope_Angle = 0;
double Acc_Calib_Data1[NUM_ACC_CAL_POS][4]; //double Acc_Calib_Data1[6][4];

uint8_t Acc_Calib_Read_Ctr[NUM_ACC_CAL_POS];
int_16 Mx, My, Mz;
int_16 Ax_raw, Ay_raw, Az_raw;
float Ax, Ay, Az;
float acc_cal_samples[600][3];
int acc_count = 0;

ACC_Values ACC_Data;

//double ACC11 = 0.000270374561242, ACC21 = 0.000001144176519, ACC31 = -0.000002550322889 , ACC12 = -0.000005367887200, ACC22 = -0.000264800091331, ACC32 = -0.000050200582977,ACC13 = 0.000004062177450, ACC23 = -0.000055360303431, ACC33 = 0.000272613670699,ACC10 = 0.028432111363023, ACC20 = 0.013558885443338 , ACC30 = -0.064402353662534;
//ACC_Values ACC_Data;
//uchar_ptr Lsm303_buffer;
uchar Lsm303_data_buffer[LSM_BUFFER_SIZE];
MQX_FILE_PTR Lsm303_fd;
ieee_double Pitch_radian, Pitch_degree, Roll_radian, Roll_degree, Mx_compensated, My_compensated,
Mz_compensated;
vector a_avg = { 0, 0, 0 }, m_avg = { 0, 0, 0 };//, a = { 0, 0, 0 }, m = { 0, 0, 0 };
/*To change */
uchar lsm_data_count = 0, Lsm_Data_Ready = 0;

/*Function prototypes*/
static void lsm303_i2c_write_polled(MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int, _mqx_int);
static void lsm303_i2c_read_polled(MQX_FILE_PTR, uint_32, uchar_ptr, _mqx_int, _mqx_int);
static void int_service_routine_lsm303(void *); /*gpio isr for interrupt read tmp006*/
static uint_8 init_gpio_isr_lsm303(void);
static void init_magnetometer(void);
static void init_accelerometer(void);
static void disable_magnetometer(void);
static void disable_acceleroometer(void);
static void enable_magnetometer(void);
static void enable_acceleroometer(void);
static ieee_double get_heading(void);
static void get_direction(char *, ieee_double);
static void Get_slope_measurement_Lsm_Data();
extern void Write_Calib_Acc_Dat();
extern void Calib_Acc_Avg();
extern void calibrate_compass_acc(uint8_t Dev_Pos);
extern void Acc_calib_Buff_Clear();

static int Bxmin = MAGMAXINT,  Bymin = MAGMAXINT,  Bzmin = MAGMAXINT;
static int Bxmax = -MAGMAXINT, Bymax = -MAGMAXINT, Bzmax = -MAGMAXINT;
static int Bxofs=0, Byofs=0, Bzofs=0; // offset: (max+min)/2
static int Bxrange2=MAGRANGE, Byrange2=MAGRANGE, Bzrange2=MAGRANGE; // half range: (max-min)/2

int Bx,By,Bz, final_yaw, final_roll;
float Roll,Pitch,Yaw;
float Gx, Gy, Gz;

uint8_t calib_count =100;
uint8_t Magnetometer_Calib_process=1;

void get_euler_angles(float *roll, float *pitch)
{
	//printf("Gx=%f, Gy=%f, Gz=%f", Gx, Gy, Gz);
//	Gx = Ax;
//	Gy = Ay;
//	Gz = Az;
	
	
	float sphi, cphi, Gz2;
	float phi, theta;

	phi = atan2f(Gy,Gz);
	sphi = sinf(phi);
	cphi = cosf(phi);
	Gz2 =  Gy*sphi + Gz*cphi;

	if (Gz2!=0.0)
		theta = atanf(-Gx / Gz2);
	
	*roll = atan2f(Gy, Gz) * RAD_TO_DEG;
	*pitch = atan2f(-Gx, sqrt(Gy*Gy + Gz*Gz)) * RAD_TO_DEG;
//	*roll = phi * RAD_TO_DEG;
//	*pitch = theta * RAD_TO_DEG;
	
}


/*-----------------------------------------------------------------------------
 *  Function:     update_magnetomer_calib_data
 *  Brief:        Update calibrate data and save them to flash 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void update_magnetomer_calib_data(void)
{
	uint8_t buffer[30]; 

	buffer[0] = Bxmin>>24;
	buffer[1] = Bxmin>>16;
	buffer[2] = Bxmin>>8;
	buffer[3] = Bxmin;
	
	buffer[4] = Bymin>>24;
	buffer[5] = Bymin>>16;
	buffer[6] = Bymin>>8;
	buffer[7] = Bymin;

	buffer[8] = Bzmin>>24;
	buffer[9] = Bzmin>>16;
	buffer[10] = Bzmin>>8;
	buffer[11] = Bzmin;

	buffer[12] = Bxmax>>24;
	buffer[13] = Bxmax>>16;
	buffer[14] = Bxmax>>8;
	buffer[15] = Bxmax;

	buffer[16] = Bymax>>24;
	buffer[17] = Bymax>>16;
	buffer[18] = Bymax>>8;
	buffer[19] = Bymax;

	buffer[20] = Bzmax>>24;
	buffer[21] = Bzmax>>16;
	buffer[22] = Bzmax>>8;
	buffer[23] = Bzmax;

	//Write to Flash here
}

/*-----------------------------------------------------------------------------
 *  Function:     reset_calibration_values
 *  Brief:        Reset Calibration values with default 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void reset_calibration_values(void)
{
	Bxmin = MAGMAXINT;
	Bymin = MAGMAXINT;
	Bzmin = MAGMAXINT;
	Bxmax = -MAGMAXINT;
	Bymax = -MAGMAXINT;
	Bzmax = -MAGMAXINT;
}

/*-----------------------------------------------------------------------------
 *  Function:     Lsm303_deinit
 *  Brief:        Disable lsm303dlhc device and uninstall i2c0 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
uint_8 Lsm303_deinit(void)
{
	if(Sensor_status.LSM_status_status == NOT_INITIALISED)
	{
		printf("LSM Already un-initialised..!");	
		return 1;
	}

	disable_magnetometer();
	disable_acceleroometer();

	if (MQX_OK != fclose(Lsm303_fd))
	{
		printf("\ni2c0 close Error\n");
	}

	//printf("LSM un-initialised..\r\n");	
	Sensor_status.LSM_status_status = NOT_INITIALISED;
	return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     calibrate_magnetometer
 *  Brief:        function to calculate heading from lsm303 raw data 
 *  Parameter:    none
 *  Return:       min_max_values of x y and z directions of magneto meter data
 -----------------------------------------------------------------------------*/
void calibrate_magnetometer(void)
{

	if (Lsm_Data_Ready == 1)
	{
		Lsm_Data_Ready = 0;
		Get_slope_measurement_Lsm_Data();
		//		read_magnetometer_data();

		//		m_min_x = (m_min_x<Mx)? m_min_x : Mx;// min(m_min_x, Mx);
		//		m_min_y = (m_min_y<My)? m_min_y : My; //min(m_min_y, My);
		//		m_min_z = (m_min_z<Mz)? m_min_z : Mz; //min(m_min_z, Mz);
		//
		//		m_max_x = (m_max_x>Mx)? m_max_x : Mx; //max(m_max_x, Mx);
		//		m_max_y = (m_max_y>My)? m_max_y : My; //max(m_max_y, My);
		//		m_max_z = (m_max_z>Mz)? m_max_z : Mz; //max(m_max_z, Mz);

		//printf( "min: {%+6d, %+6d, %+6d}    max: {%+6d, %+6d, %+6d}\r\n", m_min_x, m_min_x, m_min_z, m_max_x, m_max_y, m_max_z);

		Start_LSM();
	}



}

void Acc_calib_Buff_Clear()
{
	memset(Acc_Calib_Data1,0x00,sizeof(Acc_Calib_Data1));
	memset(Acc_Calib_Read_Ctr,0x00,sizeof(Acc_Calib_Read_Ctr));
}

/*-----------------------------------------------------------------------------
 *  Function:     calibrate_magnetometer
 *  Brief:        function to calculate heading from lsm303 raw data 
 *  Parameter:    none
 *  Return:       min_max_values of x y and z directions of magneto meter data
 -----------------------------------------------------------------------------*/
void calibrate_compass_acc(uint8_t Dev_Pos)
{
	if (Lsm_Data_Ready == 1)
	{
		Lsm_Data_Ready = 0;
		read_accelerometer_data();
		
		
		acc_cal_samples[acc_count][0]  =Ax;
		acc_cal_samples[acc_count][1]  =Ay;
		acc_cal_samples[acc_count][2]  =Az;
		acc_count++;
		
		//printf("--- Ax=%f, Ay=%f, Az=%f \n", (double)Ax, (double)Ay, (double)Az);
		Acc_Calib_Data1[Dev_Pos][0] += (double)Ax;//add raw acceleromter values. 
		Acc_Calib_Data1[Dev_Pos][1] += (double)Ay;
		Acc_Calib_Data1[Dev_Pos][2] += (double)Az;
		Acc_Calib_Read_Ctr[Dev_Pos] += 1;
		Start_LSM();
	}
}


void Calib_Acc_Avg()
{
	uint8_t Dev_Pos=0;
	for(Dev_Pos=0;Dev_Pos<NUM_ACC_CAL_POS;Dev_Pos++)
	{
		Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][0] = 
				(Acc_Calib_Data1[Dev_Pos][0]/Acc_Calib_Read_Ctr[Dev_Pos]);

		Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][1] = 
				(Acc_Calib_Data1[Dev_Pos][1]/Acc_Calib_Read_Ctr[Dev_Pos]);

		Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][2] = 
				(Acc_Calib_Data1[Dev_Pos][2]/Acc_Calib_Read_Ctr[Dev_Pos]);

		Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][3] = 1;
	}
}

/*
 * this wirtes the w matrix
 */
void Write_Calib_Acc_Dat()
{
	MQX_FILE_PTR Ser_fd_ptr; 
	char Ser_File_Name[60];

	memset(Ser_File_Name,0x00,60);
	strcpy(Ser_File_Name,"a:");	
	strcat(Ser_File_Name,Serial_Numbr);
	strcat(Ser_File_Name,"_Compass_Acc_Calib.txt");

	Ser_fd_ptr = fopen(Ser_File_Name, "w");
	if (Ser_fd_ptr != NULL)
	{

		uint8_t Dev_Pos=0;
		for(Dev_Pos=0;Dev_Pos<NUM_ACC_CAL_POS;Dev_Pos++)
		{
			fprintf(Ser_fd_ptr, "%d\t",Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][0]);
			fprintf(Ser_fd_ptr, "%d\t",Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][1]);
			fprintf(Ser_fd_ptr, "%d\t",Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][2]);
			fprintf(Ser_fd_ptr, "%d\r\n",Acc_Calib1.Acc_Calib_Write_Buf[Dev_Pos][3]);
		}
		fclose(Ser_fd_ptr);
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     collect_accelerometer_data
 *  Brief:        Collect accelerometer calibration data
 *  Parameter:    Position
 *  Return:       None
 -----------------------------------------------------------------------------*/
void collect_accelerometer_data(uint8_t position)
{
	ui_timer_start(ACC_REFRESH_TIME);
	while (Check_UI_Timer_Timeout() != TIME_OUT) 
	{
		calibrate_compass_acc(position);
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     lsm303_i2c0_init
 *  Brief:        Initialise i2c0, accelerometer magnetometer and GPIO ISR
 *  Parameter:    None
 *  Return:       Status,0=Success,1=Failure
 -----------------------------------------------------------------------------*/
uint_8 lsm303_i2c0_init(void)
{

	if(Sensor_status.LSM_status_status == INITIALISED)
	{
		printf("LSM Already initialized..!");	
		return 1;
	}
	_mqx_int param;
	//    Lsm303_buffer = Lsm303_data_buffer;

	/* Open the I2C driver */
	Lsm303_fd = fopen(I2C_DEVICE_POLLED, NULL);
	if (Lsm303_fd == NULL)
	{
		printf("ERROR Lsm303_fd == NULL!\n");
		return 1;
	}
	param = 100000;
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_BAUD, &param);
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);

#if ENABLE_LSM303DLHC	
	init_accelerometer();
	init_magnetometer();
#else if ENABLE_LSM303D
	LSM303D_Init();
#endif

	disable_magnetometer();
	disable_acceleroometer();

	init_gpio_isr_lsm303();

	printf("LSM  initialisation Success..!");
	Sensor_status.LSM_status_status = INITIALISED;
	return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     LSM303D_Init
 *  Brief:        Initialise LSM303D accelerometer magnetometer.
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
#if ENABLE_LSM303D
static void LSM303D_Init(void)
{
	uint8_t CTRL_Values[8] = {0x00,0x87,0x00,0x00,0x04,0x64,0x00,0x02};
	uint8_t Dev_ID[5] = {0};
	Dev_ID[0] = 0x09;
			
	lsm303_i2c_write_polled(Lsm303_fd, CTRL0, (uchar_ptr) &CTRL_Values[0],1, 0);	
	lsm303_i2c_write_polled(Lsm303_fd, CTRL1, (uchar_ptr) &CTRL_Values[1],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL2, (uchar_ptr) &CTRL_Values[2],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL3, (uchar_ptr) &CTRL_Values[3],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL4, (uchar_ptr) &CTRL_Values[4],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL5, (uchar_ptr) &CTRL_Values[5],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL6, (uchar_ptr) &CTRL_Values[6],1, 0);
//	lsm303_i2c_read_polled(Lsm303_fd, CTRL7, (uchar_ptr) &Dev_ID[0],1, 0);
	lsm303_i2c_write_polled(Lsm303_fd, CTRL7, (uchar_ptr) &CTRL_Values[7],1, 0);
}
#endif

/*-----------------------------------------------------------------------------
 *  Function:     lsm303_i2c0_init
 *  Brief:        Initialise i2c0, accelerometer magnetometer and GPIO ISR
 *  Parameter:    None
 *  Return:       Status,0=Success,1=Failure
 -----------------------------------------------------------------------------*/
uint_8 Lsm303_Test_Init(void)
{

	if(Sensor_status.LSM_status_status == INITIALISED)
	{
		printf("LSM Already initialized..!");	
		return 1;
	}
	_mqx_int param;
	//    Lsm303_buffer = Lsm303_data_buffer;

	init_gpio_isr_lsm303();

	/* Open the I2C driver */
	Lsm303_fd = fopen(I2C_DEVICE_POLLED, NULL);
	if (Lsm303_fd == NULL)
	{
		printf("ERROR Lsm303_fd == NULL!\n");
		return 1;
	}
	param = 100000;
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_BAUD, &param);
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);

	//    init_accelerometer();
	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1, (uchar_ptr) &Lsm_Test_commands[0], 1, 0);

	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG4, (uchar_ptr) &Lsm_Test_commands[1], 1, 0);

	//    init_magnetometer();
	lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG1, (uchar_ptr) &Lsm_Test_commands[2], 3, 1);

	//    disable_magnetometer();
	lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG3, (uchar_ptr) &Lsm_enable_Test_commands[1], 1, 1);

	//    disable_acceleroometer();
	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1,(uchar_ptr) &Lsm_enable_Test_commands[0], 1, 0);

	printf("LSM  initialisation Success..!");
	Sensor_status.LSM_status_status = INITIALISED;
	return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     int_service_routine_lsm303
 *  Brief:        Lsm303 data conversion complete interrupt
 *  Parameter:    ISR parameter
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void int_service_routine_lsm303(void *pin)
{
	if(	(State_of_Screen == UI_NEW_SNOW_PROFILE_CALIBRATING)
		|| (State_of_Screen == UI_NEW_SNOW_PROFILE_COLLECTING))
	{
#if ENABLE_LSM303DLHC
		lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
		_lwevent_set(&Lsm_Event, SW_EVENT_MASK);
#else if ENABLE_LSM303D		
		static uint8_t LSM_Sample_count = 0;
		if (lwgpio_int_get_flag(&Lsm303D_MAG_pin) == TRUE )
		{
			lwgpio_int_clear_flag(&Lsm303D_MAG_pin);

		}
		if(lwgpio_int_get_flag(&Lsm303D_ACC_pin) == TRUE )
		{
			lwgpio_int_clear_flag(&Lsm303D_ACC_pin);
		}
//		if(LSM_Sample_count++> 100)
		{
			lwgpio_int_enable(&Lsm303D_MAG_pin, FALSE );
			lwgpio_int_enable(&Lsm303D_ACC_pin, FALSE );
			LSM_Sample_count = 0;
			_lwevent_set(&Lsm_Event, SW_EVENT_MASK);
		}
//lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);

#endif
	}
	else
	{
#if ENABLE_LSM303DLHC
		lwgpio_int_enable(&Lsm303_int_pin, FALSE );
		lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
#else if ENABLE_LSM303D

		if (lwgpio_int_get_flag(&Lsm303D_MAG_pin) == TRUE )
		{
			lwgpio_int_clear_flag(&Lsm303D_MAG_pin);
			lwgpio_int_enable(&Lsm303D_MAG_pin, FALSE );
		}

		if(lwgpio_int_get_flag(&Lsm303D_ACC_pin) == TRUE )
		{
			lwgpio_int_clear_flag(&Lsm303D_ACC_pin);
			lwgpio_int_enable(&Lsm303D_ACC_pin, FALSE );		
		}
#endif	
		Lsm_Data_Ready = 1;
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     init_gpio_isr_lsm303
 *  Brief:        Lsm303 data conversion complete interrupt pin initialisation
 *  Parameter:    None
 *  Return:       Status 0=Success,1=Failure
 -----------------------------------------------------------------------------*/
static uint_8 init_gpio_isr_lsm303(void)

{

#if ENABLE_LSM303DLHC
	/*
     Opening the pin for input, initialize interrupt on it and set interrupt handler*/
	/* opening pins for input */
	if (!lwgpio_init(&Lsm303_int_pin, BSP_LSM_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		return 1;
		printf("LSM GPIO init as i/p  Fail\n");
	}
	/* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&Lsm303_int_pin, LWGPIO_MUX_B9_GPIO);
	lwgpio_set_attribute(&Lsm303_int_pin, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(&Lsm303_int_pin, LWGPIO_INT_MODE_FALLING))
	{
		printf("LSM GPIO init Fail\n");
		return 1;
	}
	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(&Lsm303_int_pin), int_service_routine_lsm303,
			(void *) &Lsm303_int_pin);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(&Lsm303_int_pin), 3, 0, TRUE);
	/* Disable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Lsm303_int_pin, FALSE );
#else if ENABLE_LSM303D

	// LSM interrupts for ACC	
#if 1 
	if (!lwgpio_init(&Lsm303D_ACC_pin, LSM303D_ACC_PIN_INT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("LSM GPIO init as i/p  Fail\n");
		return 1;		
	}

	lwgpio_set_functionality(&Lsm303D_ACC_pin, LWGPIO_MUX_D10_GPIO);
	lwgpio_set_attribute(&Lsm303D_ACC_pin, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);

	if (!lwgpio_int_init(&Lsm303D_ACC_pin, LWGPIO_INT_MODE_RISING))
	{
		printf("LSM GPIO init Fail\n");
		return 1;
	}
	_int_install_isr(lwgpio_int_get_vector(&Lsm303D_ACC_pin), int_service_routine_lsm303,(void *) &Lsm303D_ACC_pin);
	_bsp_int_init(lwgpio_int_get_vector(&Lsm303D_ACC_pin), 3, 0, TRUE);
	lwgpio_int_enable(&Lsm303D_ACC_pin, FALSE );
#endif

	// LSM interrupts for MAG	
#if 1 
	/* Opening the pin for input, initialize interrupt on it and set interrupt handler*/
	/* opening pins for input */
	
	if (!lwgpio_init(&Lsm303D_MAG_pin, LSM303D_MAG_PIN_INT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("LSM GPIO init as i/p  Fail\n");
		return 1;		
	}
	/* Some platforms require to setup MUX to IRQ functionality,for the rest just set MUXto GPIO functionality */
	lwgpio_set_functionality(&Lsm303D_MAG_pin, LWGPIO_MUX_D11_GPIO);
	lwgpio_set_attribute(&Lsm303D_MAG_pin, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);

	/* enable gpio functionality for given pin, react on falling edge */
	if (!lwgpio_int_init(&Lsm303D_MAG_pin, LWGPIO_INT_MODE_RISING))
	{
		printf("LSM GPIO init Fail\n");
		return 1;
	}
	/* install gpio interrupt service routine */
	_int_install_isr(lwgpio_int_get_vector(&Lsm303D_MAG_pin), int_service_routine_lsm303,(void *) &Lsm303D_MAG_pin);
	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
	_bsp_int_init(lwgpio_int_get_vector(&Lsm303D_MAG_pin), 3, 0, TRUE);
	/* Disable interrupt on GPIO peripheral module*/
	lwgpio_int_enable(&Lsm303D_MAG_pin, FALSE );
#endif

#endif	
	return 0;
}

/*-----------------------------------------------------------------------------
 *  Function:     enableLSM303D_interrupts
 *  Brief:        Enable LSM303D Interrupts  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
#if ENABLE_LSM303D
void enableLSM303D_interrupts(void)
{
	lwgpio_int_enable(&Lsm303D_MAG_pin, TRUE );
	lwgpio_int_enable(&Lsm303D_ACC_pin, TRUE );
	uint8_t Reg_val = ACC_DATA_RATE_200Hz | ACC_XYZ_EN;
	lsm303_i2c_write_polled(Lsm303_fd, CTRL1, (uchar_ptr) &Reg_val,1, 0);
	Reg_val = 0x00;
	lsm303_i2c_write_polled(Lsm303_fd, CTRL7, (uchar_ptr) &Reg_val,1, 0);

}
#endif

/*-----------------------------------------------------------------------------
 *  Function:     init_accelerometer
 *  Brief:        Function to initialise Accelerometer in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void init_accelerometer(void)
{
	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1, (uchar_ptr) &Lsm_commands[0],
			1, 0);

	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG4, (uchar_ptr) &Lsm_commands[1],
			1, 0);

}

/*-----------------------------------------------------------------------------
 *  Function:     init_magnetometer
 *  Brief:        Function to initialise magnetometer in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void init_magnetometer(void)
{

	lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG1, (uchar_ptr) &Lsm_commands[2],
			3, 1);

}

/*-----------------------------------------------------------------------------
 *  Function:     disable_acceleroometer
 *  Brief:        Function to disable accelerometer in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void disable_acceleroometer(void)
{
#if ENABLE_LSM303DLHC	
	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1,
			(uchar_ptr) &Lsm_enable_commands[0], 1, 0);
#else if ENABLE_LSM303D	
	uint8_t Reg_val = ACC_POWER_DOWN;	
	lsm303_i2c_write_polled(Lsm303_fd, CTRL1,(uchar_ptr) &Reg_val, 1, 1);
#endif
}


/*-----------------------------------------------------------------------------
 *  Function:     disable_magnetometer
 *  Brief:        Function to disable magnetometer in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void disable_magnetometer(void)
{
#if ENABLE_LSM303DLHC
	lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG3,
			(uchar_ptr) &Lsm_enable_commands[1], 1, 1);
			
#else if ENABLE_LSM303D
	uint8_t Reg_val = MAG_MOD_POWER_DOWN;	
	lsm303_i2c_write_polled(Lsm303_fd, CTRL7,(uchar_ptr) &Reg_val, 1, 1);
		
#endif
}

/*-----------------------------------------------------------------------------
 *  Function:     enable_magnetometer
 *  Brief:        function to enable Magnetic sensor in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void enable_magnetometer(void)
{
#if ENABLE_LSM303DLHC
	lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG3,
			(uchar_ptr) &Lsm_enable_commands[1], 1, 1);
#else if ENABLE_LSM303D
	uint8_t Reg_val = 0x64;
	Reg_val = MAG_MOD_SINGLE;
	lsm303_i2c_write_polled(Lsm303_fd, CTRL7,(uchar_ptr) &Reg_val, 1, 1);
#endif			
}
/*-----------------------------------------------------------------------------
 *  Function:     enable_acceleroometer
 *  Brief:        function to enable accelerometer in lsm303 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void enable_acceleroometer(void)
{
#if ENABLE_LSM303DLHC
	lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1,
			(uchar_ptr) &Lsm_enable_commands[0], 1, 0);
			
#else if ENABLE_LSM303D
	uint8_t Reg_val = ACC_DATA_RATE_400Hz | ACC_XYZ_EN;	
	lsm303_i2c_write_polled(Lsm303_fd, CTRL1,(uchar_ptr) &Reg_val, 1, 0);	
#endif
}


/*-----------------------------------------------------------------------------
 *  Function:     read_accelerometer_data
 *  Brief:        function to read accelerometer data from lsm303 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void read_accelerometer_data(void)
{
	//    memset(&Lsm303_data_buffer, 0, 6);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_X_L_A, &Lsm303_data_buffer[0], 1, 0);/*LSB at lower address*/
	lsm303_i2c_read_polled(Lsm303_fd, OUT_X_H_A, &Lsm303_data_buffer[1], 1, 0);/*MSB at lower address*/
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Y_L_A, &Lsm303_data_buffer[2], 1, 0);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Y_H_A, &Lsm303_data_buffer[3], 1, 0);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Z_L_A, &Lsm303_data_buffer[4], 1, 0);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Z_H_A, &Lsm303_data_buffer[5], 1, 0);
	Ax_raw = (int_16) ((uint_16) Lsm303_data_buffer[1] << 8) + (uint_16) Lsm303_data_buffer[0];
	Ay_raw = (int_16) ((uint_16) Lsm303_data_buffer[3] << 8) + (uint_16) Lsm303_data_buffer[2];
	Az_raw = (int_16) ((uint_16) Lsm303_data_buffer[5] << 8) + (uint_16) Lsm303_data_buffer[4];
	
	//printf("Ax=%d, Ay=%d, Az=%d \n", Ax_raw, Ay_raw, Az_raw);
#if ENABLE_LSM303DLHC
	Ax_raw = (Ax_raw >> 4);
	Ay_raw = (Ay_raw >> 4);
	Az_raw = (Az_raw >> 4);
#endif
	
	Ax =  (float)Ax_raw;
	Ay =  (float)Ay_raw;
	Az =  (float)Az_raw;
	//printf("Ax=%f, Ay=%f, Az=%f \n", Ax, Ay, Az);

}

/*-----------------------------------------------------------------------------
 *  Function:     read_magnetometer_data
 *  Brief:        function to read magnetometer data from lsm303 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
void read_magnetometer_data(void)
{
	//    memset(&Lsm303_data_buffer, 0, 6);
#if ENABLE_LSM303DLHC	
	
	lsm303_i2c_read_polled(Lsm303_fd, OUT_X_H_M, Lsm303_data_buffer, 6, 1);
	
	Bx = (int_16) ((uint_16) Lsm303_data_buffer[0] << 8) + (uint_16) Lsm303_data_buffer[1];
	By = (int_16) ((uint_16) Lsm303_data_buffer[4] << 8) + (uint_16) Lsm303_data_buffer[5];
	Bz = (int_16) ((uint_16) Lsm303_data_buffer[2] << 8) + (uint_16) Lsm303_data_buffer[3];
	
	if ((Bx & 0x0800) == 0x0800) Bx |= 0xF000;
	if ((By & 0x0800) == 0x0800) By |= 0xF000;	
	if ((Bz & 0x0800) == 0x0800) Bz |= 0xF000;
	
#else if ENABLE_LSM303D	
	
	lsm303_i2c_read_polled(Lsm303_fd, OUT_X_H_M, &Lsm303_data_buffer[0] , 1, 1);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_X_L_M, &Lsm303_data_buffer[1] , 1, 1);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Y_H_M, &Lsm303_data_buffer[2] , 1, 1);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Y_L_M, &Lsm303_data_buffer[3] , 1, 1);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Z_H_M, &Lsm303_data_buffer[4] , 1, 1);
	lsm303_i2c_read_polled(Lsm303_fd, OUT_Z_L_M, &Lsm303_data_buffer[5] , 1, 1);
	
	Bx = (int_16) ((uint_16) Lsm303_data_buffer[0] << 8) + (uint_16) Lsm303_data_buffer[1];
	By = (int_16) ((uint_16) Lsm303_data_buffer[2] << 8) + (uint_16) Lsm303_data_buffer[3];	
	Bz = (int_16) ((uint_16) Lsm303_data_buffer[4] << 8) + (uint_16) Lsm303_data_buffer[5];
	
#endif	
}

/*-----------------------------------------------------------------------------
 *  Function:     get_slope_measurement
 *  Brief:        function to read slope measurement data from lsm303 
 *  Parameter:    Address of the variables to store slope angle ,aspect, magnetic heading
 *  Return:       None
 -----------------------------------------------------------------------------*/
uint_8 get_slope_measurement(int_16 *slope_angle, char *aspect, uint_16 *magnetic_heading)
{
	if (Lsm_Data_Ready == 1)
	{
		Get_slope_measurement_Lsm_Data();

		*magnetic_heading = (uint_16)final_yaw;
		*slope_angle = (int_16)final_roll;
		get_direction(aspect, *magnetic_heading);
		//   printf("\nslope: %d,Compass: %d %s\n", *slope_angle, *magnetic_heading, aspect);
		Slope_Measurment_Result_Slope_Angle = *slope_angle;
		strcpy(Slope_Measurment_Result_Aspect, aspect);
		Start_LSM();
		return 0;

	}
	else
	{
		*magnetic_heading = 0xFFFF;
		*slope_angle = 0xFFFF;
		strcpy(aspect, "NA");
		//		printf("\nLSM DATA NOT READY\n");
		return 1;
	}

}

/*-----------------------------------------------------------------------------
 *  Function:     get_heading
 *  Brief:        function to calculate heading from lsm303 raw data 
 *  Parameter:    none
 *  Return:       heading (in degrees)
 -----------------------------------------------------------------------------*/
static ieee_double get_heading(void)
{
	ieee_double heading = 0;
	Vector_normalize(&a_avg); /*Normalization*/
	Pitch_radian = asin(-a_avg.x);/*in radians/inverse sine */
	Pitch_degree = Pitch_radian * RAD_TO_DEG; /*in degreee */
	if (cos(Pitch_radian) != 0)
		Roll_radian = asin(a_avg.y / cos(Pitch_radian)); /*in radians/inverse sine */
	else
		Roll_radian = 0;
	Roll_degree = Roll_radian * RAD_TO_DEG; /*in degreee */
	printf("Pitch_degree : %f\tRoll Degree: %f\n",Pitch_degree,Roll_degree);
	Vector_normalize(&m_avg); /*Normalization*/
	//	Mx_compensated = (m_avg.x * cos(Pitch_radian) + m_avg.z * sin(Pitch_radian));
	//	My_compensated = ((m_avg.x * sin(Roll_radian) * cos(Pitch_radian))
	//			+ (m_avg.y * cos(Roll_radian)) - (m_avg.z * sin(Roll_radian) * cos(Roll_radian)));
	//	Mz_compensated = ((-m_avg.x * cos(Roll_radian) * sin(Pitch_radian))
	//			+ (m_avg.y * sin(Roll_radian)) + (m_avg.z * cos(Roll_radian) * sin(Pitch_radian)));

	Mx_compensated = (m_avg.x * cos(Pitch_radian) + m_avg.z * sin(Pitch_radian));
	My_compensated = ((m_avg.x * sin(Roll_radian) * sin(Pitch_radian))
			+ (m_avg.y * cos(Roll_radian)) - (m_avg.z * sin(Roll_radian) * cos(Pitch_radian)));
	Mz_compensated = ((-m_avg.x * cos(Roll_radian) * sin(Pitch_radian))
			+ (m_avg.y * sin(Roll_radian)) + (m_avg.z * cos(Roll_radian) * cos(Pitch_radian)));

	heading = atan(My_compensated / Mx_compensated);
	heading = heading * RAD_TO_DEG;
	if ((Mx_compensated > 0) && (My_compensated >= 0))
		heading = heading;
	else if (Mx_compensated < 0)
		heading = heading + 180;
	else if ((Mx_compensated > 0) && (My_compensated <= 0))
		heading = heading + 360;
	else if ((Mx_compensated == 0) && (My_compensated < 0))
		heading = 90;
	else if ((Mx_compensated == 0) && (My_compensated > 0))
		heading = 270;
	return heading;
}

/*-----------------------------------------------------------------------------
 *  Function:     get_direction
 *  Brief:        function to aspect from heading angle
 *  Parameter:    Address of heading variable, pointer to store aspect string  
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void get_direction(char * ribbon_segment, ieee_double heading)
{
	ribbon_segment[0] = 0;
	ribbon_segment[1] = 0;
	ribbon_segment[2] = 0;
	if ((heading <= 20) || (heading > 340))
	{
		ribbon_segment[0] = 'N';
		ribbon_segment[1] = '\0';
	}
	else if ((heading <= 70) && (heading > 20))
	{
		ribbon_segment[0] = 'N';
		ribbon_segment[1] = 'E';
		ribbon_segment[2] = '\0';
	}
	else if ((heading <= 110) && (heading > 70))
	{
		ribbon_segment[0] = 'E';
		ribbon_segment[1] = '\0';
	}
	else if ((heading <= 160) && (heading > 110))
	{
		ribbon_segment[0] = 'S';
		ribbon_segment[1] = 'E';
		ribbon_segment[2] = '\0';
	}
	else if ((heading <= 200) && (heading > 160))
	{
		ribbon_segment[0] = 'S';
		ribbon_segment[1] = '\0';
	}
	else if ((heading <= 250) && (heading > 200))
	{
		ribbon_segment[0] = 'S';
		ribbon_segment[1] = 'W';
		ribbon_segment[2] = '\0';
	}
	else if ((heading <= 290) && (heading > 250))
	{
		ribbon_segment[0] = 'W';
		ribbon_segment[1] = '\0';
	}
	else if ((heading <= 340) && (heading > 290))
	{
		ribbon_segment[0] = 'N';
		ribbon_segment[1] = 'W';
		ribbon_segment[2] = '\0';
	}

}

/*-----------------------------------------------------------------------------
 *  Function:     lsm303_i2c_write_polled
 *  Brief:        function to write data to the i2c device
 *  Parameter:    The file pointer for the I2C channel, The address in device to write to,
 *                The array of characters are to be written in device , 
 *                Number of bytes to be written,Data size,mode to select device *                    
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void lsm303_i2c_write_polled(
		/* [IN] The file pointer for the I2C channel */
		MQX_FILE_PTR Lsm303_fd,

		/* [IN] The address in device to write to */
		uint_32 addr,
		/* [IN] The array of characters are to be written in device */
		uchar_ptr Lsm303_buffer,
		/* [IN] Number of bytes in that Lsm303_buffer */
		_mqx_int n,
		/* [IN] Number of bytes in that Lsm303_buffer */
		_mqx_int mode)
{ /* Body */
	uint_32 param;
	_mqx_int length;
	_mqx_int result;
	uint_8 mem[MEMORY_WIDTH];
	do
	{
		if (!mode)
		{
			param = I2C_ACCELEROMETER_BUS_ADDRESS;
		}
		else
		{
			param = I2C_MAGNETOMETER_BUS_ADDRESS;
		}
		if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
		{
			printf("Set Destination Address ERROR\n");
		}

		length = (_mqx_int) (PAGE_SIZE - (addr & (PAGE_SIZE - 1)));
		if (length > n)
			length = n;
		/* Initiate start and send I2C bus address */
		fwrite(mem, 1, 0, Lsm303_fd);
		/* Check ack (device exists) */
		if (I2C_OK == ioctl(Lsm303_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
		{
			if (param)
			{
				/* Stop I2C transfer */
				if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_STOP, NULL))
				{
					printf("i2c Stop ERROR\n");
				}
				break;
			}
		}
		else
		{
			printf("i2c Flush out put ERROR\n");

		}
		/* Write address within memory block */
#if MEMORY_WIDTH == 2
		mem[0] = (uint_8)(addr >> 8);
		mem[1] = (uint_8)addr;

		result = fwrite (mem, 1, 2, Lsm303_fd);
#else
		mem[0] = (uint_8) addr;
		result = fwrite (mem, 1, 1, Lsm303_fd);
#endif
		/* Page write of data */
		result = fwrite (Lsm303_buffer, 1, length, Lsm303_fd);
		if (result != length)
		{
			printf("i2c fwrite ERROR\n");
		}
		/* Wait for completion */
		result = fflush(Lsm303_fd);
		if (MQX_OK != result)
		{
			printf("i2c fflush ERROR\n");
		}
		/* Stop I2C transfer - initiate device write cycle */

		if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_STOP, NULL))
		{
			printf("i2c stop ERROR\n");

		}
		/* Wait for device write cycle finish - acknowledge */
		result = 0;
		do
		{
			/* Sends just I2C bus address, returns acknowledge bit and stops */
			fwrite(&result, 1, 0, Lsm303_fd);
			if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
			{
				printf("  ERROR during wait (flush)\n");

			}
			if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_STOP, NULL))
			{
				printf("  ERROR during wait (stop)\n");

			}
			result++;
		} while ((param & 1) || (result <= 1));

		/* Next round */
		Lsm303_buffer += length;
		addr += length;
		n -= length;

	} while (n);

}

/*-----------------------------------------------------------------------------
 *  Function:     lsm303_i2c_read_polled
 *  Brief:        function to read data from the i2c device
 *  Parameter:    The file pointer for the I2C channel, The address in device to read
 *                from,The array to store data, Number of bytes to read, 
 *                Data size,mode to select device                
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void lsm303_i2c_read_polled(
		/* [IN] The file pointer for the I2C channel */
		MQX_FILE_PTR Lsm303_fd,
		/* [IN] The address in device to read from */
		uint_32 addr,
		/* [IN] The array to store data*/
		uchar_ptr Lsm303_buffer,
		/* [IN] Number of bytes to read */
		_mqx_int n,
		/* [IN] Size of bytes to read */
		_mqx_int mode)
{ /* Body */
	_mqx_int param;
	_mqx_int result;
	uint_8 mem[MEMORY_WIDTH];
	if (0 == n)
	{
		return;
	}
	if (!mode)
	{

		param = I2C_ACCELEROMETER_BUS_ADDRESS;
	}
	else
	{
		param = I2C_MAGNETOMETER_BUS_ADDRESS;

	}
	if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
	{
		printf("i2c Set destination address ERROR\n");
	}
	/* Initiate start and send I2C bus address */
	fwrite(mem, 1, 0, Lsm303_fd);
	/* Check ack (device exists) */
	if (I2C_OK == ioctl(Lsm303_fd, IO_IOCTL_FLUSH_OUTPUT, &param))
	{
		if (param)
		{
			/* Stop I2C transfer */
			if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_STOP, NULL))
			{
				printf("i2c stop ERROR\n");

			}
			return;
		}
	}
	else
	{
		printf("i2c Flush out put ERROR\n");

	}
	/* Write address within memory block */
#if MEMORY_WIDTH == 2
	mem[0] = (uint_8)(addr >> 8);
	mem[1] = (uint_8)addr;

	result = fwrite (mem, 1, 2, Lsm303_fd);
#else
	mem[0] = (uint_8) addr;
	result = fwrite (mem, 1, 1, Lsm303_fd);
#endif
	/* Wait for completion */
	result = fflush(Lsm303_fd);
	/* Restart I2C transfer for reading */
	if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_REPEATED_START, NULL))
	{
		printf("i2c Repeated start ERROR\n");
	}
	/* Set read request */
	param = n;
	if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
	{
		printf("Set number of bytes requested to%d ERROR\n", param);
	}
	/* Read all data */
	result = fread (Lsm303_buffer, 1, n, Lsm303_fd);
	if (result != n)
	{
		printf("  Read %d bytes ERROR\n ", n);
	}
	/* Stop I2C transfer - initiate device write cycle */
	if (I2C_OK != ioctl(Lsm303_fd, IO_IOCTL_I2C_STOP, NULL))
	{
		printf("i2c Stop ERROR\n");
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     Stop_LSM
 *  Brief:        function stop lsm303 device
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Stop_LSM(void)
{
#if ENABLE_LSM303DLHC
	lwgpio_int_enable(&Lsm303_int_pin, FALSE );
#else if ENABLE_LSM303D
	lwgpio_int_enable(&Lsm303D_MAG_pin, FALSE );
	lwgpio_int_enable(&Lsm303D_ACC_pin, FALSE );
#endif
}

/*-----------------------------------------------------------------------------
 *  Function:     Start_LSM
 *  Brief:        function start lsm303 device
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Start_LSM(void)
{
	Lsm_Data_Ready = 0;
	
#if ENABLE_LSM303DLHC
	lwgpio_int_enable(&Lsm303_int_pin, TRUE );
#else if ENABLE_LSM303D
	lwgpio_int_enable(&Lsm303D_MAG_pin,TRUE);
	lwgpio_int_enable(&Lsm303D_ACC_pin,TRUE);
#endif
	
	enable_acceleroometer();
	enable_magnetometer();
	
	
}

void Start_Test_LSM(void)
{
	Lsm_Data_Ready = 0;
	lwgpio_int_enable(&Lsm303_TMP_pin, TRUE );
}

/*-----------------------------------------------------------------------------
 *  Function:     clear_lsm_data_buffer
 *  Brief:        function clear lsm303 data buffer
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void clear_lsm_data_buffer(void)
{
	char i = 0;
	for (i = 0; i < MAX_LSM_SAMPLE_COUNT; i++)
	{
		Data_Write_file.LSM_MeterData[i].AX_data = 0;
		Data_Write_file.LSM_MeterData[i].AY_data = 0;
		Data_Write_file.LSM_MeterData[i].AZ_data = 0;
		Data_Write_file.LSM_MeterData[i].MX_data = 0;
		Data_Write_file.LSM_MeterData[i].MY_data = 0;
		Data_Write_file.LSM_MeterData[i].MZ_data = 0;
	}
}

/*-----------------------------------------------------------------------------
 *  Function:     Get_slope_measurement_Lsm_Data
 *  Brief:        function read raw data to calculate slope
 *  Parameter:    Address of variable to store data 
 *  Return:       none
 -----------------------------------------------------------------------------*/
static void Get_slope_measurement_Lsm_Data(void)
{
	float Roll,Pitch,Yaw;
	
	Lsm_Data_Ready = 0;
	read_magnetometer_data();
	read_accelerometer_data();
	
	basicmagcalib(&Bx,&By,&Bz);
	fusion6(Gx,Gy,Gz,Bx,By,Bz,&Roll,& Pitch,&Yaw);
#if ENABLE_LSM303DLHC
	final_yaw = (Yaw*RAD_TO_DEG) + 180 + 270;
	final_yaw = final_yaw % 360;
	final_yaw = (360 - final_yaw + 180)%360;
	final_roll = (Roll*RAD_TO_DEG);
	if(final_roll > 0)
		final_roll = (final_roll-180+15)%180;
	else 
		final_roll = (final_roll+180+15)%180;
	Pitch = Pitch*RAD_TO_DEG;
	
#else if ENABLE_LSM303D
	
	Yaw = 180*Yaw/M_PI;
	final_yaw = Yaw *-1;
	/*We will also convert the values so that they are in the range of 0 to 360*/
	if(final_yaw < 0)
		final_yaw += 360;
	
//	final_roll = (Roll*RAD_TO_DEG);
//	if(final_roll > 0)
//		final_roll = (final_roll-180+15)%180;
//	else 
//		final_roll = (final_roll+180+15)%180;
	final_roll = Pitch*RAD_TO_DEG;
		
#endif

}

/*-----------------------------------------------------------------------------
 *  Function:     basicmagcalib
 *  Brief:        Ultra simple magnetometer calibration. Compensate offset and scale IF AND ONLY IF the full range on each axis is stimulated!
 	 	 	 	  When magnetometer is not ready, values read are (0,0,0). They need to be rejected not to compromise the calibration
 *  Parameter:    Address of variable to store data 
 *  Return:       none
 -----------------------------------------------------------------------------*/
void basicmagcalib(int *cBx, int *cBy, int *cBz)
{
//
	int Bx, By, Bz; 

	if((*cBx==0)&&(*cBy==0)&&(*cBz==0)) return; // reject outliers

	Bx=*cBx; By=*cBy; Bz=*cBz;                                                      
	if (Bx<Bxmin) { Bxmin=Bx; Bxofs=(Bxmax+Bxmin)>>1; Bxrange2=(Bxmax-Bxmin)>>1; }  
	if (Bx>Bxmax) { Bxmax=Bx; Bxofs=(Bxmax+Bxmin)>>1; Bxrange2=(Bxmax-Bxmin)>>1; }  
	if (By<Bymin) { Bymin=By; Byofs=(Bymax+Bymin)>>1; Byrange2=(Bymax-Bymin)>>1; }  
	if (By>Bymax) { Bymax=By; Byofs=(Bymax+Bymin)>>1; Byrange2=(Bymax-Bymin)>>1; }  
	if (Bz<Bzmin) { Bzmin=Bz; Bzofs=(Bzmax+Bzmin)>>1; Bzrange2=(Bzmax-Bzmin)>>1; }  
	if (Bz>Bzmax) { Bzmax=Bz; Bzofs=(Bzmax+Bzmin)>>1; Bzrange2=(Bzmax-Bzmin)>>1; }  
	if (Bxrange2!=0) *cBx=(Bx-Bxofs)*MAGRANGE/Bxrange2;    
	if (Byrange2!=0) *cBy=(By-Byofs)*MAGRANGE/Byrange2;          
	if (Bzrange2!=0) *cBz=(Bz-Bzofs)*MAGRANGE/Bzrange2;

	return;
}

/*-----------------------------------------------------------------------------
 *  Function:    fusion6
 *  Brief:       6-axis data fusion Acc+Mag -> Euler angles
 *  Parameter:   acc(Gx,Gy,Gz)  mag(Bx,By,Bz)   phi = atan2f(Gy,Gz) theta = atanf(-Gx./(Gy.*sinf(phi)+Gz.*cosf(phi)))
  	  	  	  	 psi = atan2f((Bz.*sinf(phi)-By.*cosf(phi)),(Bx.*cosf(theta)+By.*sinf(theta).*sinf(phi)+Bz.*sinf(theta).*cosf(phi)));
 *  Return:      none
 -----------------------------------------------------------------------------*/
void fusion6(float Gx, float Gy, float Gz, int Bx, int By, int Bz, float *Roll, float *Pitch, float *Yaw)
{
	float By2, Bz2, Gz2, Bx3; //Bz3;
	float sphi, cphi, stheta, ctheta;
	float phi, theta, psi;

	phi = atan2f(Gy,Gz);
	sphi = sinf(phi);
	cphi = cosf(phi);

	By2 =  Bz*sphi - By*cphi;
	Bz2 =  By*sphi + Bz*cphi;
	Gz2 =  Gy*sphi + Gz*cphi;

	if (Gz2!=0.0) theta = atanf(-Gx / Gz2);
	stheta = sinf(theta);
	ctheta = cosf(theta);

	Bx3 = +Bx*ctheta + Bz2*stheta;
	//Bz3 = -Bx*stheta + Bz2*ctheta;

	psi = atan2f(By2,Bx3);

	// no output lowpass filter
	*Roll=phi; 
	*Pitch=theta;
	*Yaw=psi;

	return;
}

/*-----------------------------------------------------------------------------
 *  Function:     Get_Lsm_Data
 *  Brief:        function to store lsm303 data in global buffer 
 *  Parameter:    none
 *  Return:       none
 -----------------------------------------------------------------------------*/
void Get_Lsm_Data()
{
	Lsm_Data_Ready = 0;
	read_magnetometer_data();
	read_accelerometer_data();
	if (lsm_data_count <= MAX_LSM_SAMPLE_COUNT)
	{
		if(lsm_data_count==0)
		{
			printf("\nLSM data count zero....!!!\n");
		}
		else
		{
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].AX_data = Ax;
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].AY_data = Ay;
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].AZ_data = Az;
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].MX_data = Mx;
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].MY_data = My;
			//			Data_Write_file.LSM_MeterData[lsm_data_count - 1].MZ_data = Mz;
		}
	}

}

/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
