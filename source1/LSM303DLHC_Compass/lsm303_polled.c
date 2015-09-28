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
int_16 Ax, Ay, Az, Mx, My, Mz;
//uchar_ptr Lsm303_buffer;
uchar Lsm303_data_buffer[LSM_BUFFER_SIZE];
MQX_FILE_PTR Lsm303_fd;
ieee_double Pitch_radian, Pitch_degree, Roll_radian, Roll_degree, Mx_compensated, My_compensated,
        Mz_compensated;
vector a_avg = { 0, 0, 0 }, m_avg = { 0, 0, 0 }, a = { 0, 0, 0 }, m = { 0, 0, 0 };
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
static void Get_slope_measurement_Lsm_Data(vector *a, vector *m);

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
    
    printf("LSM un-initialised..!");	
    Sensor_status.LSM_status_status = NOT_INITIALISED;
    return 0;
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
    
    init_accelerometer();
    init_magnetometer();
    
    disable_magnetometer();
    disable_acceleroometer();
    
    init_gpio_isr_lsm303();
    
    printf("LSM  initialisation Success..!");
    Sensor_status.LSM_status_status = INITIALISED;
    return 0;
}

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
	if((State_of_Screen == UI_NEW_SNOW_PROFILE_CALIBRATING)||(State_of_Screen == UI_NEW_SNOW_PROFILE_COLLECTING))
	{
		lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
		_lwevent_set(&Lsm_Event, SW_EVENT_MASK);
//	    read_magnetometer_data();
//	    read_accelerometer_data();
	}
	else
	{
		lwgpio_int_enable(&Lsm303_TMP_pin, FALSE );
		lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
		Lsm_Data_Ready = 1;
		lsm_data_count++;
		if (lsm_data_count > MAX_LSM_SAMPLE_COUNT)
		{
			lsm_data_count = MAX_LSM_SAMPLE_COUNT;
		}
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
    /*
     Opening the pin for input, initialize interrupt on it and set interrupt handler*/
    /* opening pins for input */
    if (!lwgpio_init(&Lsm303_TMP_pin, BSP_LSM_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        return 1;
        printf("LSM GPIO init as i/p  Fail\n");
    }
    /* Some platforms require to setup MUX to IRQ functionality,
     for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&Lsm303_TMP_pin, LWGPIO_MUX_B9_GPIO);
    lwgpio_set_attribute(&Lsm303_TMP_pin, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    
    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&Lsm303_TMP_pin, LWGPIO_INT_MODE_FALLING))
    {
        printf("LSM GPIO init Fail\n");
        return 1;
    }
    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&Lsm303_TMP_pin), int_service_routine_lsm303,
            (void *) &Lsm303_TMP_pin);
    /* set the interrupt level, and unmask the interrupt in interrupt controller*/
    _bsp_int_init(lwgpio_int_get_vector(&Lsm303_TMP_pin), 3, 0, TRUE);
    /* Disable interrupt on GPIO peripheral module*/
    lwgpio_int_enable(&Lsm303_TMP_pin, FALSE );
    return 0;
}

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
    lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1,
            (uchar_ptr) &Lsm_enable_commands[0], 1, 0);
}

/*-----------------------------------------------------------------------------
 *  Function:     disable_magnetometer
 *  Brief:        Function to disable magnetometer in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void disable_magnetometer(void)
{
    lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG3,
            (uchar_ptr) &Lsm_enable_commands[1], 1, 1);
}

/*-----------------------------------------------------------------------------
 *  Function:     enable_magnetometer
 *  Brief:        function to enable Magnetic sensor in Lsm303  
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void enable_magnetometer(void)
{
    lsm303_i2c_write_polled(Lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG3,
            (uchar_ptr) &Lsm_enable_commands[1], 1, 1);
}

/*-----------------------------------------------------------------------------
 *  Function:     enable_acceleroometer
 *  Brief:        function to enable accelerometer in lsm303 
 *  Parameter:    None
 *  Return:       None
 -----------------------------------------------------------------------------*/
static void enable_acceleroometer(void)
{
    lsm303_i2c_write_polled(Lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1,
            (uchar_ptr) &Lsm_enable_commands[0], 1, 0);
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
    Ax = (int_16) ((uint_16) Lsm303_data_buffer[1] << 8) + (uint_16) Lsm303_data_buffer[0];
    Ay = (int_16) ((uint_16) Lsm303_data_buffer[3] << 8) + (uint_16) Lsm303_data_buffer[2];
    Az = (int_16) ((uint_16) Lsm303_data_buffer[5] << 8) + (uint_16) Lsm303_data_buffer[4];
    Ax = (Ax >> 4);
    Ay = (Ay >> 4);
    Az = (Az >> 4);
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
    lsm303_i2c_read_polled(Lsm303_fd, OUT_X_H_M, Lsm303_data_buffer, 6, 1);
    Mx = (int_16) ((uint_16) Lsm303_data_buffer[0] << 8) + (uint_16) Lsm303_data_buffer[1];
    if ((Mx & 0x0800) == 0x0800)
        Mx |= 0xF000;
    My = (int_16) ((uint_16) Lsm303_data_buffer[4] << 8) + (uint_16) Lsm303_data_buffer[5];
    if ((My & 0x0800) == 0x0800)
        My |= 0xF000;
    Mz = (int_16) ((uint_16) Lsm303_data_buffer[2] << 8) + (uint_16) Lsm303_data_buffer[3];
    if ((Mz & 0x0800) == 0x0800)
        Mz |= 0xF000;
}

/*-----------------------------------------------------------------------------
 *  Function:     get_slope_measurement
 *  Brief:        function to read slope measurement data from lsm303 
 *  Parameter:    Address of the variables to store slope angle ,aspect, magnetic heading
 *  Return:       None
 -----------------------------------------------------------------------------*/
void get_slope_measurement(int_16 *slope_angle, char *aspect, uint_16 *magnetic_heading)
{
    if (Lsm_Data_Ready == 1)
    {
        Get_slope_measurement_Lsm_Data(&a_avg, &m_avg);
        *magnetic_heading = (uint_16) get_heading();
        *magnetic_heading = (*magnetic_heading + 90) % 360;
        *slope_angle = 20 - Roll_degree;
        get_direction(aspect, *magnetic_heading);
        printf("\nslope: %d,Compass: %d %s\n", *slope_angle, *magnetic_heading, aspect);
        Slope_Measurment_Result_Slope_Angle = *slope_angle;
        strcpy(Slope_Measurment_Result_Aspect, aspect);
        Start_LSM();
        
    }
    else
    {
        *magnetic_heading = 0xFFFF;
        *slope_angle = 0xFFFF;
        strcpy(aspect, "NA");
        printf("\nLSM DATA NOT READY\n");
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
    Vector_normalize(&m_avg); /*Normalization*/
    Mx_compensated = (m_avg.x * cos(Pitch_radian) + m_avg.z * sin(Pitch_radian));
    My_compensated = ((m_avg.x * sin(Roll_radian) * cos(Pitch_radian))
            + (m_avg.y * cos(Roll_radian)) - (m_avg.z * sin(Roll_radian) * cos(Roll_radian)));
    Mz_compensated = ((-m_avg.x * cos(Roll_radian) * sin(Pitch_radian))
            + (m_avg.y * sin(Roll_radian)) + (m_avg.z * cos(Roll_radian) * sin(Pitch_radian)));
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
    lwgpio_int_enable(&Lsm303_TMP_pin, FALSE );
//    disable_magnetometer();
//    disable_acceleroometer();
//    Lsm_Data_Ready = 0;
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
    lwgpio_int_enable(&Lsm303_TMP_pin, TRUE );
    printf("mag and acc enabling\n");
    enable_magnetometer();
    enable_acceleroometer();
    printf("mag and acc enabled\n");
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
static void Get_slope_measurement_Lsm_Data(vector *a, vector *m)
{
    Lsm_Data_Ready = 0;
    read_magnetometer_data();
    m->x = (float) Mx / MAGNETOMETR_XY_GAIN;
    m->y = (float) My / MAGNETOMETR_XY_GAIN;
    m->z = (float) Mz / MAGNETOMETR_Z_GAIN;
    read_accelerometer_data();
    a->x = (float) Ax / ACCELROMETER_GAIN;
    a->y = (float) Ay / ACCELROMETER_GAIN;
    a->z = (float) Az / ACCELROMETER_GAIN;
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
void Test_lsm303(void )
{       
	printf("\n************STARTING LSM303 TEST****************\n");
	buff_clear();
	Draw_Image_on_Buffer((uint_8 *) both_footer_background);
	Create_Title("LSM TEST",strlen("LSM TEST"));
	Draw_string_new(15,80, (uint_8 *)"STARTING LSM TEST",COLOUR_BLACK,MEDIUM_FONT);
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	_time_delay(2000);

	_mqx_int param;

	/* Open the I2C driver */
	Lsm303_fd = fopen(I2C_DEVICE_POLLED, NULL);
	if (Lsm303_fd == NULL)
	{
		printf("ERROR Lsm303_fd == NULL!\n");
	}
	param = 100000;
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_BAUD, &param);
	ioctl(Lsm303_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
	
#if ENABLE_LSM303DLHC
	uchar CONTROL_REG_STRING[] = {0x47,0x00,0x14,0x20,0x00};
	printf("\nInitializing Accelerometer.");
	lsm303_i2c_write_polled (lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1, (uchar_ptr)&CONTROL_REG_STRING[0],1,0);
	lsm303_i2c_write_polled (lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG4, (uchar_ptr)&CONTROL_REG_STRING[1],1,0); 
	printf("\nInitializing Magnetometer.");
	lsm303_i2c_write_polled (lsm303_fd, I2C_MAGNETOMETER_CONTROL_REG1,(uchar_ptr)&CONTROL_REG_STRING[2],3,1);
	
	lsm303_i2c_write_polled (lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1, (uchar_ptr)&CONTROL_REG_STRING[0],1,0);	
	lsm303_i2c_read_polled (lsm303_fd, I2C_ACCELEROMETER_CONTROL_REG1 , (uchar_ptr)&lsm303_buffer[0], 1,0);/*LSB at lower address*/

#else if ENABLE_LSM303D	
	uint8_t CTRL_Values[8] = {0x00,0x87,0x00,0x00,0x04,0x64,0x00,0x02};
	uint8_t Dev_ID[5] = {0};
	lsm303_i2c_write_polled(Lsm303_fd, CTRL1, (uchar_ptr) &CTRL_Values[1],1, 0);
	lsm303_i2c_read_polled(Lsm303_fd, CTRL1, (uchar_ptr) &Dev_ID[1],1, 0);	
#endif

#if ENABLE_LSM303DLHC
	if(CONTROL_REG_STRING[0]==lsm303_buffer[0])
#else if ENABLE_LSM303D
	if(CTRL_Values[1]==Dev_ID[1])	
#endif 
	{
		printf("\nLSM303 Test Success\n");
		Draw_string_new(10,200, (uint_8 *)"LSM TEST SUCCESS",COLOUR_BLACK,MEDIUM_FONT);
	}
	else
	{
		printf("\nLSM303 Test Fail\n");
		Draw_string_new(10,200, (uint_8 *)"LSM TEST FAILED",COLOUR_BLACK,MEDIUM_FONT);
	}

	printf("\n\n*********LSM303 TEST COMPLETED********\n\n");
	Refresh_Lcd_Buffer((uint_8 *) frame_buff);
	fclose(Lsm303_fd);
	_time_delay(1000);
}
/*-----------------------------------------------------------------------------
 *************************        END        **********************************
 -----------------------------------------------------------------------------*/
