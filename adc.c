/*!
  @file     adc.c
  @brief    Source file for ADC initialization
  @date     01/02/2024
  @author   Federica Lorenzini
 */
#include "adc.h"

void adcInit()
{
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
}
