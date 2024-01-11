/*!
    @file       GPX.h
    @brief      GPX file creation and manipulation
    @details    This file contains the function definitions for the GPX file creation and manipulation
                functions. These functions are used to create a GPX file and add track points to it.
                The GPX file is used to store the GPS data from the GPS module.
    @author     Alan Masutti
    @date       08/01/2024
    @see        GPX.c for implementation

*/
#ifndef __GPX_H__
#define __GPX_H__

#ifndef SIMULATE_HARDWARE
    #include <fatfs/ff.h>
    #include <fatfs/diskio.h>
    #define FILE_TYPE FIL*      //! Definition for file handler type in the MSP432 version

#else
    #include <stdio.h>
    #define FILE_TYPE FILE**    //! Definition for file handler type in the PC version

#endif
#include <string.h>


/// @{
void GPXInitFile(FILE_TYPE file, const char* filename);


void GPXAddTrack(FILE_TYPE file, const char* trackName, const char* trackDesc, const char* time);
void GPXAddTrackSegment(FILE_TYPE file);
void GPXAddNewTrackSegment(FILE_TYPE file);

void GPXAddTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time);
void GPXCloseTrackSegment(FILE_TYPE file);
void GPXCloseTrack(FILE_TYPE file);
void GPXCloseFile(FILE_TYPE file);

/// @}
//Future implementation
//void GPXAddWayPoint(FILE_TYPE file, double lat, double lon, double ele, const char* time);



#endif // __GPX_H__
