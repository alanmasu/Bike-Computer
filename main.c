/*!
 * @file main.c
 * @brief MSP432 UART - Loopback with 24MHz DCO BRCLK
 * @details I'm testing UART connections and implementation using DriverLib
 * 	This demo connects TX to ESP32 RX and RX to ESP32 TX,
 *  The example code shows proper initialization of registers
 *  and interrupts to receive and transmit data.
 *
 *  ESP32 is programmed for printing numbers from 0 to 255 when it receives 's' character
 *  and echo on the USB Virtual COM all packets that's receives back
 *
 *
 *  MCLK = HSMCLK = SMCLK = DCO of 24MHz
 *
 * @date 03/01/2024
 */

/* DriverLib Includes */
#ifndef SIMULATE_HARDWARE
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include <ti/devices/msp432p4xx/driverlib/dma.h>
    #include <ti/devices/msp432p4xx/inc/msp.h>
#endif
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Local Includes */
#include "GPS.h"
#ifndef SIMULATE_HARDWARE
    #include <DMAModule.h>
#endif

#ifndef NULL
    #define NULL (void*)0
#endif

#ifndef SIMULATE_HARDWARE
/**
 * @brief Parameters for PC UART initialization
 * @details These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate whit ClockSource of 24MHz.
 * These values were calculated using the online calculator that TI provides
 * at: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_ConfigV1 pcUartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        13,                                         // BRDIV = 13
        0,                                          // UCxBRF = 0
        37,                                         // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                     // No Parity
        EUSCI_A_UART_LSB_FIRST,                     // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                  // One stop bit
        EUSCI_A_UART_MODE,                          // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                      // 8 bit data length
};


/*!
    @brief      Main function thats starts my tests
	@details    This function initializes:
	 			 - The DC0 module at 24MHz
				 - The UART modules
				 - The DMA module
				 - The GPIO pins.
*/

int main(void){
    /* Halting WDT  */
    WDT_A_holdTimer();

    /* P1.0 as output (LED) */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    //Set RGB led pins as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Setting DCO to 24MHz (upping Vcore) */
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);


    /* Configuring UART Modules */
    //Enable PC UART
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);   //PC
    MAP_UART_initModule(EUSCI_A0_BASE, &pcUartConfig);                  //PC
    MAP_UART_enableModule(EUSCI_A0_BASE);                               //PC
    // MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
	//Enable GPS UART
	gpsUartConfig();

	/* Configuring DMA module */
	//Enable DMA module
	dmaInit();
	//Enable DMA for EUSCI_A2 RX
	gpsDMAConfiguration();



    MAP_Interrupt_enableSleepOnIsrExit();
    while(1){
        if(stringEnd){
            for(int i = 0; i < RX_BUFFER_SIZE; ++i){
                MAP_UART_transmitData(EUSCI_A0_BASE, uartData[i]);
            }
            stringEnd = false;
            gpsDMARestoreChannel();
        }
        MAP_Interrupt_enableSleepOnIsrExit();
        MAP_PCM_gotoLPM0InterruptSafe();
    }
}

//void EUSCIA0_IRQHandler(void){
//    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
//
//
//    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
//        uint_fast8_t data = MAP_UART_receiveData(EUSCI_A0_BASE);
//        MAP_UART_transmitData(EUSCI_A0_BASE, data);
//    }
//
//}
#else

//Standard includes
#include <stdio.h>

int main(void){
    FILE* nmeaData = fopen("Test/NMEAFileCorrected.txt", "r");
    if(nmeaData == NULL){
        printf("Error opening file\n");
        return -1;
    }else{
        printf("File opened\n");
        char nmeaString[RX_BUFFER_SIZE];
        char comand;
        do{
            size_t size = fread(nmeaString, sizeof(char), RX_BUFFER_SIZE, nmeaData);
            if(size <= RX_BUFFER_SIZE){
                printf("String read\n");
                printf("String: %s\n", nmeaString);
                stringEnd = true;
                gpsParseData(nmeaString);
            }
            printf("Press 'c' to Continue, press 's' to Stop: c/[s]\n");
            fflush(stdin);
            comand = getchar();
        }while(comand == 'c');
    }
}

#endif
