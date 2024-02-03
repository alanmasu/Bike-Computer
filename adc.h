/*!
  @file     adc.h
  @brief    Header file for ADC initialization
  @date     01/02/2024
  @author   Federica Lorenzini
 */

#ifndef ADC_H_
#define ADC_H_
#include "mainInterface.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>

/*!
    @defgroup ADC_module ADC
    @{ 
*/

/*!
   @brief Inizialitation of ADC module.
*/
void adcInit();

#endif /* ADC_H_ */
