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

void GPXInitFile(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "%s", GPX_HEADER);
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "w");
        if(file == NULL){
            return;
            printf("Error opening file!\n");
        }
        fprintf(file, GPX_HEADER);
        fclose(file);
    #endif
}


void GPXAddTrack(const char* filename, const char* trackName, const char* trackDesc, const char* time){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "\t<trk>\n");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "\t<trk>\n");
        fclose(file);
    #endif
}
void GPXAddTrackSegment(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "\t\t<trkseg>\n");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "\t\t<trkseg>\n");
        fclose(file);
    #endif
}
void GPXAddNewTrackSegment(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "\t\t</trkseg>\n");
        f_printf(&file, "\t\t<trkseg>\n");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "\t\t</trkseg>\n");
        fprintf(file, "\t\t<trkseg>\n");
        fclose(file);
    #endif
}
void GPXAddTrackPoint(const char* filename, const char* lat, const char* lon, const char* ele, const char* time){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, GPX_TRACK_POINT, lat, lon, ele, time);
        f_close(&file);
    #else
        FILE *file;
        if(file == NULL){
            return;
        }
        fprintf(file, GPX_TRACK_POINT, lat, lon, ele, time);
        fclose(file);
    #endif
}
void GPXCloseTrackSegment(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "\t\t</trkseg>\n");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "\t\t</trkseg>\n");
        fclose(file);
    #endif
}
void GPXCloseTrack(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "\t</trk>\n");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "\t</trk>\n");
        fclose(file);
    #endif
}

void GPXCloseFile(const char* filename){
    #ifndef SIMULATE_HARDWARE
        FRESULT r;
        FIL file;
        r = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
        if(r != FR_OK){
            return;
        }
        f_printf(&file, "</gpx>");
        f_close(&file);
    #else
        FILE *file;
        file = fopen(filename, "a");
        if(file == NULL){
            return;
        }
        fprintf(file, "</gpx>");
        fclose(file);
    #endif
}
