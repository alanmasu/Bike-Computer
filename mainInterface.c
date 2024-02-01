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
tRectangle wheelDim = {70, 80, 110, 95};
tRectangle dimChoice = {70, 75, 110, 128};
Page_t myPage = PAGE_1;
toShowPage1 myParamStruct;
toShowPage2 myParamStruct2;
int XaxisPrev = 0;
int YaxisPrev = 0;

void graphicsInit(eUSCI_SPI_MasterConfig *config)
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
    GrStringDraw(&g_sContext, (int8_t *)"Time:", -1, multipleData.xMin + 7, multipleData.yMin + 2, 1);
    GrStringDraw(&g_sContext, (int8_t *)"Altitude:", -1, multipleData.xMin + 7, multipleData.yMin + 27, 1);
    GrStringDraw(&g_sContext, (int8_t *)"Sats:", -1, multipleData.xMin + 7, multipleData.yMin + 52, 1);
    GrStringDraw(&g_sContext, (int8_t *)"Temp:", -1, multipleData.xMin + 7, multipleData.yMin + 77, 1);
    GrRectDraw(&g_sContext, &instSpeed);
    GrStringDrawCentered(&g_sContext, (int8_t *)"Speed:", -1, 96, 42, 1);
    GrRectDraw(&g_sContext, &tripTime);
    GrStringDrawCentered(&g_sContext, (int8_t *)"Trip Time (h:m:s):", -1, 64, 110, 1);
}

void showPage1(toShowPage1 *paramToShow1)
{
    char tmpString[40] = "/0";

    // time-altitude-satellites-temperature
    snprintf(tmpString, 39, "%s", paramToShow1->time);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);

    snprintf(tmpString, 39, "%.2f m", paramToShow1->altitude);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 35, 1);

    snprintf(tmpString, 39, "%.2f", paramToShow1->sats);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 60, 1);

    snprintf(tmpString, 39, "%.2f C", paramToShow1->temp);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 85, 1);

    // speed
    snprintf(tmpString, 39, "%.2f km/h", paramToShow1->speed);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 55, 1);

    // trip time
    snprintf(tmpString, 39, "%s", paramToShow1->tripTime);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 64, 118, 1);
}
void drawGrid2()
{
    GrStringDrawCentered(&g_sContext, (int8_t *)"GPS VALUES", -1, 64, 10, 1);
    GrStringDrawCentered(&g_sContext, (int8_t *)"HDOP:", -1, 44, 40, 1);
    GrStringDrawCentered(&g_sContext, (int8_t *)"VDOP:", -1, 44, 60, 1);
    GrStringDrawCentered(&g_sContext, (int8_t *)"SPEED:", -1, 44, 80, 1);
    GrStringDrawCentered(&g_sContext, (int8_t *)"FIX TYPE:", -1, 44, 100, 1);
}
void showPage2(toShowPage2 *paramToShow2)
{
    char tmpString[40] = "/0";

    snprintf(tmpString, 39, "%.2f", paramToShow2->hdop);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 80, 40, 1);

    snprintf(tmpString, 39, "%.2f", paramToShow2->vdop);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 80, 60, 1);

    snprintf(tmpString, 39, "%.2f", paramToShow2->speed);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 80, 80, 1);

    snprintf(tmpString, 39, "%s", paramToShow2->fixType);
    GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 80, 100, 1);
}
void showPage3()
{
    GrStringDrawCentered(&g_sContext, (int8_t *)"MENU", -1, 64, 10, 1);

    GrStringDraw(&g_sContext, (int8_t *)"SPACE", -1, 3, 20, 1);
    GrStringDraw(&g_sContext, (int8_t *)"m", -1, 45, 20, 1);
    GrStringDraw(&g_sContext, (int8_t *)"km", -1, 75, 20, 1);
    GrStringDraw(&g_sContext, (int8_t *)"mi", -1, 100, 20, 1);

    GrStringDraw(&g_sContext, (int8_t *)"SPEED", -1, 3, 40, 1);
    GrStringDraw(&g_sContext, (int8_t *)"m/s", -1, 45, 40, 1);
    GrStringDraw(&g_sContext, (int8_t *)"km/h", -1, 70, 40, 1);
    GrStringDraw(&g_sContext, (int8_t *)"mi/h", -1, 100, 40, 1);

    GrStringDraw(&g_sContext, (int8_t *)"TEMP", -1, 3, 60, 1);
    GrStringDraw(&g_sContext, (int8_t *)"C", -1, 45, 60, 1);
    GrStringDraw(&g_sContext, (int8_t *)"F", -1, 90, 60, 1);

    GrStringDraw(&g_sContext, (int8_t *)"Wheel", -1, 3, 80, 1);
    GrStringDraw(&g_sContext, (int8_t *)"Dimension", -1, 3, 88, 1);
    GrRectDraw(&g_sContext, &wheelDim);
    // GrRectDraw(&g_sContext, &dimChoice);
    // GrLineDrawH(&g_sContext, 70, 110, 84);
    // GrLineDrawH(&g_sContext, 70, 110, 93);
    // GrLineDrawH(&g_sContext, 70, 110, 102);
    // GrLineDrawH(&g_sContext, 70, 110, 111);
    // GrLineDrawH(&g_sContext, 70, 110, 120);
    int diff = abs(MAP_ADC14_getResult(ADC_MEM2) - YaxisPrev);

    if (MAP_ADC14_getResult(ADC_MEM2) > 1400 && diff > 3000)
    {
    }
}
void scrollPages()
{
    int Xaxis = MAP_ADC14_getResult(ADC_MEM1);
    int diff = abs(MAP_ADC14_getResult(ADC_MEM1) - XaxisPrev);
    switch (myPage)
    {
    case PAGE_1:
        drawGrid1();
        if (Xaxis > 14000 && diff > 3000)
        {
            myPage = PAGE_2;
            Graphics_clearDisplay(&g_sContext);
        }
        if (Xaxis < 500 && diff > 3000)
        {
            myPage = PAGE_3;
            Graphics_clearDisplay(&g_sContext);
        }
        break;
    case PAGE_2:
        drawGrid2();
        if (Xaxis > 14000 && diff > 3000)
        {
            myPage = PAGE_3;
            Graphics_clearDisplay(&g_sContext);
        }
        if (Xaxis < 500 && diff > 3000)
        {
            myPage = PAGE_1;
            Graphics_clearDisplay(&g_sContext);
        }
        break;
    case PAGE_3:
        if (Xaxis > 14000 && diff > 3000)
        {
            myPage = PAGE_1;
            Graphics_clearDisplay(&g_sContext);
        }
        if (Xaxis < 500 && diff > 3000)
        {
            myPage = PAGE_2;
            Graphics_clearDisplay(&g_sContext);
        }
        break;
    }
    XaxisPrev = Xaxis;
}
void showPages()
{
    switch (myPage)
    {
    case PAGE_1:
        showPage1(&myParamStruct);
        break;
    case PAGE_2:
        showPage2(&myParamStruct2);
        break;
    case PAGE_3:
        showPage3();
        break;
    }
}
