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
volatile Page_t myPage=PAGE_1;
toShowPage1 myParamStruct;

void graphicsInit(eUSCI_SPI_MasterConfig* config)
{
    /* Initializes display */
    Crystalfontz128x128_Init(config);
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

void showPage1(toShowPage1* paramToShow1)
{
    char tmpString[40] = "/0";

    // time-altitude-satellites-temperature
    snprintf(tmpString, 39, "Time:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 2, 1);
    snprintf(tmpString, 39, "%s", paramToShow1->time);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);

    snprintf(tmpString, 39, "Altitude:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 27, 1);
    snprintf(tmpString, 39, "%.2f m", paramToShow1->altitude);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 35, 1);

    snprintf(tmpString, 39, "Sats:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 52, 1);
    snprintf(tmpString, 39, "%.2f", paramToShow1->sats);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 60, 1);

    snprintf(tmpString, 39, "Temp:");
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 77, 1);
    snprintf(tmpString, 39, "%.2f C", paramToShow1->temp);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 85, 1);

    // speed
    snprintf(tmpString, 39, "Speed:");
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 42, 1);
    snprintf(tmpString, 39, "%.2f km/h", paramToShow1->speed);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 55, 1);

    // trip time
    snprintf(tmpString, 39, "Trip Time (h:m:s):");
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 64, 110, 1);
    snprintf(tmpString, 39, "%s", paramToShow1->tripTime);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 64, 118, 1);
}
void scrollPages()
{
    switch (myPage)
    {
    case PAGE_1:
        drawGrid1();
        showPage1(&myParamStruct);
        int tmp=MAP_ADC14_getResult(ADC_MEM1);
        if (MAP_ADC14_getResult(ADC_MEM1)>16000)
        {
            GrStringDraw(&g_sContext, "Page2", -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);

        }
        
        break;
    case PAGE_2:
        //
        break;
    case PAGE_3:
        //
        break;
    }
}
