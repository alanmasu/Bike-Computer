/*
 * main.c
 *
 *  Created on: 25 nov 2023
 *      Author: Federica_Lorenzini
 */
#include "mainInterface.h"

int main(void){
    graphicsInit();
    drawGrid();
    char tmpString[40]="/0";

    snprintf(tmpString,39,"Temp: %.2f C",22.6);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1, temp.xMin + 60, temp.yMin + 4, 0);

    snprintf(tmpString,39,"AvgSpeed: %.2f km/h",24.5);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1,avSpeed.xMin+7,avSpeed.yMin+2,0);

    snprintf(tmpString,39,"Speed: %.2f km/h",34.5);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1,instSpeed.xMin+7,instSpeed.yMin+4,0);

    snprintf(tmpString,39,"Acc: %.2f m/s^2",20.6);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1,acc.xMin+7,acc.yMin+2,0);

    snprintf(tmpString,39,"Distance: %.2f km",20.0);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1,distance.xMin+7,distance.yMin+2,0);

    snprintf(tmpString,39,"Gps Altitude:%d m",600);
    GrStringDraw(&g_sContext,(int8_t*)tmpString,-1,coord.xMin+7,coord.yMin+2,0);

    GrStringDrawCentered(&g_sContext,"GO TO MENU",-1,(menu.xMax)/2,109,0);
    GrFlush(&g_sContext);

    free(StringTemp);

}



