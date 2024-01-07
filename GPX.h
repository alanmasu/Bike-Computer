#ifndef __GPX_H__
#define __GPX_H__

#ifndef SIMULATE_HARDWARE
    #include <fatfs/ff.h>
    #include <fatfs/diskio.h>
#else
    #include <stdio.h>
#endif
#include <string.h>


void GPXInitFile(const char* filename);


void GPXAddTrack(const char* filename, const char* trackName, const char* trackDesc, const char* time);
void GPXAddTrackSegment(const char* filename);
void GPXAddNewTrackSegment(const char* filename);

void GPXAddTrackPoint(const char* filename, const char* lat, const char* lon, const char* ele, const char* time);
void GPXCloseTrackSegment(const char* filename);
void GPXCloseTrack(const char* filename);
void GPXCloseFile(const char* filename);

//Future implementation
//void GPXAddWayPoint(const char* filename, double lat, double lon, double ele, const char* time);



#endif // __GPX_H__
