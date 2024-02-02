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


int main(void)
{
    float speed;

    /* Stop watchdog timer */
    MAP_WDT_A_holdTimer();

    timerInit();

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
