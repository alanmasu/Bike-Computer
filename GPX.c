/*!
    @file       GPX.c
    @brief      GPX file creation and manipulation
    @details    This file contains the function implementation for the GPX file creation and manipulation
                functions.
    @author     Alan Masutti
    @date       08/01/2024

*/
#include "GPX.h"
#include <stdio.h>

/*!
    @brief      GPX Header Constant String
    @details    Constant string containing the GPX header, it is used to:
                    - Initialize the GPX file
                    - Declare the GPX namespace
                    - Declare the GPX version
                    - Declare the GPX creator
                    - Declare the GPX schema location
*/
const char* GPX_HEADER = "\
<?xml version='1.0' encoding='UTF-8'?>\n\
<gpx version=\"1.1\" creator=\"IoTProject2023\" xmlns=\"http://www.topografix.com/GPX/1/1\" \
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";


/*!
    @brief      GPX Track Point Constant String
    @details    Constant string containing the GPX track point schema and placeholders for point data;
                it is used to add a track point to the GPX file
*/
const char* GPX_TRACK_POINT = "\
            <trkpt lat=\"%s\" lon=\"%s\">\n\
                <ele>%s</ele>\n\
                <time>%s</time>\n\
            </trkpt>\n";
            
///@{
///    @name    GPX File Manipulation Functions
///    @brief   Functions used to create and manipulate GPX files

/*!

    @brief      GPXInitFile
    @details    Initializes a GPX file with the given filename and populates the handler,
                it also adds the header to the file.
    @param      file: Pointer to the file handler
    @param      filename: Name of the file to be created

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
*/

void GPXInitFile(FILE_TYPE file, const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        r = f_open(file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if(r != FR_OK){
            return;
        }
        f_printf(file, "%s", GPX_HEADER);
    #else
        *file = fopen(filename, "w");
        if(*file == NULL){
            return;
            printf("Error opening file!\n");
        }
        fprintf(*file, GPX_HEADER);
    #endif
}

/*!
    @brief      GPXAddTrack
    @details    Adds the track namespace to the file
    @param      file: Pointer to the file handler
    @param      trackName: Name of the track
    @param      trackDesc: Description of the track
    @param      time: Time of the track

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXAddTrack(FILE_TYPE file, const char* trackName, const char* trackDesc, const char* time){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t<trk>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t<trk>\n");
    #endif
}

/*!
    @brief      GPXAddTrackSegment
    @details    Adds the track segment namespace to the file
    @param      file: Pointer to the file handler

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXAddTrackSegment(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t\t<trkseg>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t\t<trkseg>\n");
    #endif
}


/*!
    @brief      GPXAddNewTrackSegment
    @details    Closes the current track segment and opens a new one
    @param      file: Pointer to the file handler

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXAddNewTrackSegment(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t\t</trkseg>\n");
        f_printf(file, "\t\t<trkseg>\n");
    #else
        if(file == NULL){
            return;
        }
        fprintf(*file, "\t\t</trkseg>\n");
        fprintf(*file, "\t\t<trkseg>\n");
    #endif
}

/*!
    @brief      GPXAddTrackPoint
    @details    Adds a track point to the file
    @param      file: Pointer to the file handler
    @param      lat: Latitude of the track point
    @param      lon: Longitude of the track point
    @param      ele: Elevation of the track point
    @param      time: Time of the track point

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXAddTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_TRACK_POINT, lat, lon, ele, time);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_TRACK_POINT, lat, lon, ele, time);
    #endif
}

/*!
    @brief      GPXCloseTrackSegment
    @details    Closes the current track segment
    @param      file: Pointer to the file handler


    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXCloseTrackSegment(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t\t</trkseg>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t\t</trkseg>\n");
    #endif
}

/*!
    @brief      GPXCloseTrack
    @details    Closes the current track
    @param      file: Pointer to the file handler

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXCloseTrack(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t</trk>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t</trk>\n");
    #endif
}

/*!
    @brief      GPXCloseFile
    @details    Closes the current file
    @param      file: Pointer to the file handler

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile must be called before this function
*/
void GPXCloseFile(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "</gpx>");
        f_close(file);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "</gpx>");
        fclose(*file);
    #endif
}
///@}
