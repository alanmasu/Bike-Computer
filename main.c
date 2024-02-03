/*!
    @file       main.c
    @brief      Photoresistance value captured and scaled, measuring of wheel speed.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "photoresistor.h"

#include "speed.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

const Timer_A_ContinuousModeConfig speedContinuousModeConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              //frequency: 3MHz
         TIMER_A_CLOCKSOURCE_DIVIDER_64,         //new frequency: 46875 Hz
         TIMER_A_TAIE_INTERRUPT_ENABLE,
         TIMER_A_SKIP_CLEAR
};

/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig photoresistorUpModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_32,
        150,
        TIMER_A_TAIE_INTERRUPT_ENABLE,       // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig photoresistorCompareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        150                                         
};

const Timer_A_CaptureModeConfig speedCaptureModeConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_2,        // CC Register 2
        TIMER_A_CAPTUREMODE_RISING_EDGE,          // Rising Edge
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,        // CCIxA Input Select
        TIMER_A_CAPTURE_SYNCHRONOUS,              // Synchronized Capture
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupt
        TIMER_A_OUTPUTMODE_OUTBITVALUE            // Output bit value
};

int main(void)
{
    float speed;

    /* Stop watchdog timer */
    MAP_WDT_A_holdTimer();

    ADC14Init(&photoresistorUpModeConfig, &photoresistorCompareConfig);

    MAP_Interrupt_enableSleepOnIsrExit();

    resultPos = 0;

    timerInit(&speedContinuousModeConfig, &speedCaptureModeConfig);

    MAP_Interrupt_enableMaster();

    while (1)
    {
        uint8_t i;
        uint_fast16_t average = 0;
        float convertedAverage = 0;

        if(isrFlag){

            printf("Register value: %d\n", getTimerAcapturedValue());
            speed = speedCompute(getTimerAcapturedValue());
            printf("Speed: %f Km/h \n", speed);

            isrFlag = false;
        }

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
