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
volatile bool flagTemp = false;
volatile int16_t conRes;
volatile uint32_t cal30;
volatile uint32_t cal85;
volatile float calDifference;
volatile float tempC;
volatile float tempF;

int main(void)
{

    /* Setting reference voltage to 2.5 and enabling temperature sensor */
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF2_5V);
    REF_A_enableReferenceVoltage();

    cal30 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_30_DEGREES_C);
    cal85 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_85_DEGREES_C);
    calDifference = cal85 - cal30;

    /* Initializing ADC (MCLK/1/1)*/
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

    /* Configuring ADC Memory ADC_MEM0 A22 (Temperature Sensor) in repeat mode.*/
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);

    /* Configuring the sample/hold time for 192 */
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    /* Enabling sample timer in auto iteration mode and interrupts*/
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    ADC14_enableInterrupt(ADC_INT0);

    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    graphicsInit(&config);
    // current_page=PAGE_1;
    drawGrid1();
    showPage1();
    GrFlush(&g_sContext);

    while (1)
    {
        if (flagTemp)
        {
            tempC = (conRes / calDifference) + 30.0f;
            tempF = tempC * 9.0f / 5.0f + 32.0f;
            flagTemp == false;
            ADC14_enableInterrupt(ADC_INT0);
        }

        PCM_gotoLPM0();
    }
}

/* This interrupt happens every time a conversion has completed.*/
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); /*clear interrupt flag*/

    if (status & ADC_INT0)
    {
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
    }
    ADC14_disableInterrupt(ADC_INT0);
}
