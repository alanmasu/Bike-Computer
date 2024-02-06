#ifndef __HAL_I2C_H_
#define __HAL_I2C_H_


void Init_I2C_GPIO(void);
void I2C_init(void);
int16_t I2C_read16(unsigned char);
int8_t I2C_read8(unsigned char);
void I2C_write16(unsigned char pointer, unsigned int writeByte);
void I2C_write8 (unsigned char pointer, unsigned int writeByte);
void I2C_setslave(unsigned int slaveAdr);

#endif
