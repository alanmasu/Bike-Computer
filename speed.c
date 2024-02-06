/*!
    @file       speed.c
    @brief      Speed functions implementation.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

#include "speed.h"

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*!
    @addtogroup speed_Module
    @{
*/
const  float clockFrequency      =       32768.0;       //Hz        //!< Clock frequency: ticks per second.
static float wheelCircumference  =       2.3141;        //metri     //!< Circumference of the wheel user selected (meters)
const  float maxTickVal          =       65535.0;                   //!< Maximum number of ticks the timer does before resetting.


static volatile uint_fast16_t timerAcapturedValue;      //!<Value of the timer captured when the wheel completes one round
static volatile uint_fast16_t overflowCounter = 0;      //!<Counter of timer overflows until wheel completes round.
static volatile uint_fast16_t roundsCounter = 0;        //!<Counter of the rounds the wheel does during the whole bike usage time.
volatile bool speedFlag = false;                        //!<Flag to arise if wheel has completed one round.

/*!
    @brief    Getter for timerAcapturedValue.
    @return   timerAcapturedValue: value in capture register.
*/
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
    @brief    Computes distance covered since wheel starts moving.
    @return   distance: distance covered in meter.
*/
float distanceCovered(){
    float distance = wheelCircumference * roundsCounter;        //metri
    return distance;
}

/*!
    @brief    Resets rounds counter to reset distance.
*/
void resetRoundsCounter(){
    roundsCounter = 0;
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

//    printf("Overflow: %d\n", overflowCounter);
//    printf("secForxTurns: %f\n\n", secForxTurns);

    overflowCounter = 0;
    speedKmH = speedMS * 3.6;

    return speedKmH;
}

/*!
    @brief      Initializes timer in continuous mode and capture mode and starts counter.
    @details    Enables capture interrupt. Sets peripheral input pin.
    @param[in]  continuousModeConfig: timer configuration in continuous mode.
    @param[in]  captureModeConfig: timer configuration in capture mode with interrupt enabled.
*/
void timerInit(const Timer_A_ContinuousModeConfig* continuousModeConfig, 
                       const Timer_A_CaptureModeConfig* captureModeConfig){
    /* Configuring Capture Mode */
    MAP_Timer_A_initCapture(TIMER_A0_BASE, captureModeConfig);

    /* Configuring Up Mode */
    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, continuousModeConfig);

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

        speedFlag = true;
        ++roundsCounter;
        timerAcapturedValue = MAP_Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2);
        MAP_Interrupt_disableSleepOnIsrExit();
        Timer_A_clearTimer(TIMER_A0_BASE);

    } else if (timer == 14){

        ++overflowCounter;
        Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    }
}

///}
