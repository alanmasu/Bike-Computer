/*! @file main.c
 *  @brief Main function for the program
 *  @details This file contains the main function for the program
 *  @version 1.0
 *  @date 03/01/2024
 */


//#define SIMULATE_HARDWARE

#ifndef SIMULATE_HARDWARE
    #include <ti/devices/msp432p4xx/inc/msp.h>
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include <ti/grlib/grlib.h>
    #include "LcdDriver/Crystalfontz128x128_ST7735.h"
    #include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
    #include "HAL_I2C.h"
    #include "MPU6050.h"
    #include "BSS.h"
#else

    #include <unistd.h>
    // seconds between readings in test scaffold mode. 10 is the lower value to be able to read all info.
    #define INTERVAL_BETWEEN_READING_TEST   10              
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <time.h>
    #include "BSS.h"

#endif



#ifndef SIMULATE_HARDWARE
    // Graphic library context
    Graphics_Context g_sContext;

    // THIS TIMER IS FOR LED FLASHING   --> Timer_A Up Configuration Parameter
    const Timer_A_UpModeConfig upConfig =
    {
        TIMER_A_CLOCKSOURCE_ACLK,               // ACLK = 32,768 KhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_32,         // ACLK/32 = 1 KhZ --> 1 tick = 1 millisecond
        150,                                    // generate interrupt every 150 millisecond == 150 tick
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
    };


    void _graphicsInit()
    {
        // Initializes display
        Crystalfontz128x128_Init();

        // Set default screen orientation
        Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

        // Initializes graphics context
        Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                             &g_sCrystalfontz128x128_funcs);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        Graphics_clearDisplay(&g_sContext);
    }

#endif

int main(){
    model_t* model = get_model();
    model->class = CLASS_IDLE;


    #ifdef SIMULATE_HARDWARE   
        fflush(stdout);
        srand(time(NULL));
        
        while(1){
            acquire_window(&model);
            compute(&model);
            classify(&model);
            print_model(&model);
            execute(model.class);
            sleep(10);
        }
        return 0;

    #else
        
        _graphicsInit();
        _BSSInit(&upConfig);
        /* Enabling MASTER interrupts */
        Interrupt_enableMaster();

        int counter = 0;
        char string[24];
        int len = 0;
        int id;

        while(1)
        {
            acquire_window(model);
            compute(model);
            classify(model);

            id = MPU6050_readDeviceId();

            len = sprintf(string, " MPU6050 I2C ");
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            20,
                                            OPAQUE_TEXT);

            len = sprintf(string, "ID : %5d ", id);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            50,
                                            OPAQUE_TEXT);

            len = sprintf(string, " xAcc : %3.3f g   ", model->averageAcc);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            70,
                                            OPAQUE_TEXT);

            sprintf(string, " temp : %3.3f ^C   ", model->temp);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            90,
                                            OPAQUE_TEXT);

            sprintf(string, " counter : %d", counter);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            110,
                                            OPAQUE_TEXT);

            counter = counter + 1;
        }

    #endif
}
