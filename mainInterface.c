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
tRectangle multipleData = {0, 0, 64, 102};
tRectangle instSpeed = {64, 0, 128, 102};
tRectangle tripTime = {0, 102, 128, 128};

void graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();
    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}
void drawGrid1()
{
    GrRectDraw(&g_sContext, &multipleData);
    GrRectDraw(&g_sContext, &instSpeed);
    GrRectDraw(&g_sContext, &tripTime);
}

void showPage1()
{
    char tmpString[40] = "/0";

    // time-altitude-satellites-temperature
    snprintf(tmpString, 39, "Time:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 2, 0);
    snprintf(tmpString, 39, "%.2f", 13.15);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 0);

    snprintf(tmpString, 39, "Altitude:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 27, 0);
    snprintf(tmpString, 39, "%.2f m", 600.00);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 35, 0);

    snprintf(tmpString, 39, "Sats:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 52, 0);
    snprintf(tmpString, 39, "%.2f", 4.00);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 60, 0);

    snprintf(tmpString, 39, "Temp:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 77, 0);
    snprintf(tmpString, 39, "%.2f C", 22.5);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 85, 0);

    // speed
    snprintf(tmpString, 39, "Speed:");
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 42, 0);
    snprintf(tmpString, 39, "%.2f km/h", 30.5);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 55, 0);

    // trip time
    snprintf(tmpString, 39, "Trip Time (h:m:s):");
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 64, 110, 0);
    snprintf(tmpString, 39, "%2d:%2d:%2d", 2, 45, 50);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 64, 118, 0);
}
void fn_PAGE_1(void)
{
    current_page=PAGE_2;
}
void fn_PAGE_2(void)
{
     current_page=PAGE_3;
}
void fn_PAGE_3(void)
{
     current_page=PAGE_1;
}
void scrollPages()
{
    switch (myPage)
    {
    case PAGE_1:
        //
        break;
    case PAGE_2:
        //
        break;
    case PAGE_3:
        //
        break;
    }
}
