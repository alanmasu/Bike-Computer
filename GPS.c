/*!
    @file   GPS.c
    @ingroup GPS_Module
    @brief  GPS module function implementations
    @date   10/01/2024
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
#include <time.h>

#ifndef PRINTF
    #ifndef SIMULATE_HARDWARE
        #if !defined(DEBUG) || defined(STAND_ALONE)
            #define PRINTF(...)
        #else
            #include <Devices/MSPIO.h>
            #define PRINTF(...) MSPrintf(EUSCI_A0_BASE ,__VA_ARGS__)
        #endif
    #else
        #include <stdio.h>
        #define PRINTF(...) printf(__VA_ARGS__)
    #endif
#endif
/*!
    @addtogroup GPS_Module
    @{
        @brief      GPS module functions
        @details    This module contains the functions used to configure and use the GPS module.
                    There are functions dipendent on hardware and functions that can be used also.
*/

/* Local Includes*/
#include "GPS.h"
#include "GPX.h"
#ifndef SIMULATE_HARDWARE
#include <DMAModule.h>
#endif

volatile uint8_t gpsUartBuffer[RX_BUFFER_SIZE];  //!< GPS UART RX buffer
volatile bool gpsStringEnd = false;            //!< Flag for end of string

GpsGGAData_t gpsGGAData;                    //!< GGA data
GpsRMCData_t gpsRMCData;                    //!< RMC data
GpsGSAData_t gpsGSAData;                    //!< GSA data
GpsGSVData_t gpsGSVData;                    //!< GSV data
GpsVTGData_t gpsVTGData;                    //!< VTG data

//GPX Configuration
static bool useTemp = true;                 //!< Use temperature flag
static bool useCad = false;                 //!< Use cadence flag

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
                               (void*) gpsUartBuffer,
                               RX_BUFFER_SIZE);
    MAP_DMA_enableChannel(5);                               // Enable DMA channel 5
}

/*!
    @brief      DMA completation interrupt handler
	@details    This function is called when DMA transfer is completed
	            so it wakes up the CPU for processing the data by setting the gpsStringEnd flag
*/
void DMA_INT1_IRQHandler(void){
	//Set the gpsStringEnd flag
    gpsStringEnd = true;
    // Disable the interrupt to allow execution
    MAP_Interrupt_disableSleepOnIsrExit();
}

#endif

/*!
    @brief      Validate NMEA checksum
    @details    This function validates the checksum of a NMEA sentence
    @param[in]  sentence: NMEA sentence
    @param[out] nextSentence: Pointer to the next sentence founded, if returns NULL there is no next sentence
    @return     true if the checksum is valid, false otherwise
*/

bool nmeaChecksumValidate(const char* sentence, char** nextSentence){
    char* str;
    char checksum[3];
    uint8_t checksumCalculated = 0;
    //Search for '*'
    str = strchr(sentence, '*');

    //If found, validate checksum else return false and set nextSentence to NULL
    if(str != NULL){
        *nextSentence = strchr(str, '$');
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
    *nextSentence = NULL;
    return false;
}

/*!
    @brief    Get time from string
    @details  This function gets the time from a string
    @param    str: String to get the time
    @return   Time in seconds
    @note     The string must be in the format HHMMSS<.SSS> where <.SSS> is optional
*/
time_t getTimeFromString(const char* str){
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

/*!
    @brief    Get date from string
    @details  This function gets the date from a string
    @param    time: String to get the time
    @param    date: String to get the date
    @return   Date in struct tm format
    @note     The time string must be in the format HHMMSS<.SSS> where <.SSS> is optional
              The date string must be in the format DDMMYY
*/
struct tm getDateFromString(const char* time, const char* date){
    char hours[3];
    char minutes[3];
    char seconds[5];
    memcpy(hours,time , 2);
    memcpy(minutes, time + 2, 2);
    memcpy(seconds, time + 4, 2);
    hours[2] = '\0';
    minutes[2] = '\0';
    seconds[4] = '\0';
    char day[3];
    char month[3];
    char year[3];
    memcpy(day,date , 2);
    memcpy(month, date + 2, 2);
    memcpy(year, date + 4, 2);
    day[2] = '\0';
    month[2] = '\0';
    year[2] = '\0';
    return (struct tm){.tm_mday = atoi(day),
                       .tm_mon = atoi(month) - 1,
                       .tm_year = atoi(year) + 100,
                       .tm_hour = atoi(hours),
                       .tm_min = atoi(minutes),
                       .tm_sec = atoi(seconds)
                       };
}

/*!
    @brief    Get latitude from string
    @details  This function gets the latitude from a string
    @param    str: String to get the latitude
    @return   Latitude in decimal degrees
    @note     The string must be in the format DDMM.MMMM
*/
float getLatitudeFromString(char* str){
    char degrees[3];
    char minutes[9];
    memcpy(degrees, str, 2);
    memcpy(minutes, str + 2, 8);
    degrees[2] = '\0';
    minutes[7] = '\0';
    return atof(degrees) + atof(minutes) / 60;
}

/*!
    @brief    Get longitude from string
    @details  This function gets the longitude from a string
    @param    str: String to get the longitude
    @return   Longitude in decimal degrees
    @note     The string must be in the format DDDMM.MMMM
*/
float getLongitudeFromString(char* str){
    char degrees[4];
    char minutes[9];
    memcpy(degrees, str, 3);
    memcpy(minutes, str + 3, 8);
    degrees[3] = '\0';
    minutes[7] = '\0';
    return atof(degrees) + atof(minutes) / 60;
}

/*!
    @brief      Split string inplace
    @details    This function splits a string in place using a delimiter,
                the string is modified replacing the first delimiter with '\0' and the next
                string is returned using the next pointer
    @param[in]  str: String to split
    @param[in]  delim: Delimiter
    @param[out] next: Pointer to the next string.
                 - if NULL is recieved back there is no next string,
                 - f NULL is passed the next pointer will be discarded.
    @return     Pointer to the first characharacter of the substring
*/

char* splitString(char* str, char delim, char** next){
    if(str != NULL){
        char* token = strchr(str, delim);
        if(token != NULL){
            token[0] = '\0';
            if(next != NULL){
                *next = token + 1;
            }
        }else{
            *next = NULL;
        }
    }
    return str;
}

/*!
    @brief    Parse GPS data
    @details  This function parses the GPS data
    @param    packet: GPS NMEA data
*/
void gpsParseData(const char* packet){
    if(packet == NULL) return;
    char* str;
    char* sentenceType;
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
                char* nextField = str;
                sentenceType = splitString(nextField, ',', &nextField);

                //Get fields
                int fieldIndex = 0;
                do{
                    fields[fieldIndex] = splitString(nextField, ',', &nextField);
                }while(fields[fieldIndex++] != NULL && fieldIndex < 20);

                int cmpResult = strcmp(sentenceType, RMC_SENTENCE);
                if(strcmp(sentenceType, GGA_SENTENCE) == 0){
                    //Parse GGA data
                    gpsGGAData.time = getTimeFromString(fields[0]);
                    float latitude = getLatitudeFromString(fields[1]);
                    float longitude = getLongitudeFromString(fields[3]);
                    if(fields[2][0] == 'S'){
                        latitude *= -1;
                    }
                    if(fields[4][0] == 'W'){
                        longitude *= -1;
                    }
                    snprintf(gpsGGAData.latitude, 12, "%f", latitude);
                    snprintf(gpsGGAData.longitude, 12, "%f", longitude);
                    //Fix
                    gpsGGAData.fix = (GGAFixData_t)atoi(fields[5]);
                    //Satellites
                    strcpy(gpsGGAData.sats, fields[6]);
                    //HDOP
                    strcpy(gpsGGAData.hdop, fields[7]);
                    //Altitude
                    strcpy(gpsGGAData.altitude, fields[8]);
                    //Altitude WSG84
                    strcpy(gpsGGAData.altitude_WSG84, fields[10]);

                    PRINTF("%d\t(%s,\t%s) \tFix:%d \tsats:%s \thdop:%s \talt:%s \taltGeo:%s\n\n", gpsGGAData.time,
                                                                                            gpsGGAData.latitude,
                                                                                            gpsGGAData.longitude,
                                                                                            gpsGGAData.fix,
                                                                                            gpsGGAData.sats,
                                                                                            gpsGGAData.hdop,
                                                                                            gpsGGAData.altitude,
                                                                                            gpsGGAData.altitude_WSG84);
                }else if(strcmp(sentenceType, RMC_SENTENCE) == 0){
                    //Parse RMC data
                    float latitude = getLatitudeFromString(fields[2]);
                    float longitude = getLongitudeFromString(fields[4]);
                    if(fields[3][0] == 'S'){
                        latitude *= -1;
                    }
                    if(fields[5][0] == 'W'){
                        longitude *= -1;
                    }
                    snprintf(gpsRMCData.latitude, 12, "%f", latitude);
                    snprintf(gpsRMCData.longitude, 12, "%f", longitude);
                    //Valid
                    gpsRMCData.valid = fields[1][0] == 'A';
                    //Speed
                    strcpy(gpsRMCData.speed, fields[6]);
                    //Course
                    strcpy(gpsRMCData.course, fields[7]);
                    //Date
                    gpsRMCData.timeInfo = getDateFromString(fields[0], fields[8]);
                    //Others
                    strcpy(gpsRMCData.others, fields[9]);

                    PRINTF("(%s,\t%s) \tValid:%d \tspeed:%s \tcourse:%s \tdate:%d/%d/%d \tothers:%s\n\n",   gpsRMCData.latitude,
                                                                                                            gpsRMCData.longitude,
                                                                                                            gpsRMCData.valid,
                                                                                                            gpsRMCData.speed,
                                                                                                            gpsRMCData.course,
                                                                                                            gpsRMCData.timeInfo.tm_mday,
                                                                                                            gpsRMCData.timeInfo.tm_mon+1,
                                                                                                            gpsRMCData.timeInfo.tm_year+1900,
                                                                                                            gpsRMCData.others);
                }else if(strcmp(sentenceType, GSA_SENTENCE) == 0){
                    //Mode
                    strcpy(gpsGSAData.mode, fields[0]);
                    //Fix
                    strcpy(gpsGSAData.fix, fields[1]);
                    //Satellites
                    int i;
                    for(i = 0; i < 12; ++i){
                        if(*fields[2 + i] == '\0'){
                            gpsGSAData.sats[i] = -1;
                        }else{
                            gpsGSAData.sats[i] = (int8_t)atoi(fields[2 + i]);
                        }
                    }
                    //PDOP
                    i+=2;
                    strcpy(gpsGSAData.pdop, fields[i]);
                    //HDOP
                    strcpy(gpsGSAData.hdop, fields[i+1]);
                    //VDOP
                    strcpy(gpsGSAData.vdop, fields[i+2]);

                    PRINTF("Mode:%s \tFix:%s \tPDOP:%s \tHDOP:%s \tVDOP:%s\n",  gpsGSAData.mode,
                                                                                    gpsGSAData.fix,
                                                                                    gpsGSAData.pdop,
                                                                                    gpsGSAData.hdop,
                                                                                    gpsGSAData.vdop);
                    PRINTF("Sats: \n");
                    for(int i = 0; gpsGSAData.sats[i] != -1 && i < 12; ++i){
                        PRINTF("\t%d ", gpsGSAData.sats[i]);
                    }
                    PRINTF("\n\n");
                }else if(strcmp(sentenceType, GSV_SENTENCE) == 0){
                    //Satellites in view
                    uint8_t satCount = (uint8_t)atoi(fields[2]);
                    uint8_t mgsIndex = (uint8_t)atoi(fields[1]);
                    for(uint8_t i = (mgsIndex-1)*4, f = 2; i < (mgsIndex-1)*4+4 && i < satCount; ++i, f+=4){
                        //Satellite ID
                        if(fields[f][0] == 0) break;
                        strcpy(gpsGSVData.sats[i].id, fields[f]);
                        //Elevation
                        if(fields[f + 1][0] == 0) break;
                        strcpy(gpsGSVData.sats[i].elevation, fields[f+1]);
                        //Azimuth
                        if(fields[f + 2][0] == 0) break;
                        strcpy(gpsGSVData.sats[i].azimuth, fields[f+2]);
                        //SNR
                        if(fields[f + 3][0] == 0) break;
                        strcpy(gpsGSVData.sats[i].snr, fields[f+3]);
                    }
                    if(mgsIndex == 1){
                        PRINTF("Satellites in view: %s\n", gpsGSVData.satsInView);
                    }
                    PRINTF("Msg ID: %d\n", mgsIndex);
                    for(uint8_t i = (mgsIndex-1)*4; i < (mgsIndex-1)*4+4; ++i){
                        PRINTF("\tSatellite ID: %s\n", gpsGSVData.sats[i].id);
                        PRINTF("\t\tElevation: %s\n", gpsGSVData.sats[i].elevation);
                        PRINTF("\t\tAzimuth: %s\n", gpsGSVData.sats[i].azimuth);
                        PRINTF("\t\tSNR: %s\n", gpsGSVData.sats[i].snr);
                    }
                // }else if(strcmp(sentenceType, GLL_SENTENCE) == 0){

                }else if(strcmp(sentenceType, VTG_SENTENCE) == 0){
                    //Course
                    strcpy(gpsVTGData.course, fields[0]);
                    //Reference
                    strcpy(gpsVTGData.courseM, fields[2]);
                    //Speed in knots
                    strcpy(gpsVTGData.speedK, fields[4]);
                    //Speed in km/h
                    strcpy(gpsVTGData.speed, fields[6]);

                    PRINTF("Course:%s \tReference:%s \tSpeed in knots:%s \tSpeed in km/h:%s\n\n", gpsVTGData.course,
                                                                                                gpsVTGData.courseM,
                                                                                                gpsVTGData.speedK,
                                                                                                gpsVTGData.speed);
                }
            }
            str = nextSentence;
        }
    }
}

void getGpsData(int* sats, float* gpsSpeed, float* gpsAltitude, float* gpsHdop){
    *sats = atoi(gpsGGAData.sats);
    *gpsAltitude = atof(gpsGGAData.altitude);

    *gpsSpeed = atof(gpsVTGData.speed);
    *gpsHdop = atof(gpsGGAData.hdop);
}

GpsGGAData_t* getGGAData(void){
    return &gpsGGAData;
}

GpsRMCData_t* getRMCData(void){
    return &gpsRMCData;
}

GpsGSAData_t* getGSAData(void){
    return &gpsGSAData;
}
GpsGSVData_t* getGSVData(void){
    return &gpsGSVData;
}
GpsVTGData_t* getVTGData(void){
    return &gpsVTGData;
}

//Setter functions

void useTemperature(bool use){
    useTemp = use;
}

void useCadence(bool use){
    useCad = use;
}

/*!
    @brief    Add point to GPX file from GPS data
    @details  This function adds a point to a GPX file from GPS data
    @param    file: GPX file
    @return   true if the point was added, false otherwise
*/
bool addPointToGPXFromGPS(FILE_TYPE file, float temp, float cadence){
    static bool fixOk = false;
    float hdop = atof(gpsGGAData.hdop);
    int fix = atoi(gpsGSAData.fix);
    if(fix > 1 && gpsRMCData.valid && hdop < 4){
        fixOk = true;
        char timeString[20];
        //Convert time to string ISO 8601
        snprintf(timeString, 20, "%d-%02d-%02dT%02d:%02d:%02dZ", gpsRMCData.timeInfo.tm_year + 1900,
                                                                gpsRMCData.timeInfo.tm_mon + 1,
                                                                gpsRMCData.timeInfo.tm_mday,
                                                                gpsRMCData.timeInfo.tm_hour,
                                                                gpsRMCData.timeInfo.tm_min,
                                                                gpsRMCData.timeInfo.tm_sec);
        if(useTemp && useCad){
            GPXAddCompleteTrackPoint(file, gpsGGAData.latitude, gpsGGAData.longitude, gpsGGAData.altitude, timeString, temp, cadence);
            return true;
        }else if(useTemp){
            GPXOpenTrackPoint(file, gpsGGAData.latitude, gpsGGAData.longitude, gpsGGAData.altitude, timeString);
            GPXAddExtensionToPoint(file);
            GPXAddTempToPoint(file, temp);
            GPXCloseExtension(file);
            GPXCloseTrackPoint(file);
            return true;
        }else if(useCad){
            GPXOpenTrackPoint(file, gpsGGAData.latitude, gpsGGAData.longitude, gpsGGAData.altitude, timeString);
            GPXAddExtensionToPoint(file);
            GPXAddCadenceToPoint(file, cadence);
            GPXCloseExtension(file);
            GPXCloseTrackPoint(file);
            return true;
        }
        GPXAddTrackPoint(file, gpsGGAData.latitude, gpsGGAData.longitude, gpsGGAData.altitude, timeString);
        return true;
    }else{
        PRINTF("Point Not Added because GPS has no valid FIX!\n");
        if(fixOk){
            GPXAddNewTrackSegment(file);
            fixOk = false;
        }
        return false;
    }
}

/*! @} */ // GPS_Module
