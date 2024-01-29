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
/*State machine to implement page scrolling

typedef enum{
    PAGE_1,
    PAGE_2,
    PAGE_3,
}Page_t;

typedef struct{
    Page_t page;
    void (*page_function)(void);
} StateMachine_t;

Page_t current_page = PAGE_1;

StateMachine_t fsm[] = {
                      {PAGE_1, fn_PAGE_1},
                      {PAGE_2, fn_PAGE_2},
                      {PAGE_3, fn_PAGE_3}
};
void fn_PAGE_1(void);
void fn_PAGE_2(void);
void fn_PAGE_3(void);*/

void scrollPages();
void drawGrid1();
void graphicsInit(eUSCI_SPI_MasterConfig*);
void showPage1(float temp);

#endif /* MAININTERFACE_H_ */




