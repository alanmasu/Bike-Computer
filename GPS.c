/*!
    @file   GPS.c
    @brief  GPS module functions and definitions
    @date   03/01/2024
    @author Alan Masutti
*/
*/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Local Includes*/
#include <GPS.h>
#include <DMAModule.h>

volatile uint8_t uartData[RX_BUFFER_SIZE];  //! GPS UART RX buffer
volatile bool stringEnd = false;            //! Flag for end of string

/**
 * @brief Parameters for PC UART initialization
 * @details These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate whit ClockSource of 24MHz. 
 * These values were calculated using the online calculator that TI provides
 * at: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 * 
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


/*!
    @brief GPS UART module initialization
    @details This function initializes the UART module for GPS communication
    @param none
    @return none
    @note For this configuration is necessary to set the SMCLK to 24MHz
*/
void gpsUartConfig(void){
    //Setting pins
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);   //GPS
    //Configure module
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);                    //GPS
    // Enable UART module
    MAP_UART_enableModule(EUSCI_A2_BASE);                               //GPS
    // Enabling UART interrupts [DEPRECATED]
    // MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
}            

/*!
    @brief GPS UART module DMA initialization
    @details This function initializes the DMA module for GPS communication
    @param none
    @return none
*/
void gpsDMAConfiguration(void){
    MAP_DMA_assignChannel(DMA_CH5_EUSCIA2RX);
    /*! 
        Set DMA chennel for EUSCI_A2 RX
        Set DMA chennel for EUSCI_A2 RX to use Primary DMA Mode sets also: 
		 - 8bit data size 
		 - source address increment is none, (source is fixed)
		 - destination address increment is 8bit (destination is incremented by 1)
		 - arbitration size is 1 (one transfer per request)
    */
    DMA_setChannelControl(DMA_CH5_EUSCIA2RX | UDMA_PRI_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
    

    //Enable DMA interrupts
    MAP_DMA_assignInterrupt(INT_DMA_INT1, 5);               // Assing DMA interrupt 1 to channel 5
    MAP_DMA_clearInterruptFlag(DMA_CH5_EUSCIA2RX & 0x0F);   // Clear interrupt flag for channel 5

    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);            // Enable DMA interrupt
    MAP_DMA_enableInterrupt(INT_DMA_INT1);                  // Enable DMA interrupt 1

    //Enable DMA transfer
    gpsDMARestoreChannel();                                 // Restore DMA channel for reciving next GPS message
}


/*!
    @brief Restore DMA channel for next GPS message
    @details This function reconfugure the DMA channel for re-enabling the DMA transfer
    @param none
    @return none
    @note This function is usefull for re-enabling the DMA transfer after a complete message parsing 
*/
void gpsDMARestoreChannel(void){
    /*!
        Set DMA chennel transfer parameters for EUSCI_A2 RX
        Set DMA chennel for EUSCI_A2 RX to use Primary DMA Mode sets also: 
		 - Basic transfer mode
		 - Source address
		 - Destination address
		 - Transfer size
    */
    DMA_setChannelTransfer(DMA_CH5_EUSCIA2RX | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC,
                               (void*) UART_getReceiveBufferAddressForDMA(EUSCI_A2_BASE),
                               (void*) uartData,
                               RX_BUFFER_SIZE);
    MAP_DMA_enableChannel(5);                               // Enable DMA channel 5                           
}

/*! 
    @brief      DMA completation interrupt handler
	@details    This function is called when DMA transfer is completed
	            so it wakes up the CPU for processing the data by setting the stringEnd flag
*/ 
void DMA_INT1_IRQHandler(void){
	//Set the stringEnd flag
    stringEnd = true;
    // Disable the interrupt to allow execution 
    MAP_Interrupt_disableSleepOnIsrExit();
}
