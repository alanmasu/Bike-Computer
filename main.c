/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_32,       // ACLK/1 = 32Khz
        150,
        TIMER_A_TAIE_INTERRUPT_ENABLE,      // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        150                                       // 16000 Period
};

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

//LCD PART
    /* Configures Pin 6.0 (horX), 4.4(verY) and 4.0(select) as ADC input */
/*MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
  //GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);*/

  /* Initializing ADC*/
  //ADC14_enableModule();
  //ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

  /* Configuring ADC Memory ADC_MEM0 A22 (Temperature Sensor) in repeat mode.*/
  /* Configuring ADC Memory ADC_MEM1 (A15) and ADC_MEM2 (A9) in repeat mode)*/
/*MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);
MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, false);
MAP_ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, false);*/

  /* Configuring the sample/hold time for 192 */
  //ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);    //???????
  //ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_4, ADC_PULSE_WIDTH_4);  

  /* Enabling sample timer in auto iteration mode and interrupts*/
  //ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
/*MAP_ADC14_enableInterrupt(ADC_INT0);
MAP_ADC14_enableInterrupt(ADC_INT1);*/

  /* Enabling Interrupts */
  //Interrupt_enableInterrupt(INT_ADC14);
  //Interrupt_enableMaster();

  /* Triggering the start of the sample */
  //ADC14_enableConversion();
  //ADC14_toggleConversionTrigger();


}

/* Statics */
static volatile uint_fast16_t resultsBuffer[20];
static volatile uint8_t resultPos;

int main(void)
{
   /* Halting WDT  */
    MAP_WDT_A_holdTimer();
    ADC14Init(&upModeConfig, &compareConfig);
    MAP_Interrupt_enableSleepOnIsrExit();
    resultPos = 0;

    /* Setting up clocks
     * MCLK = MCLK = 3MHz
     * ACLK = REFO = 32Khz */
    /* MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); */

    /* Initializing ADC (MCLK/1/1) */
    /* MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
            0); */

    /* Configuring GPIOs (5.5 A0) */
   /*  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4,
    GPIO_TERTIARY_MODULE_FUNCTION);
 */
    /* Configuring ADC Memory */
   /*  MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A1, false);
 */
    /* Configuring Timer_A in continuous mode and sourced from ACLK */
    /* MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);*/

    /* Configuring Timer_A0 in CCR1 to trigger at 16000 (0.5s) */
   /* MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig); */

    /* Configuring the sample trigger to be sourced from Timer_A0  and setting it
     * to automatic iteration after it is triggered*/
   /*  MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false); */

    /* Enabling the interrupt when a conversion on channel 1 is complete and
     * enabling conversions */
    /* MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_ADC14_enableConversion(); */

    /* Enabling Interrupts */
  /*   MAP_Interrupt_enableInterrupt(INT_ADC14); */
    MAP_Interrupt_enableMaster();

    /* Starting the Timer */
   /*  MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); */

    

    while (1)
    {
        uint8_t i;
        uint_fast16_t average = 0;
        float convertedAverage = 0;

        for(i=0; i<5; i++){
            average = average + resultsBuffer[i];
            printf("Value [%d]: %d\n",i,resultsBuffer[i]);
        }
        printf("\n");

        average /= 5;

        convertedAverage = photoresistorConverter(average);
        //ADC14_disableInterrupt(ADC_INT0);
        resultPos = 0;

        printf("Average: %d\n",average);
        printf("Converted average: %f \n\n",convertedAverage);

        MAP_Interrupt_enableSleepOnIsrExit();
        MAP_PCM_gotoLPM0();
    }
}

/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM0 */
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
