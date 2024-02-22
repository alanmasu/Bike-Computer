/*!
    @file       GPX.c
    @ingroup    GPX_Module
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
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" \
xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" \
xmlns:gpxx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\" \
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 \
http://www.topografix.com/GPX/1/1/gpx.xsd \
http://www.garmin.com/xmlschemas/GpxExtensions/v3 \
http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd \
http://www.garmin.com/xmlschemas/TrackPointExtension/v1 \
http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\" \
version=\"1.1\" \
creator=\"IoTProject2023\">\n";


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

/*!
    @brief      GPX Track Point start constant string
    @details    Constant string containing the GPX track point schema and placeholders for point data;
                it is used to add a track point to the GPX file but it is not closed, so that extensions can be added
*/
const char* GPX_START_TRACK_POINT = "\
            <trkpt lat=\"%s\" lon=\"%s\">\n\
                <ele>%s</ele>\n\
                <time>%s</time>\n";
const char* GPX_EXTENSION = "\
                <extensions>\n \
                    <gpxtpx:TrackPointExtension xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\">\n";


const char* GPX_TEMP = "\
                    <gpxtpx:atemp>%.2f</gpxtpx:atemp>\n";

const char* GPX_CADENCE = "\
                    <gpxtpx:cad>%.2f</gpxtpx:cad>\n";

const char* GPX_END_EXTENSION = "\
                    </gpxtpx:TrackPointExtension>\n \
                </extensions>\n";

const char* GPX_END_TRACK_POINT = "\
            </trkpt>\n";
/*!
    @brief      GPX MetaData Constant String
    @details    Constant string containing the metadata tag and placeholder for the time;
                it is used to add the metadata mamespace to the GPX file
*/
const char* GPX_METADATA = "\
    <metadata>\n\
        <time>%s</time>\n\
    </metadata>\n";

///@addtogroup GPX_Module
///@{
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
    @brief      GPXAddTrackName
    @details    Adds the track name to the file
    @param      file: Pointer to the file handler
    @param      name: Name of the track

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile and @ref GPXAddTrack must be called before this function
*/
void GPXAddTrackName(FILE_TYPE file, const char* name){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t\t<name>%s</name>\n", name);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t\t<name>%s</name>\n", name);
    #endif
}

/*!
    @brief      GPXAddTrackName
    @details    Adds the track name to the file
    @param      file: Pointer to the file handler
    @param      name: Name of the track

    @note       The function will not close the file handler, it is the responsibility of the caller
                to do so by calling @ref GPXCloseFile
    @pre        @ref GPXInitFile and @ref GPXAddTrack must be called before this function
*/
void GPXAddTrackType(FILE_TYPE file, const char* type){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, "\t\t<type>%s</type>\n", type);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t\t<type>%s</type>\n", type);
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
void GPXAddTrack(FILE_TYPE file, const char* time){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_METADATA, time);
        f_printf(file, "\t<trk>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_METADATA, time);
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
    @brief      GPXOpenTrackPoint
    @details    Opens a track point to the file, but does not close it, so that extensions can be added
    @param      file: Pointer to the file handler
    @param      lat: Latitude of the track point
    @param      lon: Longitude of the track point
    @param      ele: Elevation of the track point
    @param      time: Time of the track point
    @pre        @ref GPXInitFile must be called before this function and almost one track and segment must be added
*/
void GPXOpenTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_START_TRACK_POINT, lat, lon, ele, time);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_START_TRACK_POINT, lat, lon, ele, time);
    #endif
}

void GPXAddExtensionToPoint(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_EXTENSION);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_EXTENSION);
    #endif
}

void GPXAddTempToPoint(FILE_TYPE file, float temp){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_TEMP, temp);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_TEMP, temp);
    #endif
}

void GPXAddCadenceToPoint(FILE_TYPE file, float cadence){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_CADENCE, cadence);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_CADENCE, cadence);
    #endif
}

void GPXCloseExtension(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_END_EXTENSION);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_END_EXTENSION);
    #endif
}

void GPXCloseTrackPoint(FILE_TYPE file){
    #ifndef SIMULATE_HARDWARE
        f_printf(file, GPX_END_TRACK_POINT);
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, GPX_END_TRACK_POINT);
    #endif
}

void GPXAddCompleteTrackPoint(FILE_TYPE file, const char* lat, const char* lon, const char* ele, const char* time, float temp, float cadence){
    GPXOpenTrackPoint(file, lat, lon, ele, time);
    GPXAddExtensionToPoint(file);
    GPXAddTempToPoint(file, temp);
    GPXAddCadenceToPoint(file, cadence);
    GPXCloseExtension(file);
    GPXCloseTrackPoint(file);
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
