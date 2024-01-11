/*!
    @file       main.c
    @brief      This file merge the functionalities of the SD-Part whit the GPS part.
    @details    The main function configure all hardware dipendent patr, start a comunication channel whit the L80 module on UART
                then start the SD card and mount the fatfs file system. After that the main function sample the GPS data and save
                it on the SD card in a file called "test.gpx" in the root directory. <br>

                For comunication whit the PC uses EUSCI_A0 and MSPIO library. <br>

                Hardware part: <br>
                @code
                        MSP432P401
                  -------------------
                  |                 |
             RST -|     P3.3/UCA0TXD|----> PC RX at 115200 8N1\
                  |                 |                          >  =>  (XDS110 USB to UART)
                  |     P3.2/UCA0RXD|----< PC TX at 115200 8N1/
                  |                 |
                  |     P3.5/UCA2TXD|----> GPS RX at 9600 8N1
                  |     P3.6/UCA2RXD|----< GPS TX at 9600 8N1
                  |                 |
                  |   P1.7/UCAB0MISO|----> SD SPI MISO
                  |   P1.6/UCAB0MOSI|----< SD SPI MOSI
                  |   P1.5/UCAB0SCLK|----< SD SPI CLK
                  |             P5.2|----> SD SPI CS
                  |                 |
                  |             P5.1|----< BTN Start
                  |             P3.5|----< BTN Stop
                  -------------------
             @endcode
 	@date       10/01/2024
    @author     Alan Masutti
 */

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
    #include <DMAModule.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
#endif

//Standard includes
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//Testing includes
#include "Test/GPX_Points.h"


//Local includes
//GPX
#include "GPX.h"
//GPS
#include "GPS.h"

#ifndef SIMULATE_HARDWARE
    #include <Devices/MSPIO.h>
    #define PRINTF(...) MSPrintf(EUSCI_A0_BASE ,__VA_ARGS__)
#else
    #include <stdio.h>
    #define PRINTF(...) printf(__VA_ARGS__)
#endif

typedef enum{STOP = 0, START = 1} ComputerState_t;

ComputerState_t computerState = STOP;

#ifndef SIMULATE_HARDWARE

//Buttons defines
#define BTN_START_PORT      GPIO_PORT_P5
#define BTN_START_PIN       GPIO_PIN1
#define BTN_STOP_PORT       GPIO_PORT_P3
#define BTN_STOP_PIN        GPIO_PIN5

#define GPX_TEST_FILENAME   "test.gpx"
FIL file;
#define GPX_TEST_FILE       file

/*!
    @brief     UART Configuration Parameter.
    @details   These are the configuration parameters to
                make the eUSCI A UART module to operate with a 115200 baud rate. These
                values were calculated using the online calculator that TI provides
                at: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
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

/*!
    @brief      SPI Configuration Parameter.
    @details    These are the configuration parameters to
                make the eUSCI B SPI module to operate with a 500KHz clock.
*/
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

bool btnStartStateP = true;
bool btnStopStateP = true;


void main(void){
    WDT_A_holdTimer();	// stop watchdog timer
	CS_Init();

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B0_BASE, SPI0MasterConfig);
    UART_Init(EUSCI_A0_BASE, UART0Config);
    SD_Init();

    /*Initialize all hardware required for the GPS*/
    /* Configuring UART Modules */
	//Enable GPS UART
	gpsUartConfig();
    //Enable DMA module
	dmaInit();
	//Enable DMA for EUSCI_A2 RX
	gpsDMAConfiguration();

    Interrupt_enableMaster();

    FRESULT r;

    /*First we should mount the SD Card into the Fatfs file system*/
    r = f_mount(&FS, "0", 1);
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK){
        PRINTF("Error mounting SD Card, mount function returned: %d \r\n", (int)r);
    }else{
        PRINTF("Mounted SD Card!\r\n");
    }


    /*Let's try to open the root directory on the SD Card*/
    r = f_opendir(&DI, "/");
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK){
        PRINTF("Could not open root directory, returned: %d\r\n", (int)r);
        while(1);
    }else{
        PRINTF("Opened DIR!\r\n");
    }

    //Configuring GPIO for buttons
    MAP_GPIO_setAsInputPin(BTN_START_PORT, BTN_START_PIN);
    MAP_GPIO_setAsInputPin(BTN_STOP_PORT, BTN_STOP_PIN);

    //testing SD and GPX features

//    f_unmount("");
    while(1){
        bool status;
        switch (computerState){
        case STOP:
            //Detecting Start button press
            status = !MAP_GPIO_getInputPinValue(BTN_START_PORT, BTN_START_PIN);
            if(status && !btnStartStateP){
                btnStartStateP = true;
                //Open the file
//                int fileIndex = 1;
//                r = f_stat(GPX_TEST_FILENAME, &FI);                     //Check if file already exists
//                if(r == FR_OK){                                         //If file already exists
//                    char newFileName[15];
//                    do{
//                        snprintf(newFileName, 14, "%test%d.gpx", fileIndex);
//                        fileIndex++;
//                        r = f_stat(newFileName, &FI);
//                    }while(r != FR_NO_FILE && fileIndex <= 999);
//                    r = f_open(&GPX_TEST_FILE, newFileName, FA_WRITE | FA_CREATE_ALWAYS);
//                }else if(r == FR_NO_FILE){
//                    r = f_open(&GPX_TEST_FILE, GPX_TEST_FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
//                }
                r = f_open(&GPX_TEST_FILE, GPX_TEST_FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
                /*Check for errors. Trap MSP432 if there is an error*/
                if(r != FR_OK){
                    PRINTF("Could not open file, returned: %d\r\n", (int)r);
                    while(1);
                }
                GPXInitFile(&GPX_TEST_FILE, GPX_TEST_FILENAME);
                GPXAddTrack(&GPX_TEST_FILE, "Test Track", "Test Description", "2024-01-10T00:00:00Z");
                GPXAddTrackSegment(&GPX_TEST_FILE);
                computerState = START;
                PRINTF("START TRACKING!!\r\n");
            }
            btnStartStateP = status;
            break;
        case START:
            //Detecting Stop button press
//            status = !MAP_GPIO_getInputPinValue(BTN_STOP_PORT, BTN_STOP_PIN);
//            if(status && !btnStopStateP){
            if(status){
                btnStopStateP = true;
                GPXCloseTrackSegment(&GPX_TEST_FILE);
                GPXCloseTrack(&GPX_TEST_FILE);
                GPXCloseFile(&GPX_TEST_FILE);
                computerState = STOP;
                PRINTF("STOP TRACKING!!\r\n");
            }
            if(stringEnd){
                addPointToGPXFromGPS(&uartData, &GPX_TEST_FILE);
                stringEnd = false;
                gpsDMARestoreChannel();
                //Go to sleep
            }
            MAP_Interrupt_enableSleepOnIsrExit();
            MAP_PCM_gotoLPM0InterruptSafe();
            btnStopStateP = status;
            break;
        }

    }
}
#else

#define GPX_TEST_FILENAME   "Test/results/test.gpx"
#define GPX_TEST_FILE       file
int main(void){
    FILE* NMEA;
    FILE* GPX;
    NMEA = fopen("Test/NMEAFileCorrected.txt", "r");
    if(NMEA == NULL){
        printf("Error opening NMEA file!\r\n");
        return -1;
    }
    GPX = fopen(GPX_TEST_FILENAME, "w");
    do{
        PRINTF("Get command: P/s [Play/Stop]\r\n");
        fflush(stdin);
        char cmd = getchar();
        switch(cmd){
            case 'P':
            case 'p':
                PRINTF("Start!\r\n");
                GPXInitFile(GPX, GPX_TEST_FILENAME);
                GPXAddTrack(GPX, "Test Track", "Test Description", "2024-01-10T00:00:00Z");
                GPXAddTrackSegment(GPX);
                computerState = START;
                break;
            case 'C':
            case 'c':
                PRINTF("Continue!\r\n");
                break;
            case 'S':
            case 's':
                PRINTF("Stop!\r\n");
                GPXCloseTrackSegment(&GPX);
                GPXCloseTrack(&GPX);
                GPXCloseFile(&GPX);
                computerState = STOP;
                break;
        }
        if(computerState == START){
            char gpsData[RX_BUFFER_SIZE];
            fread(gpsData, sizeof(char), RX_BUFFER_SIZE, NMEA);
            stringEnd = true;
            addPointToGPXFromGPS(gpsData, &GPX);
        }
    }while(computerState != STOP);
	return 0;
}
#endif
