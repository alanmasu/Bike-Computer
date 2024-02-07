/*!
    @file   HAL_I2C.h
    @brief  Initializing main functions for I2C communication between accelerometer and odometer
    @date   11/11/2023
    @author Alberto Dal Bosco
*/

#ifndef __HAL_I2C_H_
#define __HAL_I2C_H_


/*!
    @defgroup I2C_module I2C
    @{
*/

/*!
    @brief Set pin on GPIO for I2C communication
*/
void Init_I2C_GPIO(void);

/*!
    @brief Initializate master to communicate with I2C
*/
void I2C_init(void);

/*!
    @brief I2C read of 16 bits
    @param[in] reg_to_read: register to read in accelerometer
    @param[out] value: value of 16 bit read from the specified register
*/
int16_t I2C_read16(unsigned char);

/*!
    @brief I2C read of 8 bits
    @param[in] reg_to_read: register to read in accelerometer
    @param[out] value: value of 8 bit read from the specified register
*/
int8_t I2C_read8(unsigned char);

/*!
    @brief I2C write 16 bits
    @param[in] pointer: register to write in accelerometer
    @param[in] byte_to_write: value to write in specified register
*/
void I2C_write16(unsigned char pointer, unsigned int writeByte);

/*!
    @brief I2C write 8 bits
    @param[in] pointer: register to write in accelerometer
    @param[in] byte_to_write: value to write in specified register
*/
void I2C_write8 (unsigned char pointer, unsigned int writeByte);

/*!
    @brief I2C set slave address
    @param[in] addr_of_slave: register to write in accelerometer
*/
void I2C_setslave(unsigned int slaveAdr);


/*
    @}
*/
#endif
