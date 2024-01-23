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

    GrStringDraw(&g_sContext,(int8_t*)StringTemp(98),-1, temp.xMin + 60, temp.yMin + 4, 0);
    GrStringDraw(&g_sContext,"Avg Speed: ",-1,avSpeed.xMin+7,avSpeed.yMin+2,0);
    GrStringDraw(&g_sContext,"Inst Speed: ",-1,instSpeed.xMin+7,instSpeed.yMin+2,0);
    GrStringDraw(&g_sContext,"Acc: ",-1,acc.xMin+7,acc.yMin+2,0);
    GrStringDraw(&g_sContext,"Distance: ",-1,distance.xMin+7,distance.yMin+2,0);
    GrStringDraw(&g_sContext,"GpsCoord&Altitude:",-1,coord.xMin+7,coord.yMin+2,0);
    GrStringDrawCentered(&g_sContext,"GO TO MENU",-1,(menu.xMax)/2,109,0);
    GrFlush(&g_sContext);

    free(StringTemp);

}



