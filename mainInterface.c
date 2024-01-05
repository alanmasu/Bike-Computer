/*
 * mainInterface.c
 *
 *  Created on: 26 nov 2023
 *      Author: Federica_Lorenzini
 */
#include "mainInterface.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>

Graphics_Context g_sContext;
tRectangle temp={0,0,128,17};
tRectangle avSpeed={0,17,128,34};
tRectangle instSpeed={0,34,128,51};
tRectangle acc={0,51,128,68};
tRectangle distance={0,68,128,85};
tRectangle coord={0,85,128,102};
tRectangle menu={0,102,128,128};


void graphicsInit(){
    /* Initializes display */
    Crystalfontz128x128_Init();
    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,&g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}
void drawGrid(){
    GrRectDraw(&g_sContext, &temp);
    GrRectDraw(&g_sContext, &avSpeed);
    GrRectDraw(&g_sContext, &instSpeed);
    GrRectDraw(&g_sContext, &acc);
    GrRectDraw(&g_sContext, &distance);
    GrRectDraw(&g_sContext, &coord);
    GrRectDraw(&g_sContext, &menu);
}
char* StringTemp(float temp){
    char* toPrint = (char*)malloc(10);
    sprintf(toPrint, "%3.2f C",temp);
    return toPrint;
}




