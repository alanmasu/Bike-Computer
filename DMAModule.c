/*!
    @file   DMAModule.c
    @brief  DMA module functions and definitions
    @date   03/01/2024
    @author Alan Masutti
*/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Local Includes */
#include <DMAModule.h>

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif

static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32]; //!< DMA control table


/*!
    @brief DMA module initialization
    @details This function initializes the DMA module
    
*/
void dmaInit(void){
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);
}
