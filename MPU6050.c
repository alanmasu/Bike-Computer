#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "MPU6050.h"
#include "HAL_I2C.h"
#include "math.h"


void MPU6050_init(void)
{
    volatile int i;

    I2C_setslave(MPU6050_SLAVE_ADDR);                   // Specify slave address for I2C

    // Initialize MPU6050 sensor
    I2C_setSlaveAddress (EUSCI_B1_BASE, MPU6050_SLAVE_ADDR);    // Specify slave address for I2C
    I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);          // Enable and clear the interrupt flag

    // Reset MPU6050, in particular DEVICE_RESET bit to 1 --> clear all the internal registers
    //I2C_write8(PWR_MGMT_1, MPU6050_DEVICE_RESET);

    //for (i=10000; i>0;i--);

    I2C_write8(PWR_MGMT_1, MPU6050_INIT_VALUE);        // Internal 8MHz oscillator, enable sensor, no sleep mode.

    for (i=10000; i>0;i--);

    // 0x1C is the ACCEL_CONFIG Register of MPU6050, AFS_SEL[1:0] bit 3,4 of 8-bit register: 0-->2g; 1-->4g; 2-->8g; 3-->16g;
    I2C_write8(MPU6050_ACCEL_CONFIG_REG, MPU6050_AFS_SEL_REG);

    for (i=10000; i>0;i--);
}


int MPU6050_readDeviceId(void)
{
    I2C_setslave(MPU6050_SLAVE_ADDR);           // Specify slave address for MPU6050
    return I2C_read8(WHO_AM_I_REGISTER);        // The Power-On-Reset value of Bit6:Bit1 is 110 100.   N.B.: add a 0 in the bit 0 to obtain 1101000 that is equal to MPU6050_SLAVE_ADDR = 0x68 in hexadecimal.
}


double MPU6050_readXvalue(void)
{
    int8_t x_value_15_8;
    int8_t x_value_7_0;
    I2C_setslave(MPU6050_SLAVE_ADDR);           // Specify slave address for MPU6050
    x_value_15_8 = I2C_read8(ACCEL_XOUT_MS_REG);
    x_value_7_0 = I2C_read8(ACCEL_XOUT_LS_REG);
    return (double)(((int16_t)((x_value_15_8 << 8) | x_value_7_0 )) / 4096.0);
}

double MPU6050_readYvalue(void)
{
    int8_t y_value_15_8;
    int8_t y_value_7_0;
    I2C_setslave(MPU6050_SLAVE_ADDR);           // Specify slave address for MPU6050
    y_value_15_8 = I2C_read8(ACCEL_YOUT_MS_REG);
    y_value_7_0 = I2C_read8(ACCEL_YOUT_LS_REG);
    return (double)(((int16_t)((y_value_15_8 << 8) | y_value_7_0 )) / 4096.0);
}

double MPU6050_readZvalue(void)
{
    int8_t z_value_15_8;
    int8_t z_value_7_0;
    I2C_setslave(MPU6050_SLAVE_ADDR);           // Specify slave address for MPU6050
    z_value_15_8 = I2C_read8(ACCEL_ZOUT_MS_REG);
    z_value_7_0 = I2C_read8(ACCEL_ZOUT_LS_REG);
    return (double)(((int16_t)((z_value_15_8 << 8) | z_value_7_0 )) / 4096.0);
}

double MPU6050_readTemp_chip(void)
{
    int8_t temp_value_15_8;
    int8_t temp_value_7_0;
    I2C_setslave(MPU6050_SLAVE_ADDR);           // Specify slave address for MPU6050
    temp_value_15_8 = I2C_read8(TEMP_OUT_MS_REG);
    temp_value_7_0 = I2C_read8(TEMP_OUT_LS_REG);
    return (double) (signed)((temp_value_15_8 << 8) | temp_value_7_0 ) / 340.0 + 36.53;
}
