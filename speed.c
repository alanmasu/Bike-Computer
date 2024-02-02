#include "speed.h"

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

const float clockFrequency      =       46875.0;       //Hz
      float wheelCircumference  =       2.3141;        //metri
const float maxTickVal          =       65535.0;

/* Statics */

static volatile uint_fast16_t timerAcapturedValue;
static volatile uint_fast16_t overflowCounter = 0;
volatile bool isrFlag = false;

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

uint_fast16_t getTimerAcapturedValue(){
    return timerAcapturedValue;
}

/*!
    @brief    Sets wheel diameter for this module.
    @param    UserDiameter: user's wheel diameter in inches.
*/
void setWheelDiameter(float userDiameter){
    wheelCircumference = userDiameter / 39.37;
}

/*!
    @brief    Computes speed using captured value from sensor.
    @param    capturedValue: number of timer ticks saved in capture register when interrupt is triggered.
    @return   speedKmH: bike speed measured in km/h.
*/
float speedCompute(uint_fast16_t capturedValue){

    float secForxTurns = (capturedValue + maxTickVal * overflowCounter) / clockFrequency;
    float speedMS;
    float speedKmH;

    speedMS = wheelCircumference / secForxTurns;

    printf("Overflow: %d\n", overflowCounter);
    printf("secForxTurns: %f\n\n", secForxTurns);

    overflowCounter = 0;
    speedKmH = speedMS * 3.6;

    return speedKmH;
}

/*!
    @brief    Initializes timer in continuous mode and capture mode and starts counter. Enables capture interrupt. Sets peripheral input pin.
*/
void timerInit(){
    /* Configuring Capture Mode */
    MAP_Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);

    /* Configuring Up Mode */
    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    /* Starting the Timer_A0 in up mode */
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    MAP_Interrupt_enableInterrupt(INT_TA0_N);
    /* Configuring P2.5 as peripheral input for capture (sensor) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
}


void TA0_N_IRQHandler(void)
{
    uint32_t timer = TIMER_A0->IV;

    if(timer == 4){

        isrFlag = true;
        timerAcapturedValue = MAP_Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2);
        MAP_Interrupt_disableSleepOnIsrExit();
        Timer_A_clearTimer(TIMER_A0_BASE);

    } else if (timer == 14){

        ++overflowCounter;
        Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    }
}
