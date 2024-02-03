/*!
    @file       photoresistor.c
    @brief      Photoresistor functions implementation.
    @date       29/01/2024
    @author     Sofia Zandona'
*/

#include "photoresistor.h"

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*!
    @addtogroup Photoresistor_Module
    @{
*/

/* Statics */
volatile uint_fast16_t resultsBuffer[LIGHT_BUFFER_LENGTH];
volatile uint8_t resultPos;

/*!
    @brief    Getter for resultsBuffer.
    @return   resultsBuffer: pointer to resultBuffer array.
*/
uint_fast16_t* getResultBuffer(){
    return (uint_fast16_t*)resultsBuffer;
}

/*!
    @brief    Scales photoresistor value in a 0-100 scale.
    @param    sampledValue: measured value of photoresistor resistance.
    @return   scaledValue: photoresistor scaled value.
*/
float photoresistorConverter(uint_fast16_t sampledValue){
    //min val = 0
    //max val = 16384
    //returns value from 0 to 1

    float maxValue = 16384;
    float scaledValue = sampledValue / maxValue;

    return scaledValue;

}

/*!
    @brief      Initializes ADC14 with timer trigger and in multisequence mode.
    @details    Configures input pins and ADC memory.
    @param[in]  upModeConfig: timer configuration in up mode.
    @param[in]  compareConfig: timer configuration in compare mode with interrupt disabled.
*/
void ADC14Init(const Timer_A_UpModeConfig* upModeConfig, 
                    const Timer_A_CompareModeConfig* compareConfig){
     /* Setting up clocks
     * MCLK = MCLK = 3MHz
     * ACLK = REFO = 32Khz */
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initializing ADC (MCLK/1/1) */
    MAP_ADC14_enableModule();
     //MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

    /* Configuring GPIOs (5.4 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    //MAP_ADC14_configureSingleSampleMode(ADC_MEM3, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM3, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A1, false);
//MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, true);
MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);
MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, false);
MAP_ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, false);
MAP_ADC14_configureConversionMemory(ADC_MEM3, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A1, false);

    /* Configuring Timer_A in continuous mode and sourced from ACLK */
    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, upModeConfig);

    /* Configuring Timer_A0 in CCR1 to trigger at 16000 (0.5s) */
    MAP_Timer_A_initCompare(TIMER_A2_BASE, compareConfig);

    /* Configuring the sample trigger to be sourced from Timer_A2  and setting it
     * to automatic iteration after it is triggered*/
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE5, false);

    /* Enabling the interrupt when a conversion on channel 1 is complete and
     * enabling conversions */
    MAP_ADC14_enableInterrupt(ADC_INT3);
MAP_ADC14_enableInterrupt(ADC_INT0);
MAP_ADC14_enableInterrupt(ADC_INT1);

    MAP_ADC14_enableConversion();
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
}


void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);


    if (status & ADC_INT3)
    {
        if(resultPos < 5) {
            resultsBuffer[resultPos++] = MAP_ADC14_getResult(ADC_MEM3);
        } else {
            MAP_Interrupt_disableSleepOnIsrExit();
        }
    }

}


void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}


///}
