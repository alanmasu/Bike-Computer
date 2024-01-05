/*
 * mainADC.c
 *
 *  Created on: 24 nov 2023
 *      Author: Federica_Lorenzini
 */
#include "temperatureADC.h"

uint32_t cal30;
uint32_t cal85;
float calDifference;
float tempC;
float tempF;

void hwInit(){

    /*INITIALIZE HARDWARE*/
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();
    Interrupt_disableMaster();
    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    /* Initialize Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);


}
void getTemp(){
    int16_t conRes;
    conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
    tempC = (conRes / calDifference) + 30.0f;
    tempF = tempC * 9.0f / 5.0f + 32.0f;
}

/* This interrupt happens every time a conversion has completed*/
void ADC14_IRQHandler(void){
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); /*clear interrupt flag*/

    if(status & ADC_INT0){
        getTemp();
    }
}

