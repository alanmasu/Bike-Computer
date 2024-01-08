#include "GPX.h"
#include <stdio.h>

const char* GPX_HEADER = "\
<?xml version='1.0' encoding='UTF-8'?>\n\
<gpx version=\"1.1\" creator=\"IoTProject2023\" xmlns=\"http://www.topografix.com/GPX/1/1\" \
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";

const char* GPX_TRACK_POINT = "\
            <trkpt lat=\"%s\" lon=\"%s\">\n\
                <ele>%s</ele>\n\
                <time>%s</time>\n\
            </trkpt>\n";

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


void GPXAddTrack(FILE_TYPE file, const char* trackName, const char* trackDesc, const char* time){
    #ifndef SIMULATE_HARDWARE
        f_printf(*file, "\t<trk>\n");
    #else
        if(*file == NULL){
            return;
        }
        fprintf(*file, "\t<trk>\n");
    #endif
}
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
