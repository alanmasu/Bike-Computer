#ifndef __SPEED_H__
#define __SPEED_H__

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

extern volatile bool isrFlag;

void setWheelDiameter(float userDiameter);
float speedCompute(uint_fast16_t capturedValue);
void timerInit();
uint_fast16_t getTimerAcapturedValue();



#endif
