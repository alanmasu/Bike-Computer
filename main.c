/*!
    @file   main.c
    @brief  Initializing main functions for LCD screen and drawing grid
    @date   29/01/2024
    @author Federica Lorenzini
*/
#include "mainInterface.h"
#include "temperature.h"
#include "adc.h"

volatile bool flagTemp;
volatile int16_t conRes;

eUSCI_SPI_MasterConfig config = {
    EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
    LCD_SYSTEM_CLOCK_SPEED,
    LCD_SPI_CLOCK_SPEED,
    EUSCI_B_SPI_MSB_FIRST,
    EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
    EUSCI_B_SPI_3PIN};

int main(void)
{
    _graphicsInitSelected(&config);
    graphicsInit(&config);
    temperatureInit();
    adcInit();
    Page_t myPage = PAGE_1;

    while (1)
    {
        if (flagTemp)
        {
            myParamStruct.temp = (conRes / calDifference) + 30.0f - 19;
            // tempF = tempC * 9.0f / 5.0f + 32.0f;
            flagTemp == false;
            Interrupt_enableInterrupt(INT_ADC14);
        }
        scrollPages();
        showPages();
        GrFlush(&g_sContext);
        Interrupt_enableSleepOnIsrExit();
    }
}

/* This interrupt happens every time a temperature conversion has completed.*/
void ADC14_IRQHandler(void)
{
    flagTemp = true;
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); /*clear interrupt flag*/

    if (status & ADC_INT0)
    {
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
    }
    Interrupt_disableInterrupt(INT_ADC14);
    Interrupt_disableSleepOnIsrExit();
}
