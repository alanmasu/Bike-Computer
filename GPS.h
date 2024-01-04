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

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define RX_BUFFER_SIZE 256                  //! Size of RX buffer
                                            //! Uesed also by DMA as max buffer length

typedef enum {INVALID, GPS_FIX, DGPS, GPS_PPS, IRTK, FRTK, DEAD_RECKONING, MANUAL, SIMULATED} FixData_t;
typedef struct{
    char latitude[12];                      //! Latitude
    char longitude[12];                     //! Longitude
    char altitude[8];                       //! Altitude
    char altitude_WSG84[8];                 //! Altitude
    char speed[8];                          //! Speed
    char course[8];                         //! Course
    struct tm timeInfo;                     //! Time info
    char sats[4];                           //! Satellites
    FixData_t fix;                          //! Fix
    char hdop[4];                           //! HDOP
    char vdop[4];                           //! VDOP
    char pdop[4];                           //! PDOP
} GpsGGAData_t;

extern volatile uint8_t uartData[RX_BUFFER_SIZE];
extern volatile bool stringEnd;


void gpsUartConfig(void);                   
void gpsDMAConfiguration(void);
void gpsDMARestoreChannel(void);

void gpsParseData(); 


#endif // __GPS_H__
