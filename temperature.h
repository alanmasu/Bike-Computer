/*!
  @file     temperature.h
  @brief    Header file for temperature sensor initialization
  @date     01/02/2024
  @author   Federica Lorenzini
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>

/*!
    @defgroup Temp_module Temperature
    @{ 
*/

extern volatile uint32_t cal30;
extern volatile uint32_t cal85;
extern volatile float calDifference;
extern volatile float tempC;
extern volatile float tempF;

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>

/*!
   @brief Inizialitation of temperature sensor and global variables related to it.
*/
void temperatureInit();

#endif /* TEMPERATURE_H_ */
