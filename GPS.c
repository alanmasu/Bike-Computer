/*!
    @file   GPS.c
    @brief  GPS module functions and definitions
    @date   03/01/2024
    @author Alan Masutti
*/

#ifndef SIMULATE_HARDWARE
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

#endif

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef SIMULATE_HARDWARE
    #define PRINTF(...)
#else
    #define PRINTF(...) printf(__VA_ARGS__)
#endif


/* Local Includes*/
#include "GPS.h"
#ifndef SIMULATE_HARDWARE
#include <DMAModule.h>
#endif

volatile uint8_t uartData[RX_BUFFER_SIZE];  //! GPS UART RX buffer
volatile bool stringEnd = false;            //! Flag for end of string

GpsGGAData_t gpsGGAData;                    //! GGA data
GpsRMCData_t gpsRMCData;                    //! RMC data
GpsGSAData_t gpsGSAData;                    //! GSA data
GpsGSVData_t gpsGSVData;                    //! GSV data
GpsVTGData_t gpsVTGData;                    //! VTG data

#ifndef SIMULATE_HARDWARE
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
#endif

bool nmeaChecksumValidate(const char* sentence, char** endOfSentence){
    char* str;
    char checksum[3];
    uint8_t checksumCalculated = 0;
    //Search for '*'
    str = strchr(sentence, '*');

    //If found, validate checksum else return false and set nextSentence to NULL
    if(str != NULL){
        *endOfSentence = strchr(str, '$');
        //Get checksum
        checksum[0] = *(str + 1);
        checksum[1] = *(str + 2);
        checksum[2] = '\0';
        //Calculate checksum
        for(int i = 1; i < str - sentence; ++i){
            checksumCalculated ^= sentence[i];
        }
        //Compare checksums
        if(checksumCalculated == (uint8_t)strtol(checksum, NULL, 16)){
            return true;
        }else{
            return false;
        }
    }
    *endOfSentence = NULL;
    return false;
}

time_t getTimeFromString(char* str){
    char hours[3];
    char minutes[3];
    char seconds[5];
    memcpy(hours,str , 2);
    memcpy(minutes, str + 2, 2);
    memcpy(seconds, str + 4, 2);
    hours[2] = '\0';
    minutes[2] = '\0';
    seconds[4] = '\0';
    return atoi(hours) * 3600 + atoi(minutes) * 60 + atoi(seconds);
}

void gpsParseData(const char* packet){
    char* str;
    char* sentenceType;
    if(stringEnd){
        //Parse data
        //Search for '$'
        str = strchr(packet, '$');
        char* nextSentence = str;
        char* fields[20];
        //If found, validate checksum
        if(str != NULL){
            while(nextSentence != NULL){
                bool valid = nmeaChecksumValidate(nextSentence, &nextSentence);
                PRINTF("Valid: %d\n", valid);
                if(valid){
                    strtok(str, "*");
                    sentenceType = strtok(str, ",");

                    //Get fields
                    int fieldIndex = 0;
                    do{
                        fields[fieldIndex] = strtok(NULL, ",");
                    }while(fields[fieldIndex++] != NULL && fieldIndex < 20);

                    int cmpResult = strcmp(sentenceType, GGA_SENTENCE);
                    if(strcmp(sentenceType, GGA_SENTENCE) == 0){
                        //Parse GGA data
                        gpsGGAData.time = getTimeFromString(fields[0]);
                        float latitude = atof(fields[1]);
                        float longitude = atof(fields[3]);
                        if(fields[2][0] == 'S'){
                            latitude *= -1;
                        }
                        if(fields[4][0] == 'W'){
                            longitude *= -1;
                        }
                        snprintf(gpsGGAData.latitude, 12, "%f", latitude);
                        snprintf(gpsGGAData.longitude, 12, "%f", longitude);
                        //Fix
                        gpsGGAData.fix = atoi(fields[5]);
                        //Satellites
                        strcpy(gpsGGAData.sats, fields[6]);
                        //HDOP
                        strcpy(gpsGGAData.hdop, fields[7]);
                        //Altitude
                        strcpy(gpsGGAData.altitude, fields[8]);
                        //Altitude WSG84
                        strcpy(gpsGGAData.altitude_WSG84, fields[10]);

                        PRINTF("%d\t(%s,\t%s) \tFix:%d \tsats:%s \thdop:%s \talt:%s \taltGeo:%s\n", gpsGGAData.time,
                                                                                                gpsGGAData.latitude,
                                                                                                gpsGGAData.longitude,
                                                                                                gpsGGAData.fix,
                                                                                                gpsGGAData.sats,
                                                                                                gpsGGAData.hdop,
                                                                                                gpsGGAData.altitude,
                                                                                                gpsGGAData.altitude_WSG84);
                    }else if(strcmp(sentenceType, RMC_SENTENCE) == 0){

                    }else if(strcmp(sentenceType, GSA_SENTENCE) == 0){

                    }else if(strcmp(sentenceType, GSV_SENTENCE) == 0){

                    }else if(strcmp(sentenceType, GLL_SENTENCE) == 0){

                    }else if(strcmp(sentenceType, VTG_SENTENCE) == 0){

                    }
                }
                str = nextSentence;
            }
        }
    }
}
