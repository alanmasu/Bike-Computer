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
extern tRectangle mutipleData;
extern tRectangle instSpeed;
extern tRectangle tripTime;
/*State machine to implement page scrolling*/
typedef enum{
    PAGE_1,
    PAGE_2,
    PAGE_3,
}Page_t;
extern volatile Page_t myPage;

typedef struct toShowPage1{
    float temp;
    char time[10];
    float altitude;
    int sats;
    float speed;
    char tripTime[10];
}toShowPage1;
extern toShowPage1 myParamStruct;

void scrollPages();
void drawGrid1();
void graphicsInit(eUSCI_SPI_MasterConfig*);
void showPage1(toShowPage1* paramToShow1);

#endif /* MAININTERFACE_H_ */




