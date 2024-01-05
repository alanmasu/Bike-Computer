/*
 * main.c
 *
 *  Created on: 25 nov 2023
 *      Author: Federica_Lorenzini
 */
#include "mainInterface.h"
#include "temperatureADC.h"

int main(void){
    hwInit();
    graphicsInit();
    drawGrid();

    /*INITIALIZE ADC MODULE*/
    /* Setting reference voltage to 2.5 and enabling temperature sensor */
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF2_5V);
    REF_A_enableReferenceVoltage();
    /* Initializing ADC (MCLK/1/1)*/
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,ADC_TEMPSENSEMAP);
    /* Configuring ADC Memory ADC_MEM0 A22 (Temperature Sensor) in repeat mode.*/
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);
    /* Configuring the sample/hold time for 192 */
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192,ADC_PULSE_WIDTH_192);
    /* Enabling sample timer in auto iteration mode and interrupts*/
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    ADC14_enableInterrupt(ADC_INT0);
    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();
    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    cal30= SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,SYSCTL_30_DEGREES_C);
    cal85= SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,SYSCTL_85_DEGREES_C);
    calDifference= cal85 - cal30;

    GrStringDraw(&g_sContext,(int8_t*)StringTemp(98),-1, temp.xMin + 60, temp.yMin + 4, 0);
    GrStringDraw(&g_sContext,"Avg Speed: ",-1,avSpeed.xMin+7,avSpeed.yMin+2,0);
    GrStringDraw(&g_sContext,"Inst Speed: ",-1,instSpeed.xMin+7,instSpeed.yMin+2,0);
    GrStringDraw(&g_sContext,"Acc: ",-1,acc.xMin+7,acc.yMin+2,0);
    GrStringDraw(&g_sContext,"Distance: ",-1,distance.xMin+7,distance.yMin+2,0);
    GrStringDraw(&g_sContext,"GpsCoord&Altitude:",-1,coord.xMin+7,coord.yMin+2,0);
    GrStringDrawCentered(&g_sContext,"GO TO MENU",-1,(menu.xMax)/2,109,0);
    GrFlush(&g_sContext);
    free(StringTemp);

}



