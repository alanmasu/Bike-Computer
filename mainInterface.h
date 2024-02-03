/*!
    @file   mainInterface.c
    @brief  Initializing main functions for LCD screen and drawing grid
    @date   26/11/2023
    @author Federica Lorenzini
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
typedef enum
{
    PAGE_1,
    PAGE_2,
    PAGE_3,
} Page_t;
extern Page_t myPage;

typedef struct toShowPage1
{
    float distance;
    float temp;
    char time[10];
    float altitude;
    int sats;
    float speed;
    char tripTime[10];
} toShowPage1;
extern toShowPage1 myParamStruct;

typedef struct toShowPage2
{
    float hdop;
    float vdop;
    float speed;
    char fixType[6];
} toShowPage2;
extern toShowPage2 myParamStruct2;

void graphicsInit(eUSCI_SPI_MasterConfig *);
void graphicsInitSelected(eUSCI_SPI_MasterConfig *);
void graphicsInitBigFont(eUSCI_SPI_MasterConfig *);
void drawGrid1();
void drawGrid2();
void showPage1(toShowPage1 *paramToShow1);
void showPage2(toShowPage2 *paramToShow2);
void showPage3();
void scrollPages();
void showPages();
float getWheel();
void setWheel(float wheelDim);
#endif /* MAININTERFACE_H_ */
