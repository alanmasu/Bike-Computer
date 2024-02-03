/*!
    @file   mainInterface.h
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

/*!
    @defgroup LCD_module LCD
    @{
*/

extern Graphics_Context g_sContext;
extern tRectangle mutipleData;
extern tRectangle instSpeed;
extern tRectangle tripTime;

/*!
    @brief Enum used to implement page scrolling.
*/
typedef enum
{
    PAGE_1,
    PAGE_2,
    PAGE_3,
} Page_t;

extern Page_t myPage;

/*!
    @brief Struct used to collect data from other modules in order to save and then show their values in page 1.
*/
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

/*!
    @brief Struct used to collect data from other modules in order to save and then show their values in page 2.
*/
typedef struct toShowPage2
{
    float hdop;
    float vdop;
    float speed;
    char fixType[6];
} toShowPage2;
extern toShowPage2 myParamStruct2;

/*!
    @brief Initialize the values for the background context (white background and blue foreground).
*/
void graphicsInit(eUSCI_SPI_MasterConfig *);

/*!
    @brief Initialiaze the values for the pink selected context, used for selection purpose in page n°3.
*/
void graphicsInitSelected(eUSCI_SPI_MasterConfig *);

/*!
    @brief Initialize the values for the background context with bigger font set.
*/
void graphicsInitBigFont(eUSCI_SPI_MasterConfig *);

/*!
    @brief Draw grid in the first page.
*/
void drawGrid1();

/*!
    @brief Draw grid in the second page.
*/
void drawGrid2();

/*!
    @brief To show content of the first page.
    @details The content is depending on the conversion choice done in page n°3.
    @param[in] paramToShow1: refers to the struct of parameters that must be shown in the page n°1.
*/
void showPage1(toShowPage1 *paramToShow1);

/*!
    @brief To show content of the second page.
    @details The content is based on the values returned by the GPS sensor.
    @param[in] paramToShow1: refers to the struct of parameters that must be shown in the page n°2.
*/
void showPage2(toShowPage2 *paramToShow2);

/*!
    @brief To show content of the third page.
    @details The page shows the different possibilities to convert distance, speed, temperature and wheels dimension.
                Using the up/down functionality of the joystick you can choice which value you want to convert. Once you have chosen
                which value is to be converted, then using select button of the joystick you can choice in which unit it 
                must be shown.
*/
void showPage3();

/*!
    @brief To scroll pages.
    @details This function allows you to scroll between the pages using the joystick. There's a total of 3 pages, and you
            can navigate through them usim the right/left functionality of the joystick.
*/
void scrollPages();

/*!
    @brief Function to show the current page, according to variable myPage.
    @details Each page has its own function to display the content.
*/
void showPages();

///@}
#endif /* MAININTERFACE_H_ */
