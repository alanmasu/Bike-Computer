/*!
    @file       photoresistor.h
    @brief      Photoresistor functions definition.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

#ifndef __PHOTORESISTOR_H__
#define __PHOTORESISTOR_H__ 

/*DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define LIGHT_BUFFER_LENGTH 4
#define MAX_LIGHT_SAMPLES   60
 
/*!
    @defgroup   Photoresistor_Module Photoresistor
    @name       Photoresistor Module
    @{
*/

extern volatile uint8_t resultPos;
extern volatile bool photoresFlag;
extern volatile uint_fast16_t resultsBuffer[LIGHT_BUFFER_LENGTH];
extern volatile uint_fast16_t lightToSend[MAX_LIGHT_SAMPLES];
extern volatile uint8_t resultPos;
extern volatile uint8_t sendPos;

uint_fast16_t* getResultBuffer();

float photoresistorConverter(uint_fast16_t sampledValue);

void ADC14Init(const Timer_A_UpModeConfig* upModeConfig, 
                    const Timer_A_CompareModeConfig* compareConfig);



/*
    @}
*/


#endif
 
