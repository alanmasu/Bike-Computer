/*
 * mainInterface.h
 *
 *  Created on: 26 nov 2023
 *      Author: Federica_Lorenzini
 */

#ifndef MAININTERFACE_H_
#define MAININTERFACE_H_

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>
extern Graphics_Context g_sContext;
extern tRectangle temp;
extern tRectangle avSpeed;
extern tRectangle instSpeed;
extern tRectangle acc;
extern tRectangle distance;
extern tRectangle coord;
extern tRectangle menu;

void drawGrid();
void graphicsInit();
char* StringTemp(float temp);

#endif /* MAININTERFACE_H_ */




