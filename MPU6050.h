/*!
    @file   MPU6050.h
    @brief  Initializing main functions for MPU5060 module and functions for reading ID and accelerations from sensor 
    @date   12/12/2023
    @author Alberto Dal Bosco
*/

#ifndef __MPU6050_H_
#define __MPU6050_H_

/* MPU6050 COSTANTS */
#define MPU6050_SLAVE_ADDR              0x68
#define I2C_SCL                         BIT7
#define I2C_SDA                         BIT6

/* MPU6050 SENSOR REGISTER DEFINITIONS */
#define PWR_MGMT_1                      0x6B
#define MPU6050_ACCEL_CONFIG_REG        0x1C
#define WHO_AM_I_REGISTER               0x75           // Who am I register --> Contains the 6-bit I2C address of the MPU-60X0.
#define ACCEL_XOUT_MS_REG               0x3B
#define ACCEL_XOUT_LS_REG               0x3C
#define ACCEL_YOUT_MS_REG               0x3D
#define ACCEL_YOUT_LS_REG               0x3E
#define ACCEL_ZOUT_MS_REG               0x3F
#define ACCEL_ZOUT_LS_REG               0x40
#define TEMP_OUT_MS_REG                 0x41
#define TEMP_OUT_LS_REG                 0x42

/* CONFIGURATION REGISTER SETTINGS */
#define MPU6050_DEVICE_RESET            0x80           // bit 7 of PWR_MGMT register, when set to 1, resets all internal registers to their default values.
#define MPU6050_INIT_VALUE              0x00
#define MPU6050_AFS_SEL_REG             0x10


/*!
    @defgroup MPU6050_module MPU6050
    @{
*/

/*!
    @brief Init of accelerometer sensor
    @details Initialization of MPU6050_ACCEL_CONFIG_REG register with MPU6050_DEVICE_RESET and MPU6050_INIT_VALUE value.
*/
void MPU6050_init(void);

/*!
    @brief Read accelerometer id, just for test.
    @details Read WHO_AM_I_REGISTER register to verify its id in I2C communication.
    @param[out] id: id of accelerometer.
*/
int MPU6050_readDeviceId(void);

/*!
    @brief Read acceleration along x axis.
    @details Read ACCEL_XOUT_MS_REG register and ACCEL_XOUT_LS_REG and concatenate it to obtain 12 bit acceleration.
    @param[out] x_acc: x acceleration.
*/
double MPU6050_readXvalue(void);

/*!
    @brief Read acceleration along y axis.
    @details Read ACCEL_YOUT_MS_REG register and ACCEL_YOUT_LS_REG and concatenate it to obtain 12 bit acceleration.
    @param[out] y_acc: y acceleration.
*/
double MPU6050_readYvalue(void);

/*!
    @brief Read acceleration along z axis.
    @details Read ACCEL_ZOUT_MS_REG register and ACCEL_ZOUT_LS_REG and concatenate it to obtain 12 bit acceleration.
    @param[out] z_acc: z acceleration.
*/
double MPU6050_readZvalue(void);

/*!
    @brief Read temperature of the chip.
    @details Read TEMP_OUT_MS_REG register and TEMP_OUT_LS_REG and concatenate it to obtain 12 bit temperature.
    @param[out] temp: temperature of the accelerometer chip.
*/
double MPU6050_readTemp_chip(void);

/*
    @}
*/

#endif
