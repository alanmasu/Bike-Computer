/*
 * main.c
 *
 *  Created on: 25 nov 2023
 *      Author: Federica_Lorenzini
 */
#include "mainInterface.h"

int main(void){
    graphicsInit();
    current_page=PAGE_1;
    drawGrid1();
    showPage1();
    GrFlush(&g_sContext);

}



