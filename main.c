/*!
    @file   main.c
    @brief  Initializing main functions for LCD screen and drawing grid
    @date   29/01/2024
    @author Federica Lorenzini
*/
#include "mainInterface.h"

eUSCI_SPI_MasterConfig config =
        {
            EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
            LCD_SYSTEM_CLOCK_SPEED,
            LCD_SPI_CLOCK_SPEED,
            EUSCI_B_SPI_MSB_FIRST,
            EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
            EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
            EUSCI_B_SPI_3PIN
        };

int main(void){
    graphicsInit(&config);
    //current_page=PAGE_1;
    drawGrid1();
    showPage1();
    GrFlush(&g_sContext);

}



