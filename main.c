/*!
    @file       main.c
    @brief      Configure timer to measure wheel speed.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "speed.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

const Timer_A_ContinuousModeConfig continuousModeConfig =
{
         TIMER_A_CLOCKSOURCE_SMCLK,              //frequency: 3MHz
         TIMER_A_CLOCKSOURCE_DIVIDER_64,         //new frequency: 46875 Hz
         TIMER_A_TAIE_INTERRUPT_ENABLE,
         TIMER_A_SKIP_CLEAR
};

const Timer_A_CaptureModeConfig captureModeConfig =
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

    timerInit(&continuousModeConfig, &captureModeConfig);

    MAP_Interrupt_enableMaster();

    while(1){

       if(isrFlag){

            printf("Register value: %d\n", getTimerAcapturedValue());
            speed = speedCompute(getTimerAcapturedValue());
            printf("Speed: %f Km/h \n", speed);

            isrFlag = false;
        }

        MAP_Interrupt_enableSleepOnIsrExit();


    }

}
