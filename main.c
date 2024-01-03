/*! @file main.c
 *  @brief Main function for the program
 *	@details This file contains the main function for the program
 *	@version 1.0
 *	@date 03/01/2024
 */

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>


void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

}
