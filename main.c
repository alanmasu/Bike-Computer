/*!
    @file   main.c
    @brief  Initializing main functions for LCD screen and drawing grid
    @date   29/01/2024
    @author Federica Lorenzini
*/
#include "mainInterface.h"

eUSCI_SPI_MasterConfig config =
    {
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        LCD_SYSTEM_CLOCK_SPEED,
        LCD_SPI_CLOCK_SPEED,
        EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
        EUSCI_B_SPI_3PIN};

volatile bool flagTemp;
volatile int16_t conRes;
volatile uint32_t cal30;
volatile uint32_t cal85;
volatile float calDifference;
volatile float tempC;
volatile float tempF;

int main(void)
{
    graphicsInit(&config);
    /* Setting reference voltage to 2.5 and enabling temperature sensor */
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF2_5V);
    REF_A_enableReferenceVoltage();

    cal30 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_30_DEGREES_C);
    cal85 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_85_DEGREES_C);
    calDifference = cal85 - cal30;

    /* Configures Pin 6.0 (horX), 4.4(verY) and 4.0(select) as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initializing ADC*/
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

    /* Configuring ADC Memory ADC_MEM0 A22 (Temperature Sensor) in repeat mode.*/
    /* Configuring ADC Memory ADC_MEM1 (A15) and ADC_MEM2 (A9) in repeat mode)*/
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, false);
    ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, false);

    /* Configuring the sample/hold time for 192 */
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    /* Enabling sample timer in auto iteration mode and interrupts*/
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    ADC14_enableInterrupt(ADC_INT0);
    ADC14_enableInterrupt(ADC_INT1);

    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    Page_t myPage=PAGE_1;

    while (1)
    {
        if (flagTemp)
        {
            myParamStruct.temp = (conRes / calDifference) + 30.0f-19;
            // tempF = tempC * 9.0f / 5.0f + 32.0f;
            flagTemp == false;
            Interrupt_enableInterrupt(INT_ADC14);
        }
        scrollPages();
        showPages();
        GrFlush(&g_sContext);
        Interrupt_enableSleepOnIsrExit();
        // PCM_gotoLPM0();
    }
}

/* This interrupt happens every time a conversion has completed.*/
void ADC14_IRQHandler(void)
{
    flagTemp = true;
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); /*clear interrupt flag*/

    if (status & ADC_INT0)
    {
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
    }
    Interrupt_disableInterrupt(INT_ADC14);
    Interrupt_disableSleepOnIsrExit();
}
