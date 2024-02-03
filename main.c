/*!
    @file       main.c
    @brief      Photoresistance value captured and scaled.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "photoresistor.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_32,
        150,
        TIMER_A_TAIE_INTERRUPT_ENABLE,       // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        150                                         
};


int main(void)
{
   /* Halting WDT  */
    MAP_WDT_A_holdTimer();

    ADC14Init(&upModeConfig, &compareConfig);

    MAP_Interrupt_enableSleepOnIsrExit();

    resultPos = 0;

    MAP_Interrupt_enableMaster();

    while (1)
    {
        uint8_t i;
        uint_fast16_t average = 0;
        float convertedAverage = 0;

        for(i=0; i<LIGHT_BUFFER_LENGTH; i++){
            average = average + getResultBuffer()[i];
            printf("Value [%d]: %d\n",i,getResultBuffer()[i]);
        }
        printf("\n");

        average /= LIGHT_BUFFER_LENGTH;

        convertedAverage = photoresistorConverter(average);
        //ADC14_disableInterrupt(ADC_INT0);
        resultPos = 0;

        printf("Average: %d\n",average);
        printf("Converted average: %f \n\n",convertedAverage);

        MAP_Interrupt_enableSleepOnIsrExit();
        MAP_PCM_gotoLPM0();
    }
}