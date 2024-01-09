/*!
    @file GPS.h
    @brief GPS module header file
    @details This file contains the GPS module functions prototypes and some definitions.
        In hardware the connections are:
    @code
            MSP432P401
         -----------------
        |                 |
   RST -|     P3.3/UCA0TXD|----> GPS_L80 RX at 9600 8N1
        |                 |
       -|                 |
        |     P3.2/UCA0RXD|----< GPS_L80 TX at 9600 8N1
        |                 |
        |             P1.0|----> LED
        |                 |
         -----------------
    @endcode
    @date 03/01/2024
    @author Alan Masutti
    @note This file have some functions thas uses the DMA so it is necessary to share the DMA module
          if needed some where else.
*/

#ifndef __GPS_H__
#define __GPS_H__

#ifndef SIMULATE_HARDWARE
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

#endif

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

//GPS Sentence constants
#define GGA_SENTENCE "$GPGGA"               //! GGA sentence
#define RMC_SENTENCE "$GPRMC"               //! RMC sentence
#define GSA_SENTENCE "$GPGSA"               //! GSA sentence
#define GSV_SENTENCE "$GPGSV"               //! GSV sentence
#define GLL_SENTENCE "$GPGLL"               //! GLL sentence
#define VTG_SENTENCE "$GPVTG"               //! VTG sentence

#define RX_BUFFER_SIZE 512                  //! Size of RX buffer
                                            //! Uesed also by DMA as max buffer length

//GGA fix data
typedef enum {INVALID = 0, GPS_FIX, DGPS, GPS_PPS, IRTK, FRTK, DEAD_RECKONING, MANUAL, SIMULATED} GGAFixData_t;

//GGA data
typedef struct{
    time_t time;                            //! Time
    char latitude[12];                      //! Latitude
    char longitude[12];                     //! Longitude
    GGAFixData_t fix;                       //! Fix
    char sats[4];                           //! Satellites
    char hdop[6];                           //! HDOP
    char altitude[8];                       //! Altitude
    char altitude_WSG84[8];                 //! Altitude
} GpsGGAData_t;

//RMC data
typedef struct{
    bool valid;                             //! Valid data
    char latitude[12];                      //! Latitude
    char longitude[12];                     //! Longitude
    char altitude[8];                       //! Altitude
    char speed[8];                          //! Speed
    char course[8];                         //! Course
    struct tm timeInfo;                     //! Time info
    char others[6];                         //! Others fields ()
} GpsRMCData_t;

//GSA data
typedef struct{
    char mode[2];                           //! Mode
    char fix[2];                            //! Fix
    int8_t sats[13];                        //! Satellites
    char pdop[4];                           //! PDOP
    char hdop[4];                           //! HDOP
    char vdop[4];                           //! VDOP
} GpsGSAData_t;

//GSV data
typedef struct {
    char id[2];                             //! Satellite ID
    char elevation[3];                      //! Elevation
    char azimuth[3];                        //! Azimuth
    char snr[3];                            //! SNR
} SatData_t;

//GSV data
typedef struct{
    char satsInView[4];                     //! Satellites in view
    SatData_t sats[12];                     //! Satellites
} GpsGSVData_t;

//VTG data
typedef struct{
    char course[8];                         //! Course
    char courseM[2];                        //! Reference
    char speedK[8];                         //! Speed in knots
    char speed[8];                          //! Speed in km/h
} GpsVTGData_t;


extern volatile uint8_t uartData[RX_BUFFER_SIZE];
extern volatile bool stringEnd;

#ifndef SIMULATE_HARDWARE
void gpsUartConfig(void);
void gpsDMAConfiguration(void);
void gpsDMARestoreChannel(void);
#endif

void gpsParseData(const char* packet);


#endif // __GPS_H__
