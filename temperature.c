/*!
  @file     temperature.c
  @brief    Source file for temperature sensor initialization
  @date     01/02/2024
  @author   Federica Lorenzini
 */
#include "temperature.h"
volatile uint32_t cal30;
volatile uint32_t cal85;
volatile float calDifference;
volatile float tempC;
volatile float tempF;

void temperatureInit()
{
  /* Setting reference voltage to 2.5 and enabling temperature sensor */
  REF_A_enableTempSensor();
  REF_A_setReferenceVoltage(REF_A_VREF2_5V);
  REF_A_enableReferenceVoltage();
  /* Setting temperature reference values */
  cal30 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_30_DEGREES_C);
  cal85 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF, SYSCTL_85_DEGREES_C);
  calDifference = cal85 - cal30;
}
