/******************************************************************************
 * MSP432 UART - Loopback with 24MHz DCO BRCLK
 *
 * Description:
 * I'm testing UART connections and implementation using DriverLib
 *
 * This demo connects TX to ESP32 RX and RX to ESP32 TX,
 *  The example code shows proper initialization of registers
 *  and interrupts to receive and transmit data.
 *
 *  ESP32 is programmed for printing numbers from 0 to 255 when it receives 's' character
 *  and echo on the USB Virtual COM all packets that's receives back
 *
 *
 *  MCLK = HSMCLK = SMCLK = DCO of 24MHz
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *       RST -|     P3.3/UCA0TXD|----> GPS_L80 RX at 9600 8N1
 *            |                 |
 *           -|                 |
 *            |     P3.2/UCA0RXD|----< GPS_L80 TX at 9600 8N1
 *            |                 |
 *            |             P1.0|---> LED
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif

#define RX_BUFFER_SIZE 256                  //! Size of RX buffer
                                            //! Uesed also by DMA as max buffer length

volatile uint8_t uartData[RX_BUFFER_SIZE];  //! GPS UART RX buffer
volatile bool stringEnd = false;            //! Flag for end of string

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        156,                                        // BRDIV = 13
        4,                                          // UCxBRF = 0
        0,                                          // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                     // No Parity
        EUSCI_A_UART_LSB_FIRST,                     // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                  // One stop bit
        EUSCI_A_UART_MODE,                          // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                      // 8 bit data length
};

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


/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32];

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
    //Setting pins
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);  //GPS
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);   //PC
    //Configure module
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);                    //GPS
    MAP_UART_initModule(EUSCI_A0_BASE, &pcUartConfig);                  //PC
    // Enable UART module
    MAP_UART_enableModule(EUSCI_A2_BASE);                               //GPS
    MAP_UART_enableModule(EUSCI_A0_BASE);                               //PC
    // Enabling UART interrupts
    // MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    // MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    //DMA
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);
    MAP_DMA_assignChannel(DMA_CH5_EUSCIA2RX);
    /*! 
        @brief      Set DMA chennel for EUSCI_A2 RX
        @details    Set DMA chennel for EUSCI_A2 RX to use Primary DMA Mode sets also: 
                     - 8bit data size 
                     - source address increment is none, (source is fixed)
                     - destination address increment is 8bit (destination is incremented by 1)
                     - arbitration size is 1 (one transfer per request)
    */
    DMA_setChannelControl(DMA_CH5_EUSCIA2RX | UDMA_PRI_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
    /*!
        @brief      Set DMA chennel transfer parameters for EUSCI_A2 RX
        @details    Set DMA chennel for EUSCI_A2 RX to use Primary DMA Mode sets also: 
                     - Basic transfer mode
                     - Source address
                     - Destination address
                     - Transfer size
    */
    DMA_setChannelTransfer(DMA_CH5_EUSCIA2RX | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC,
                               (void *) UART_getReceiveBufferAddressForDMA(EUSCI_A2_BASE),
                               uartData,
                               RX_BUFFER_SIZE);

    //Enable DMA interrupts
    MAP_DMA_assignInterrupt(INT_DMA_INT1, 5);               //!< Assing DMA interrupt 1 to channel 5
    MAP_DMA_clearInterruptFlag(DMA_CH5_EUSCIA2RX & 0x0F);   //!< Clear interrupt flag for channel 5

    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);            //!< Enable DMA interrupt
    MAP_DMA_enableInterrupt(INT_DMA_INT1);                  //!< Enable DMA interrupt 1
    MAP_DMA_enableChannel(5);                               //!< Enable DMA channel 5


    MAP_Interrupt_enableSleepOnIsrExit();
    while(1){
        if(stringEnd){
            for(int i = 0; i < RX_BUFFER_SIZE; ++i){
                MAP_UART_transmitData(EUSCI_A0_BASE, uartData[i]);
            }
            stringEnd = false;
            DMA_setChannelTransfer(DMA_CH5_EUSCIA2RX | UDMA_PRI_SELECT,
                                   UDMA_MODE_BASIC,
                                   (void *) UART_getReceiveBufferAddressForDMA(EUSCI_A2_BASE),
                                   uartData,
                                   RX_BUFFER_SIZE);
            MAP_DMA_enableChannel(5);
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


/*! 
    @brief      DMA interrupt handler and disable interrupts
*/ 
void DMA_INT1_IRQHandler(void){
    stringEnd = true;
    /* Disable the interrupt to allow execution */
    MAP_Interrupt_disableSleepOnIsrExit();
}
