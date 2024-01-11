#ifndef __SD_DRIVER_H__
#define __SD_DRIVER_H__

#include "GPIO_Driver.h"
#include "SPI_Driver.h"
#include "TIMERA_Driver.h"
#include "UART_Driver.h"

#define MMC_SS_GPIO_PORT  GPIO_PORT_P5
#define MMC_SS_GPIO_PIN   GPIO_PIN2

void SD_Init(void);

#endif // __SD_DRIVER_H__
