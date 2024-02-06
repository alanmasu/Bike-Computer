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


void MPU6050_init(void);
int MPU6050_readDeviceId(void);
double MPU6050_readXvalue(void);
double MPU6050_readYvalue(void);
double MPU6050_readZvalue(void);
double MPU6050_readTemp_chip(void);

#endif
