#include "GPIO_Driver.h"
#include "SPI_Driver.h"
#include "TIMERA_Driver.h"
#include "UART_Driver.h"
#include "SD_Driver.h"

void SD_Init(uint32_t SPI, uint32_t Port, uint16_t Pin){
    GPIO_Init(MMC_SS_PORT, MMC_SS_PIN);
}
