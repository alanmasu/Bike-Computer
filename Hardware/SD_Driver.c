#include "GPIO_Driver.h"
#include "SPI_Driver.h"
#include "TIMERA_Driver.h"
#include "UART_Driver.h"
#include "SD_Driver.h"
#include <fatfs/ff.h>
#include <fatfs/diskio.h>

/* Timer_A UpMode Configuration Parameters */
Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,         // SMCLK/1 = 3MHz
        30000,                                  // 1 ms tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void SD_Init(){
    GPIO_Init(MMC_SS_GPIO_PORT, MMC_SS_GPIO_PIN);
    TIMERA_Init(TIMER_A1_BASE, UP_MODE, &upConfig, disk_timerproc);
}
