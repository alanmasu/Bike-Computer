/*
 * temperatureADC.h
 *
 *  Created on: 26 nov 2023
 *      Author: Federica_Lorenzini
 */

#ifndef TEMPERATUREADC_H_
#define TEMPERATUREADC_H_
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>

extern uint32_t cal30;
extern uint32_t cal85;
extern float calDifference;
extern float tempC;
extern float tempF;

extern void hwInit();
void ADC14_IRQHandler(void);
void getTemp();




#endif /* TEMPERATUREADC_H_ */
