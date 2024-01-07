/*! @file main.c
 *  @brief Main function for the program
 *	@details This file contains the main function for the program
 *	@version 1.0
 *	@date 03/01/2024
 */
// #define SIMULATE_HARDWARE

#ifndef SIMULATE_HARDWARE
	#include <ti/devices/msp432p4xx/inc/msp.h>
	#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

	//SD
	#include <Hardware/SPI_Driver.h>
	#include <Hardware/GPIO_Driver.h>
	#include <Hardware/CS_Driver.h>
	#include <Hardware/TIMERA_Driver.h>
	#include <Hardware/SD_Driver.h>
	#include <fatfs/ff.h>
	#include <fatfs/diskio.h>
	#include <Devices/MSPIO.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
#endif

//Standard includes
#include <string.h>

//Testing includes
#include "Test/GPX_Points.h"

//GPX
#include "GPX.h"

#ifndef SIMULATE_HARDWARE

#define GPX_TEST_FILE "test.gpx"

/*!
 * @brief UART Configuration Parameter.
 * @details These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART0Config = {
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/* SPI Configuration Parameter. These are the configuration parameters to
 * make the eUSCI B SPI module to operate with a 500KHz clock.*/
eUSCI_SPI_MasterConfig SPI0MasterConfig = {
     EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
     3000000,
     500000,
     EUSCI_B_SPI_MSB_FIRST,
     EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
     EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
     EUSCI_B_SPI_3PIN
};

FATFS FS;
DIR DI;
FILINFO FI;
FIL file;

void main(void){
    WDT_A_holdTimer();	// stop watchdog timer
	CS_Init();

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B0_BASE, SPI0MasterConfig);
    UART_Init(EUSCI_A0_BASE, UART0Config);
    SD_Init();

    Interrupt_enableMaster();

    FRESULT r;

    /*First we should mount the SD Card into the Fatfs file system*/
    r = f_mount(&FS, "0", 1);
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK){
        MSPrintf(EUSCI_A0_BASE, "Error mounting SD Card, mount function returned: %d \r\n", r);
    }


    /*Let's try to open the root directory on the SD Card*/
    r = f_opendir(&DI, "/");
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK){
        MSPrintf(EUSCI_A0_BASE, "Could not open root directory, returned: %d\r\n", r);
        while(1);
    }

    //testing SD and GPX features
    GPXInitFile(GPX_TEST_FILE);
    MSPrintf(EUSCI_A0_BASE, "Initialized GPX\r\n");
    GPXAddTrack(GPX_TEST_FILE, "Test Track", "Test Description", "2024-01-03T00:00:00Z");
    MSPrintf(EUSCI_A0_BASE, "Initialized trak\r\n");
    GPXAddTrackSegment(GPX_TEST_FILE);
    MSPrintf(EUSCI_A0_BASE, "Initialized segment\r\n");
    for(int i = 0; i < CoordinateArray_size; i++){
        GPXAddTrackPoint(GPX_TEST_FILE, CoordinateArray[i].latitude, CoordinateArray[i].longitude, CoordinateArray[i].altitude, CoordinateArray[i].time);
//        for(int d = 0; d < 100000; ++d);
    }
    MSPrintf(EUSCI_A0_BASE, "Added points in: %f sec\r\n");
    GPXCloseTrackSegment(GPX_TEST_FILE);
    MSPrintf(EUSCI_A0_BASE, "Closet segment GPX\r\n");
    GPXCloseTrack(GPX_TEST_FILE);
    MSPrintf(EUSCI_A0_BASE, "Closed track \r\n");
    GPXCloseFile(GPX_TEST_FILE);
    MSPrintf(EUSCI_A0_BASE, "Closed GPX\r\n");
//    f_unmount("");
    while(1);
}
#else

#define GPX_TEST_FILE "Test/results/test.gpx"
int main(void){
	GPXInitFile(GPX_TEST_FILE);
    GPXAddTrack(GPX_TEST_FILE, "Test Track", "Test Description", "2024-01-03T00:00:00Z");
    GPXAddTrackSegment(GPX_TEST_FILE);
    for(int i = 0; i < CoordinateArray_size; i++){
        GPXAddTrackPoint(GPX_TEST_FILE, CoordinateArray[i].latitude, CoordinateArray[i].longitude, CoordinateArray[i].altitude, CoordinateArray[i].time);
    }
    GPXCloseTrackSegment(GPX_TEST_FILE);
    GPXCloseTrack(GPX_TEST_FILE);
    GPXCloseFile(GPX_TEST_FILE);
	return 0;
}
#endif
