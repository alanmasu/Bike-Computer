/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define NUMBER_TIMER_CAPTURES       20
#define WHEEL_CIRCUMFERENCE         2.3141      //metri

/* Timer_A Up Mode Configuration Parameter */
/*const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,               //frequency: 32.768 kHz
        TIMER_A_CLOCKSOURCE_DIVIDER_64,         //new frequency: 512 Hz
        512,                                    //timer counts 512 rising edges of the clock: 1s (qualsiasi sia questo valore, e' importante contare sempre fino a 1s)
        TIMER_A_TAIE_INTERRUPT_ENABLE,
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
        TIMER_A_SKIP_CLEAR
};*/
const Timer_A_ContinuousModeConfig continuousModeConfig =
{
         TIMER_A_CLOCKSOURCE_SMCLK,               //frequency: 3MHz
         TIMER_A_CLOCKSOURCE_DIVIDER_64,         //new frequency: 512 Hz
         TIMER_A_TAIE_INTERRUPT_ENABLE,          //timer counts 512 rising edges of the clock: 1s (qualsiasi sia questo valore, e' importante contare sempre fino a 1s)
         TIMER_A_SKIP_CLEAR
};
const Timer_A_CaptureModeConfig captureModeConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_2,        // CC Register 2
        TIMER_A_CAPTUREMODE_RISING_EDGE,          // Rising Edge
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,        // CCIxB Input Select
        TIMER_A_CAPTURE_SYNCHRONOUS,              // Synchronized Capture
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupt
        TIMER_A_OUTPUTMODE_OUTBITVALUE            // Output bit value
};

/* Statics */
//static volatile uint_fast16_t timerAcaptureValues[NUMBER_TIMER_CAPTURES];
static volatile uint_fast16_t timerAcaptureValues;
static volatile uint32_t timerAcapturePointer = 0;
static volatile uint16_t capturedValue = 0;

bool isrFlag = false;
bool speedZero = false;
int main(void)
{
    /* Stop watchdog timer */
    MAP_WDT_A_holdTimer();

    /* Configuring P7.1 as peripheral input for capture (sensor) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
   // MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN2,
     //       GPIO_SECONDARY_MODULE_FUNCTION);
   // MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN6,
     //       GPIO_PRIMARY_MODULE_FUNCTION);


  /* Configuring Capture Mode */
    MAP_Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);


    /* Configuring Up Mode */
    //MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);
    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    /* Enabling interrupts and going to sleep */
    //MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA0_N);
    MAP_Interrupt_enableMaster();

    /* Starting the Timer_A1 in up mode */
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
   // MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    while(1){
        if(isrFlag){
            printf("Register value: %d\n", timerAcaptureValues);
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
        timerAcaptureValues = MAP_Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2);

    } else if (timer == 14){

        speedZero = true;
        Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    }

     Timer_A_clearTimer(TIMER_A0_BASE);



     MAP_Interrupt_disableSleepOnIsrExit();
}

