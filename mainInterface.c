/*!
    @file   mainInterface.c
    @brief  Initializing main functions for LCD screen and drawing grid
    @date   26/11/2023
    @author Federica Lorenzini
*/
#include "mainInterface.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <stdlib.h>

/*!
    @addtogroup LCD_module
    @{
*/

Graphics_Context g_sContext;
Graphics_Context g_sContextSelected;
Graphics_Context g_sContextBig;
tRectangle multipleData = {0, 0, 64, 102}; //!< Part of the main interface grid
tRectangle instSpeed = {64, 0, 128, 102};  //!< Part of the main interface grid
tRectangle tripTime = {0, 102, 128, 128};  //!< Part of the main interface grid
Page_t myPage = PAGE_1;                    //!< Main page
toShowPage1 myParamStruct;                 //!< Struct with the parameters to be shown in the first page
toShowPage2 myParamStruct2;                //!< Struct with the parameters to be shown in the second page
int XaxisPrev = 0;
int YaxisPrev = 0;
int Ycounter = 1;
int selectSpeed = 0;
int selectDist = 0;
int selectWheel = 0;
int selectTemp = 0;
static float wheelDim = 29.0;
float metres = 0;     //!< To store distance in metres
float miles = 0;      //!< To store distance in miles
float fahrenheit = 0; //!< To store temperature in fahrenheit
float ms = 0;         //!< To store speed in metres/second
float mih = 0;        //!< To store speeed in miles/hour

void graphicsInit(eUSCI_SPI_MasterConfig *config)
{
    Crystalfontz128x128_Init(config);
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

void graphicsInitBigFont(eUSCI_SPI_MasterConfig *config)
{
    Crystalfontz128x128_Init(config);
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContextBig, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContextBig, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContextBig, GRAPHICS_COLOR_WHITE);
    Graphics_setFont(&g_sContextBig, &g_sFontCmtt24);
    Graphics_clearDisplay(&g_sContextBig);
}

void graphicsInitSelected(eUSCI_SPI_MasterConfig *config)
{
    Crystalfontz128x128_Init(config);
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContextSelected, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContextSelected, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContextSelected, GRAPHICS_COLOR_PINK);
    GrContextFontSet(&g_sContextSelected, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContextSelected);
}

void drawGrid1()
{
    GrRectDraw(&g_sContext, &multipleData);
    GrStringDraw(&g_sContext, (int8_t *)"Distance:", -1, multipleData.xMin + 7, multipleData.yMin + 2, 1);
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
    // distance-altitude-satellites-temperature
    switch (selectDist)
    {
    case 0:
        snprintf(tmpString, 39, "%2.2f", paramToShow1->distance);
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);
        snprintf(tmpString, 39, "km");
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 40, multipleData.yMin + 10, 1);
        break;
    case 1:
        snprintf(tmpString, 39, "%2.2f", metres);
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);
        snprintf(tmpString, 39, "m");
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 58, multipleData.yMin + 10, 1);
        break;
    case 2:
        snprintf(tmpString, 39, "%2.2f", miles);
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 10, 1);
        snprintf(tmpString, 39, "mi");
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 40, multipleData.yMin + 10, 1);
        break;
    }
    snprintf(tmpString, 39, "%4.2f m", paramToShow1->altitude);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 35, 1);

    snprintf(tmpString, 39, "%d", paramToShow1->sats);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 60, 1);

    switch (selectTemp)
    {
    case 0:
        snprintf(tmpString, 39, "%2.1f", paramToShow1->temp);
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 85, 1);
        snprintf(tmpString, 39, "C");
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 50, multipleData.yMin + 85, 1);
        break;
    case 1:
        snprintf(tmpString, 39, "%2.2f", fahrenheit);
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 7, multipleData.yMin + 85, 1);
        snprintf(tmpString, 39, "F");
        GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, multipleData.xMin + 50, multipleData.yMin + 85, 1);
    default:
        break;
    }

    // speed and time
    snprintf(tmpString, 39, "Time: %s", paramToShow1->time);
    GrStringDraw(&g_sContext, (int8_t *)tmpString, -1, 95, 7, 1);

    switch (selectSpeed)
    {
    case 0:
        snprintf(tmpString, 39, "%2.1f", paramToShow1->speed);
        GrStringDrawCentered(&g_sContextBig, (int8_t *)tmpString, -1, 96, 55, 1);
        snprintf(tmpString, 39, "km/h");
        GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 70, 1);
        break;
    case 1:
        snprintf(tmpString, 39, "%2.1f", ms);
        GrStringDrawCentered(&g_sContextBig, (int8_t *)tmpString, -1, 96, 55, 1);
        snprintf(tmpString, 39, "m/s");
        GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 70, 1);
        break;
    case 2:
        snprintf(tmpString, 39, "%2.1f", mih);
        GrStringDrawCentered(&g_sContextBig, (int8_t *)tmpString, -1, 96, 55, 1);
        snprintf(tmpString, 39, "mi/h");
        GrStringDrawCentered(&g_sContext, (int8_t *)tmpString, -1, 96, 70, 1);
        break;
    default:
        break;
    }

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
    /*Using joystick's vertical axis (y) to choice which field should be converted*/
    int Yaxis = MAP_ADC14_getResult(ADC_MEM2);
    switch (Ycounter)
    {
    case 1:
        GrStringDraw(&g_sContextSelected, (int8_t *)"SPACE", -1, 3, 20, 1);
        GrStringDraw(&g_sContext, (int8_t *)"SPEED", -1, 3, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"km/h", -1, 45, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"m/s", -1, 75, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"mi/h", -1, 100, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"TEMP", -1, 3, 60, 1);
        GrStringDraw(&g_sContext, (int8_t *)"C", -1, 45, 60, 1);
        GrStringDraw(&g_sContext, (int8_t *)"F", -1, 90, 60, 1);
        GrStringDraw(&g_sContext, (int8_t *)"Wheel dim", -1, 3, 80, 1);
        GrStringDraw(&g_sContext, (int8_t *)"29 in", -1, 80, 92, 1);
        GrStringDrawCentered(&g_sContext, (int8_t *)"(Click", -1, 20, 92, 1);
        GrStringDraw(&g_sContext, (int8_t *)"to modify)", -1, 3, 100, 1);
        switch (selectDist)
        {
        case 0:
            GrStringDraw(&g_sContextSelected, (int8_t *)"km", -1, 45, 20, 1);
            GrStringDraw(&g_sContext, (int8_t *)"m", -1, 75, 20, 1);
            GrStringDraw(&g_sContext, (int8_t *)"mi", -1, 100, 20, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectDist = 1;
            }
            break;
        case 1:
            metres = myParamStruct.distance * 1000.0;
            GrStringDraw(&g_sContext, (int8_t *)"km", -1, 45, 20, 1);
            GrStringDraw(&g_sContextSelected, (int8_t *)"m", -1, 75, 20, 1);
            GrStringDraw(&g_sContext, (int8_t *)"mi", -1, 100, 20, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectDist = 2;
            }
            break;
        case 2:
            miles = myParamStruct.distance * 0.62;
            GrStringDraw(&g_sContext, (int8_t *)"km", -1, 45, 20, 1);
            GrStringDraw(&g_sContext, (int8_t *)"m", -1, 75, 20, 1);
            GrStringDraw(&g_sContextSelected, (int8_t *)"mi", -1, 100, 20, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectDist = 0;
            }
            break;
        }
        if (Yaxis < 200)
        {
            Ycounter = 2;
        }
        if (Yaxis > 14000)
        {
            Ycounter = 4;
        }
        break;
    case 2:
        GrStringDraw(&g_sContext, (int8_t *)"SPACE", -1, 3, 20, 1);
        GrStringDraw(&g_sContextSelected, (int8_t *)"SPEED", -1, 3, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"TEMP", -1, 3, 60, 1);
        GrStringDraw(&g_sContext, (int8_t *)"Wheel dim", -1, 3, 80, 1);
        GrStringDrawCentered(&g_sContext, (int8_t *)"(Click", -1, 20, 92, 1);
        GrStringDraw(&g_sContext, (int8_t *)"to modify)", -1, 3, 100, 1);
        switch (selectSpeed)
        {
        case 0:
            GrStringDraw(&g_sContextSelected, (int8_t *)"km/h", -1, 45, 40, 1);
            GrStringDraw(&g_sContext, (int8_t *)"m/s", -1, 75, 40, 1);
            GrStringDraw(&g_sContext, (int8_t *)"mi/h", -1, 100, 40, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectSpeed = 1;
            }
            break;
        case 1:
            ms = myParamStruct.speed / 3.6;
            GrStringDraw(&g_sContext, (int8_t *)"km/h", -1, 45, 40, 1);
            GrStringDraw(&g_sContextSelected, (int8_t *)"m/s", -1, 75, 40, 1);
            GrStringDraw(&g_sContext, (int8_t *)"mi/h", -1, 100, 40, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectSpeed = 2;
            }
            break;
        case 2:
            mih = myParamStruct.speed * 0.62;
            GrStringDraw(&g_sContext, (int8_t *)"km/h", -1, 45, 40, 1);
            GrStringDraw(&g_sContext, (int8_t *)"m/s", -1, 75, 40, 1);
            GrStringDraw(&g_sContextSelected, (int8_t *)"mi/h", -1, 100, 40, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectSpeed = 0;
            }
            break;
        }
        if (Yaxis < 200)
        {
            Ycounter = 3;
        }
        if (Yaxis > 14000)
        {
            Ycounter = 1;
        }
        break;
    case 3:
        GrStringDraw(&g_sContext, (int8_t *)"SPACE", -1, 3, 20, 1);
        GrStringDraw(&g_sContext, (int8_t *)"SPEED", -1, 3, 40, 1);
        GrStringDraw(&g_sContextSelected, (int8_t *)"TEMP", -1, 3, 60, 1);
        GrStringDraw(&g_sContext, (int8_t *)"Wheel dim", -1, 3, 80, 1);
        GrStringDrawCentered(&g_sContext, (int8_t *)"(Click", -1, 20, 92, 1);
        GrStringDraw(&g_sContext, (int8_t *)"to modify)", -1, 3, 100, 1);
        switch (selectTemp)
        {
        case 0:
            GrStringDraw(&g_sContextSelected, (int8_t *)"C", -1, 45, 60, 1);
            GrStringDraw(&g_sContext, (int8_t *)"F", -1, 90, 60, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectTemp = 1;
            }
            break;
        case 1:
            fahrenheit = (myParamStruct.temp * 1.8) + 32.0;
            GrStringDraw(&g_sContext, (int8_t *)"C", -1, 45, 60, 1);
            GrStringDraw(&g_sContextSelected, (int8_t *)"F", -1, 90, 60, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectTemp = 0;
            }
            break;
        }
        if (Yaxis < 200)
        {
            Ycounter = 4;
        }
        if (Yaxis > 14000)
        {
            Ycounter = 2;
        }
        break;

    case 4:
        GrStringDraw(&g_sContext, (int8_t *)"SPACE", -1, 3, 20, 1);
        GrStringDraw(&g_sContext, (int8_t *)"SPEED", -1, 3, 40, 1);
        GrStringDraw(&g_sContext, (int8_t *)"TEMP", -1, 3, 60, 1);
        GrStringDraw(&g_sContextSelected, (int8_t *)"Wheel dim", -1, 3, 80, 1);
        GrStringDrawCentered(&g_sContext, (int8_t *)"(Click", -1, 20, 92, 1);
        GrStringDraw(&g_sContext, (int8_t *)"to modify)", -1, 3, 100, 1);
        switch (selectWheel)
        {
        case 0:
            GrStringDraw(&g_sContextSelected, (int8_t *)"29 in", -1, 80, 92, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectWheel = 1;
            }
            break;
        case 1:
            GrStringDraw(&g_sContextSelected, (int8_t *)"27 in", -1, 80, 92, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectWheel = 2;
            }
            break;
        case 2:
            GrStringDraw(&g_sContextSelected, (int8_t *)"26 in", -1, 80, 92, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectWheel = 3;
            }
            break;
        case 3:
            GrStringDraw(&g_sContextSelected, (int8_t *)"28 in", -1, 80, 92, 1);
            if (!(P4IN & GPIO_PIN1))
            {
                selectWheel = 0;
            }
            break;
        }
        if (Yaxis < 200)
        {
            Ycounter = 1;
        }
        if (Yaxis > 14000)
        {
            Ycounter = 3;
        }
        break;
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
        if (Xaxis < 600 && diff > 3000)
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
        if (Xaxis < 600 && diff > 3000)
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
        if (Xaxis < 600 && diff > 3000)
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

///@}
