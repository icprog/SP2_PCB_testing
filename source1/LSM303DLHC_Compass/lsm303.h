#ifndef __LSM303_h__
#define __LSM303_h__
#include "vector.h"
/**HEADER********************************************************************

 * $FileName: lsm303.h$
 *
 *END************************************************************************/

#if defined(BSP_MPC8308RDB)
#define PAGE_SIZE       0x40     /* Maximum bytes per write*/
#define MEMORY_WIDTH    0x02     /* Memory address width in bytes */
#elif defined(BSP_M53015EVB)
#define PAGE_SIZE       0x08     /* Maximum bytes per write*/
#define MEMORY_WIDTH    0x01     /* Memory address width in bytes */
#else
#define PAGE_SIZE       (0x10)     /* Maximum bytes per write      */
#define MEMORY_WIDTH    (0x01)     /* Memory address width in bytes */
#endif

#if ENABLE_LSM303DLHC
/* The I2C ACCELEROMETER test addresses */
#define I2C_ACCELEROMETER_BUS_ADDRESS     (0x19)     /* I2C bus address of ACCELEROMETER */
#define I2C_MAGNETOMETER_BUS_ADDRESS      (0x1E)     /* I2C bus address of MAGNETOMETER */
#define I2C_ACCELEROMETER_CONTROL_REG1    (0x20)     /* ACCELEROMETER CONTROL REGISTER 1*/
#define I2C_ACCELEROMETER_CONTROL_REG4    (0x23)     /* ACCELEROMETER CONTROL REGISTER 4*/
#define I2C_MAGNETOMETER_CONTROL_REG1     (0x00)     /* MAGNETOMETER CONTROL REGISTER 1*/
#define I2C_MAGNETOMETER_CONTROL_REG2     (0x01)     /* MAGNETOMETER CONTROL REGISTER 2*/
#define I2C_MAGNETOMETER_CONTROL_REG3     (0x02)     /* MAGNETOMETER CONTROL REGISTER 3*/

/*output registers of accelerometer*/
#define OUT_X_L_A (0x28)
#define OUT_X_H_A (0x29)
#define OUT_Y_L_A (0x2A)
#define OUT_Y_H_A (0x2B)
#define OUT_Z_L_A (0x2C)
#define OUT_Z_H_A (0x2D)
/*output registers of magnetic sensor*/
#define OUT_X_H_M  (0x03)
#define OUT_X_L_M  (0x04)
#define OUT_Y_H_M  (0x07)
#define OUT_Y_L_M  (0x08)
#define OUT_Z_H_M  (0x05)
#define OUT_Z_L_M  (0x06)

#else if ENABLE_LSM303D

#define I2C_ACCELEROMETER_CONTROL_REG1    	(0x20)     /* ACCELEROMETER CONTROL REGISTER 1*/
#define I2C_ACCELEROMETER_CONTROL_REG4    	(0x23)     /* ACCELEROMETER CONTROL REGISTER 4*/
#define I2C_MAGNETOMETER_CONTROL_REG1     	(0x00)     /* MAGNETOMETER CONTROL REGISTER 1*/
#define I2C_MAGNETOMETER_CONTROL_REG2     	(0x01)     /* MAGNETOMETER CONTROL REGISTER 2*/
#define I2C_MAGNETOMETER_CONTROL_REG3     	(0x02)     /* MAGNETOMETER CONTROL REGISTER 3*/

#define I2C_ACCELEROMETER_BUS_ADDRESS     	(0x1E)     /* I2C bus address of ACCELEROMETER */
#define I2C_MAGNETOMETER_BUS_ADDRESS      	(0x1E)     /* I2C bus address of MAGNETOMETER */

#define LSM303D_DEVICE_ADDRESS 				(0x1E)	//If SA0 is high, 1D else if SA0 is grounded, 1E

#define WHO_AM_I   							(0x0F)	//Device Identification Register; Default Value: 01001001
#define STATUS_M   							(0x07)   //Magnetic X,Y,Z axis data overrun indication status bit; may be required.
#define STATUS_A   							(0x27)	//Acceletometer X,Y,Z axis data overrun status register.

/*Interrupt Registers */
#define INT_CTRL_M  						(0x12)	//Magnetometer Interrupt Control Register; Default Value: 11101000    (R/W)
#define INT_SRC_M   						(0x13)  //Latched Interrupt Requests for mag; interrupts clear upon reading.  (R)

/*output registers of accelerometer; The value is expressed in 16-bit as two’s complement.*/
#define OUT_X_L_A 							(0x28)
#define OUT_X_H_A 							(0x29)
#define OUT_Y_L_A 							(0x2A)
#define OUT_Y_H_A 							(0x2B)
#define OUT_Z_L_A 							(0x2C)
#define OUT_Z_H_A 							(0x2D)

/*output registers of magnetic sensor; The value is expressed in 16-bit as two’s complement.*/
#define OUT_X_H_M 							(0x09)
#define OUT_X_L_M 							(0x08)
#define OUT_Y_H_M 							(0x0B)
#define OUT_Y_L_M  						  	(0x0A)
#define OUT_Z_H_M    						(0x0D)
#define OUT_Z_L_M  						  	(0x0C)

/*Magnetic Offset Registers; The value is expressed in 16-bit as two’s complement.   (R/W)*/
#define OFFSET_X_L_M    					(0x16)
#define OFFSET_X_H_M    					(0x17)
#define OFFSET_Y_L_M    					(0x18)
#define OFFSET_Y_H_M    					(0x19)
#define OFFSET_Z_L_M    					(0x1A)
#define OFFSET_Z_H_M    					(0x1B)

#define REFERENCE_X    						(0x1C)  //Reference value for high-pass filter for X-axis acceleration data.
#define REFERENCE_Y       					(0x1D)  //Reference value for high-pass filter for Y-axis acceleration data.
#define REFERENCE_Z       					(0x1E)  //Reference value for high-pass filter for Z-axis acceleration data.

/*Control Register for accelerometer and magnetometer.   (R/W)*/
#define CTRL0	    						(0x1F)	//Normal Mode, FIFO, High Pass Filter toggle for interrupts
#define CTRL1	    						(0x20)	//Accelerometer
#define CTRL2		    					(0x21)	//Accelerometer
#define CTRL3		    					(0x22)	//Accelerometer Data Ready Set Register
#define CTRL4		    					(0x23)	//Magnetometer Data Ready Set Register
#define CTRL5		    					(0x24)	//Magnetometer control register.
#define CTRL6		    					(0x25)	//Magnetic full-scale selection control register
#define CTRL7		    					(0x26)	//Accelerometer/Magnetometer control register



/*Unused Registers*/
#define FIFO_CTRL 							(0x2E)	
#define FIFO_SRC  							(0x2F)	
#define IG_CFG1   							(0x30)	//Inertial interrupt generator 1 configuration register.
#define IG_SRC1   							(0x31)	//Inertial interrupt generator 1 status register.
#define IG_THS1   							(0x32)
#define IG_DUR1   							(0x33)
#define IG_CFG2   							(0x34)
#define IG_SRC2   							(0x35)
#define IG_THS2   							(0x36)
#define IG_DUR2   							(0x37)
#define CLICK_CFG 							(0x38)
#define CLICK_SRC   						(0x39)
#define CLICK_THS 							(0x3A)
#define TIME_LIMIT 							(0x3B)
#define TIME_LATENCY						(0x3C)
#define TIME_WINDOW 						(0x3D)
#define ACT_THS 							(0x3E)
#define ACT_DUR								(0x3F)

//CTR1 CONFIGURATION VALUES
//ODR selection
#define ACC_POWER_DOWN			(0x00)
#define ACC_DATA_RATE_3_125Hz	(0x10)
#define ACC_DATA_RATE_6_25Hz	(0x20)
#define ACC_DATA_RATE_12_5Hz	(0x30)
#define ACC_DATA_RATE_25Hz		(0x40)
#define ACC_DATA_RATE_50Hz		(0x50)
#define ACC_DATA_RATE_100Hz		(0x60)
#define ACC_DATA_RATE_200Hz		(0x70)
#define ACC_DATA_RATE_400Hz		(0x80)
#define ACC_DATA_RATE_800Hz		(0x90)
#define ACC_DATA_RATE_1600Hz	(0xA0)

//ACC axis selection
#define ACC_X_EN				(0x01)
#define ACC_Y_EN				(0x02)
#define ACC_Z_EN				(0x04)
#define ACC_XYZ_EN				(0x07)


/*
 * CTR3 CONFIGURATION VALUES

INT1_BOOT Boot on INT1 enable. Default value: 0 (0: disable; 1: enable)
INT1_Click Click generator interrupt on INT1. Default value: 0(0: disable; 1: enable)
INT1_IG1 Inertial interrupt generator 1 on INT1. Default value: 0(0: disable; 1: enable)
INT1_IG2 Inertial interrupt generator 2 on INT1. Default value: 0(0: disable; 1: enable)
INT1_IGM Magnetic interrupt generator on INT1. Default value: 0(0: disable; 1: enable)
INT1_DRDY_A Accelerometer data-ready signal on INT1. Default value: 0(0: disable; 1: enable)
INT1_DRDY_M Magnetometer data-ready signal on INT1. Default value: 0(0: disable; 1: enable)
INT1_EMPTY FIFO empty indication on INT1. Default value: 0(0: disable; 1: enable)
*/

#define INT1_BOOT				(0x80)
#define INT1_Click				(0x40)
#define INT1_IG1				(0x20)
#define INT1_IG2				(0x10)
#define INT1_IGM				(0x08)
#define INT1_DRDY_A				(0x04)
#define INT1_DRDY_M				(0x02)
#define INT1_EMPTY				(0x01)

/*
 * CTR4 CONFIGURATION VALUES
 * 
INT2_Click Click generator interrupt on INT2. Default value: 0(0: disable; 1: enable)
INT2_IG1 Inertial interrupt generator 1 on INT2. Default value: 0(0: disable; 1: enable)
INT2_IG2 Inertial interrupt generator 2 on INT2. Default value: 0(0: disable; 1: enable)
INT2_IGM Magnetic interrupt generator on INT2. Default value: 0(0: disable; 1: enable)
INT2_DRDY_A Accelerometer data-ready signal on INT2. Default value: 0(0: disable; 1: enable)
INT2_DRDY_M Magnetometer data-ready signal on INT2. Default value: 0(0: disable; 1: enable)
INT2_Overrun FIFO overrun interrupt on INT2. Default value: 0(0: disable; 1: enable)
INT2_FTH FIFO threshold interrupt on INT2. Default value: 0(0: disable; 1: enable)
*/

#define INT2_Click				(0x80)
#define INT2_IG1				(0x40)
#define INT2_IG2				(0x20)
#define INT2_IGM				(0x10)
#define INT2_DRDY_A				(0x08)
#define INT2_DRDY_M				(0x04)
#define INT2_Overrun			(0x02)
#define INT2_FTH				(0x01)

/*
 * CTR5 CONFIGURATION VALUES
 * 
 TEMP_EN Temperature sensor enable. Default value: 0(0: temperature sensor disabled; 1: temperature sensor enabled)
M_RES [1:0] Magnetic resolution selection. Default value: 00(00: low resolution, 11: high resolution)
M_ODR [2:0] Magnetic data rate selection. Default value: 110
Refer to Table 47
LIR2 Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by reading INT2_SRC itself. Default value: 0.(0: interrupt request not latched; 1: interrupt request latched)
LIR1 Latch interrupt request on INT1_SRC register, with INT1_SRC register cleared by reading INT1_SRC itself. Default value: 0.(0: interrupt request not latched; 1: interrupt request latched)
  
*/

#define MAG_RES_LOW				(0x00)
#define MAG_RES_LOW_MED		    (0x20)
#define MAG_RES_MED_LOW			(0x40)
#define MAG_RES_HIGH			(0x60)

#define MAG_DATA_RATE_3_125Hz	(0x00)
#define MAG_DATA_RATE_6_25Hz	(0x04)
#define MAG_DATA_RATE_12_5Hz	(0x08)
#define MAG_DATA_RATE_25Hz		(0x0C)
#define MAG_DATA_RATE_50Hz		(0x10)
#define MAG_DATA_RATE_100Hz		(0x14)

/*
 * CTR6 CONFIGURATION VALUES
 *
 */
#define MAG_FULL_SCALE_2		(0x00)
#define MAG_FULL_SCALE_4		(0x20)
#define MAG_FULL_SCALE_8		(0x40)
#define MAG_FULL_SCALE_12		(0x60)


/*
 * CTR7 CONFIGURATION VALUES
AHPM[1:0] High-pass filter mode selection for acceleration data. Default value: 00Refer to Table 53
AFDS Filtered acceleration data selection. Default value: 0(0: internal filter bypassed; 1: data from internal filter sent to output register and FIFO)
T_ONLY Temperature sensor only mode. Default value: 0 If this bit is set to ‘1’, the temperature sensor is on while the magnetic sensor is off.
MLP Magnetic data low-power mode. Default value: 0 If this bit is ‘1’, the M_ODR [2:0] is set to 3.125 Hz independently from the MODR settings. Once the bit is set to ‘0’, the magnetic data rate is configured by the MODR bits in the CTRL5 (24h)register.
MD[1:0] Magnetic sensor mode selection. Default 10 Refer to Table 54
 */
 
#define MAG_MOD_CONTINUES		(0x00)
#define MAG_MOD_SINGLE			(0x01)
#define MAG_MOD_POWER_DOWN		(0x02)

#define LSM303D_ACC_PIN_INT     (GPIO_PORT_D | GPIO_PIN10)
#define LWGPIO_MUX_D10_GPIO  	(1)

#define LSM303D_MAG_PIN_INT     (GPIO_PORT_D | GPIO_PIN11)
#define LWGPIO_MUX_D11_GPIO  	(1)

#define M_PI   					3.14159265358979323846264338327950288

#endif

#define LSM_BUFFER_SIZE      (7)
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C0
#define ACCELEROMETER_GAIN    (1000)
#define MAGNETOMETR_XY_GAIN  (1100)
#define MAGNETOMETR_Z_GAIN   (980)
#define DEG_TO_RAD           (0.0174532925)
#define RAD_TO_DEG           (57.2957795)
#define LWGPIO_MUX_B9_GPIO   (1)
#define BSP_LSM_PIN          (GPIO_PORT_D|GPIO_PIN7)
#define LWGPIO_MUX_D7_GPIO   (1)
#define NUM_ACC_CAL_POS		 (2)

#define MAGMAXINT 1073741824
#define MAGRANGE  16384

typedef union
{
	int16_t Acc_Calib_Write_Buf[6][4];
	uint8_t Acc_Calib_char_Buff[48];
}ACC_CALIB;
extern ACC_CALIB Acc_Calib1;

//typedef struct
//{
//	double ACC11 , ACC21 , ACC31 , ACC12, ACC22 , ACC32 ,ACC13 , ACC23 , ACC33 ,ACC10 , ACC20  , ACC30 ;
//}ACC;

typedef struct
{
	double ACC00, ACC01, ACC02, ACC10 , ACC11 , ACC12 , ACC20 ,ACC21 , ACC22, ACC30, ACC31, ACC32 ;
}ACC;

typedef union
{
	ACC  data;
	uint8_t ACC_char [96];
	
}ACC_Values;

extern ACC_Values ACC_Data;

/* Global variables */
extern LWGPIO_STRUCT Lsm303_TMP_pin;
extern uchar_ptr Lsm303_buffer;
extern MQX_FILE_PTR Lsm303_fd;
extern uchar Lsm_commands[], Lsm_Data_Ready, lsm_data_count;
extern vector a_avg, m_avg, a, m;
extern int_16 Slope_Measurment_Result_Slope_Angle;
extern char Slope_Measurment_Result_Aspect[];
extern volatile uchar Test_Complete_flag;
extern int_16 Ax_raw, Ay_raw, Az_raw, Mx, My, Mz;
/*Function prototype*/
void Start_LSM(void);
extern void Get_Lsm_Data();
extern uint8_t Read_Acc_Calib_values_from_Sd_Card();
extern uint_8 lsm303_i2c0_init(void);
extern void collect_accelerometer_data(uint8_t position);
uint_8 Lsm303_Test_Init(void);
extern uint_8 Lsm303_deinit(void);
extern uint_8 get_slope_measurement(int_16 *slope_angle, char *aspect, uint_16 *magnetic_heading);
void get_euler_angles(float *roll, float *pitch);

extern void Stop_LSM(void);
void read_accelerometer_data(void);
void read_magnetometer_data(void);
extern void calibrate_magnetometer(void);
extern void update_magnetomer_calib_data(void);
extern void reset_calibration_values(void);
extern void Sample_average_LSM_data(void);
extern void basicmagcalib(int *cBx, int *cBy, int *cBz);
extern void fusion6(float Gx, float Gy, float Gz, int Bx, int By, int Bz, float *Roll, float *Pitch, float *Yaw);
extern void Get_Sample_average_LSM_results(int_16 *, char *, uint_16 *);

extern int Bx,By,Bz, final_yaw, final_roll;
extern float Roll,Pitch,Yaw;
extern float Gx, Gy, Gz;
extern int_16 Ax_raw, Ay_raw, Az_raw;
extern float Ax, Ay, Az;
extern float acc_cal_samples[600][3];
extern int acc_count;


extern uint8_t calib_count;
extern uint8_t Magnetometer_Calib_process;

#endif
/* EOF */
