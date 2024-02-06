/*!
    @file       GPX.h
    @ingroup    GPX_Module
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


/*!
    @defgroup   GPX_Module GPX
    @name       GPX Module

    @{
*/
void GPXInitFile(FILE_TYPE file, const char* filename);

void GPXAddTrackName(FILE_TYPE file, const char* name);
void GPXAddTrackType(FILE_TYPE file, const char* type);
void GPXAddTrack(FILE_TYPE file, const char* time);
void GPXAddTrackSegment(FILE_TYPE file);
void GPXAddNewTrackSegment(FILE_TYPE file);

//Point manipulation
void GPXAddTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time);
void GPXOpenTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time);
void GPXAddTempToPoint(FILE_TYPE file, float temp);
void GPXAddCadenceToPoint(FILE_TYPE file, float cadence);
void GPXAddExtensionToPoint(FILE_TYPE file);
void GPXCloseExtension(FILE_TYPE file);
void GPXCloseTrackPoint(FILE_TYPE file);
void GPXAddCompleteTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time, float temp, float cadence);

void GPXCloseTrackSegment(FILE_TYPE file);
void GPXCloseTrack(FILE_TYPE file);
void GPXCloseFile(FILE_TYPE file);

/// @}
//Future implementation
//void GPXAddWayPoint(FILE_TYPE file, double lat, double lon, double ele, const char* time);



#endif // __GPX_H__
