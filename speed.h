/*!
    @file       speed.h
    @brief      Speed functions definition.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

#ifndef __SPEED_H__
#define __SPEED_H__

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*!
    @defgroup   speed_Module Speed
    @name       Speed Module
    @{
*/
extern volatile bool isrFlag;

void setWheelDiameter(float userDiameter);
float speedCompute(uint_fast16_t capturedValue);
void timerInit(const Timer_A_ContinuousModeConfig* continuousModeConfig, 
                       const Timer_A_CaptureModeConfig* captureModeConfig);
uint_fast16_t getTimerAcapturedValue();

/*
    @}
*/

#endif
