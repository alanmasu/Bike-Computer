/*!
    @file       main.c
    @brief      Configure timer to measure wheel speed.
    @date       29/01/2024
    @author     Sofia Zandonà
*/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

const float  clockFrequency =            46875.0;       //Hz
      float wheelCircumference  =       2.3141  ;    //metri

/* Statics */

static volatile uint_fast16_t timerAcapturedValue;
static volatile float speed;
static volatile uint_fast16_t overflowCounter = 0;

const Timer_A_ContinuousModeConfig continuousModeConfig =
{
         TIMER_A_CLOCKSOURCE_HSMCLK,              //frequency: 3MHz
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

/*!
    @brief    Sets wheel diameter for this module.
    @param    UserDiameter: user's wheel diameter in inches.  
*/
void setWheelDiameter(float userDiameter){
    wheelCircumference = userDiameter / 39.37;
}

float speedCompute(uint_fast16_t capturedValue){

    float secForxTurns = (capturedValue + clockFrequency * overflowCounter) / clockFrequency;
    float speedMS;
    float speedKmH;

    if(overflowCounter == 0){
        speedMS = wheelCircumference / secForxTurns;
    } else {
        speedMS = (wheelCircumference * overflowCounter) / secForxTurns;
    }

    speedKmH = speedMS * 3.6;

    if(overflowCounter != 0)
        overflowCounter = 0;

    return speedKmH;
}


bool isrFlag = false;
bool speedZero = false;

int main(void)
{
    /* Stop watchdog timer */
    MAP_WDT_A_holdTimer();

     /* Starting and enabling HSMCLK (3MHz/64) */
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_64);

    /* Configuring P7.1 as peripheral input for capture (sensor) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Capture Mode */
    MAP_Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);

    /* Configuring Up Mode */
    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    /* Enabling interrupts and going to sleep */
    //MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA0_N);
    MAP_Interrupt_enableMaster();

    /* Starting the Timer_A1 in up mode */
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
   // MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    //float secForxTurns;
    //float speedMS;
                //float speedKmH = speedMS;

    while(1){
        if(isrFlag){

            printf("Register value: %d\n", timerAcapturedValue);

            /*secFor1Turn = timerAcapturedValue / clockFrequency;
            printf("secFor1Turn: %f\n", secFor1Turn);

            speedMS = wheelCircumference / secFor1Turn;
            printf("speedMS: %f\n\n", speedMS);

           //float speedKmH = speedMS;*/
            speed = speedCompute(timerAcapturedValue);
            printf("Speed: %f Km/h \n\n", speed);
            isrFlag = false;
        }

        MAP_Interrupt_enableSleepOnIsrExit();

    }

}

void TA0_N_IRQHandler(void)
{
    uint32_t timer = TIMER_A0->IV;

    if(timer == 4){

        isrFlag = true;
        timerAcapturedValue = MAP_Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2);

    } else if (timer == 14){

        speedZero = true;
        ++overflowCounter;
        Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    }

     Timer_A_clearTimer(TIMER_A0_BASE);



     MAP_Interrupt_disableSleepOnIsrExit();
}

