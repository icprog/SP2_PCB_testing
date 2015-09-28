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

#define ENABLE_LSM303DLHC						(0)
#define ENABLE_LSM303D							(1)
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

#else ENABLE_LSM303D
	#define I2C_ACCELEROMETER_CONTROL_REG1    (0x20)     /* ACCELEROMETER CONTROL REGISTER 1*/
	#define I2C_ACCELEROMETER_CONTROL_REG4    (0x23)     /* ACCELEROMETER CONTROL REGISTER 4*/
	#define I2C_MAGNETOMETER_CONTROL_REG1     (0x00)     /* MAGNETOMETER CONTROL REGISTER 1*/
	#define I2C_MAGNETOMETER_CONTROL_REG2     (0x01)     /* MAGNETOMETER CONTROL REGISTER 2*/
	#define I2C_MAGNETOMETER_CONTROL_REG3     (0x02)     /* MAGNETOMETER CONTROL REGISTER 3*/
	
	#define I2C_ACCELEROMETER_BUS_ADDRESS     (0x1E)     /* I2C bus address of ACCELEROMETER */
	#define I2C_MAGNETOMETER_BUS_ADDRESS      (0x1E)     /* I2C bus address of MAGNETOMETER */
	
	#define LSM303D_DEVICE_ADDRESS (0x1E)	//If SA0 is high, 1D else if SA0 is grounded, 1E
	
	#define WHO_AM_I   (0x0F)	//Device Identification Register; Default Value: 01001001
	#define STATUS_M   (0x07)   //Magnetic X,Y,Z axis data overrun indication status bit; may be required.
	#define STATUS_A   (0x27)	//Acceletometer X,Y,Z axis data overrun status register.
	
	/*Interrupt Registers */
	#define INT_CTRL_M  (0x12)	//Magnetometer Interrupt Control Register; Default Value: 11101000    (R/W)
	#define INT_SRC_M   (0x13)  //Latched Interrupt Requests for mag; interrupts clear upon reading.  (R)
	
	/*output registers of accelerometer; The value is expressed in 16-bit as two’s complement.*/
	#define OUT_X_L_A (0x28)
	#define OUT_X_H_A (0x29)
	#define OUT_Y_L_A (0x2A)
	#define OUT_Y_H_A (0x2B)
	#define OUT_Z_L_A (0x2C)
	#define OUT_Z_H_A (0x2D)
	
	/*output registers of magnetic sensor; The value is expressed in 16-bit as two’s complement.*/
	#define OUT_X_H_M  (0x09)
	#define OUT_X_L_M  (0x08)
	#define OUT_Y_H_M  (0x0B)
	#define OUT_Y_L_M  (0x0A)
	#define OUT_Z_H_M  (0x0D)
	#define OUT_Z_L_M  (0x0C)
	
	/*Magnetic Offset Registers; The value is expressed in 16-bit as two’s complement.   (R/W)*/
	#define OFFSET_X_L_M  (0x16)
	#define OFFSET_X_H_M  (0x17)
	#define OFFSET_Y_L_M  (0x18)
	#define OFFSET_Y_H_M  (0x19)
	#define OFFSET_Z_L_M  (0x1A)
	#define OFFSET_Z_H_M  (0x1B)
	
	#define REFERENCE_X   (0x1C)  //Reference value for high-pass filter for X-axis acceleration data.
	#define REFERENCE_Y   (0x1D)  //Reference value for high-pass filter for Y-axis acceleration data.
	#define REFERENCE_Z   (0x1E)  //Reference value for high-pass filter for Z-axis acceleration data.
	
	/*Control Register for accelerometer and magnetometer.   (R/W)*/
	#define CTRL0	(0x1F)	//Normal Mode, FIFO, High Pass Filter toggle for interrupts
	#define CTRL1	(0x20)	//Accelerometer
	#define CTRL2	(0x21)	//Accelerometer
	#define CTRL3	(0x22)	//Accelerometer Data Ready Set Register
	#define CTRL4	(0x23)	//Magnetometer Data Ready Set Register
	#define CTRL5	(0x24)	//Magnetometer control register.
	#define CTRL6	(0x25)	//Magnetic full-scale selection control register
	#define CTRL7	(0x26)	//Accelerometer/Magnetometer control register
	
	
	
	/*Unused Registers*/
	#define FIFO_CTRL 		(0x2E)	
	#define FIFO_SRC  		(0x2F)	
	#define IG_CFG1   		(0x30)	//Inertial interrupt generator 1 configuration register.
	#define IG_SRC1   		(0x31)	//Inertial interrupt generator 1 status register.
	#define IG_THS1   		(0x32)
	#define IG_DUR1   		(0x33)
	#define IG_CFG2   		(0x34)
	#define IG_SRC2   		(0x35)
	#define IG_THS2   		(0x36)
	#define IG_DUR2   		(0x37)
	#define CLICK_CFG 		(0x38)
	#define CLICK_SRC   	(0x39)
	#define CLICK_THS 		(0x3A)
	#define TIME_LIMIT 		(0x3B)
	#define TIME_LATENCY	(0x3C)
	#define TIME_WINDOW 	(0x3D)
	#define ACT_THS 		(0x3E)
	#define ACT_DUR			(0x3F)

#endif


#define LSM_BUFFER_SIZE      (7)
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C0
#define ACCELROMETER_GAIN    (1000)
#define MAGNETOMETR_XY_GAIN  (1100)
#define MAGNETOMETR_Z_GAIN   (980)
#define DEG_TO_RAD           (0.0174532925)
#define RAD_TO_DEG           (57.2957795)
#define LWGPIO_MUX_B9_GPIO   (1)
#define BSP_LSM_PIN          (GPIO_PORT_D|GPIO_PIN7)
#define LWGPIO_MUX_D7_GPIO   (1)

/* Global variables */
extern LWGPIO_STRUCT Lsm303_TMP_pin;
extern uchar_ptr Lsm303_buffer;
extern MQX_FILE_PTR Lsm303_fd;
extern uchar Lsm_commands[], Lsm_Data_Ready, lsm_data_count;
extern vector a_avg, m_avg, a, m;
extern int_16 Slope_Measurment_Result_Slope_Angle;
extern char Slope_Measurment_Result_Aspect[];
extern volatile uchar Test_Complete_flag;
extern int_16 Ax, Ay, Az, Mx, My, Mz;
/*Function prototype*/
extern void Start_LSM(void);
extern void Get_Lsm_Data();
extern uint_8 lsm303_i2c0_init(void);
uint_8 Lsm303_Test_Init(void);
extern uint_8 Lsm303_deinit(void);
extern void get_slope_measurement(int_16 *slope_angle, char *aspect, uint_16 *magnetic_heading);
extern void Stop_LSM(void);
void read_accelerometer_data(void);
void read_magnetometer_data(void);
#endif
/* EOF */
