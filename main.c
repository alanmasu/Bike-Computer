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
    // port 6.6 front light
    #define GPIO_PORT_FRONT_LIGHT   GPIO_PORT_P6            
    #define GPIO_PIN_FRONT_LIGHT    GPIO_PIN6
    // port 6.7 rear light
    #define GPIO_PORT_REAR_LIGHT    GPIO_PORT_P6            
    #define GPIO_PIN_REAR_LIGHT     GPIO_PIN7

#else

    #include <unistd.h>
    // seconds between readings in test scaffold mode. 10 is the lower value to be able to read all info.
    #define INTERVAL_BETWEEN_READING_TEST   10              
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <time.h>

#endif


#define ACCEL_WINDOW_SIZE 1
#define ACC_THREASHOLD -0.1
#define LIGHT_THREASHOLD 40            // TODO: DEFINE IT WITH REAL VALUE --> 40 stands for 40% of light
#define ACC_MIN -4.5
#define ACC_MAX 1.0
#define NUM_FLASH 4
#define T_MIN -20
#define T_MAX 60


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

    volatile int count_flash = 0;

#endif


typedef struct {
    double x;
    double y;
    double z;
}threeAxis_t;

typedef enum {
    CLASS_IDLE,                     // State for evaluating class
    CLASS_ERROR,                    // Sensor temperature exceed MAX Temperature
    CLASS_BRAKING,                  // BSS active, so flash!
    CLASS_MOVING,                   // normal functionality, light off. 
    CLASS_LOW_AMBIENT_LIGHT,        // normal functionality, light on.
}class_t;

typedef threeAxis_t accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
    accelWindow window;
    class_t class;
    float averageAcc;
    double temp;
    double light;
}model_t;

volatile model_t *model_BSS;

const char* get_class_name(class_t class_enum) {
    switch (class_enum) {
        case CLASS_IDLE:
            return "CLASS_IDLE";
        case CLASS_ERROR:
            return "CLASS_ERROR";
        case CLASS_BRAKING:
            return "CLASS_BRAKING";
        case CLASS_MOVING:
            return "CLASS_MOVING";
        case CLASS_LOW_AMBIENT_LIGHT:
            return "CLASS_LOW_AMBIENT_LIGHT";
        default:
            return "Unknown Class";
    }
}

#ifdef SIMULATE_HARDWARE
    float randomNum(float min, float max) {
        int randInt = rand();
        float randFloat = min + ((float)randInt / RAND_MAX) * (max - min);
        return randFloat;
    }

    float readAccelX(){
        return randomNum(ACC_MIN, ACC_MAX);
    }

    float readAccelY(){
        return randomNum(ACC_MIN, ACC_MAX);
    }

    float readAccelZ(){
        return randomNum(ACC_MIN, ACC_MAX);
    }

    double rand_temp(){
        return randomNum(20, 65);
    }

    double rand_light(){
        return randomNum(0, 100);
    }

#else

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


    void _MPU6050SensorInit()                             // MPU6050 Sensor initialization
    {
        // Select I2C function for I2C_SCL(P6.5) & I2C_SDA(P6.4)
        Init_I2C_GPIO();

        // Initialize I2C communication
        I2C_init();

        // Initialization of the accelerometer sensor
        MPU6050_init();

        // I wait a little bit.
        __delay_cycles(100000);
    }

    void _ledInit()
    {
        // Set front light to output direction
        GPIO_setAsOutputPin(GPIO_PORT_FRONT_LIGHT,GPIO_PIN_FRONT_LIGHT);
        // Set rear light to output direction
        GPIO_setAsOutputPin(GPIO_PORT_REAR_LIGHT,GPIO_PIN_REAR_LIGHT);
    }

    void _timerFlashInit()
    {
        /* Configuring Timer_A1 for Up Mode */
        Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

        /* Enabling interrupts and starting the timer */
        Interrupt_enableInterrupt(INT_TA1_0);

        /* Enabling MASTER interrupts */
        Interrupt_enableMaster();

        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    }

    void _hwInit()
    {
        // Halting WDT and disabling master interrupts
        WDT_A_holdTimer();
        Interrupt_disableMaster();

        // Set the core voltage level to VCORE1
        PCM_setCoreVoltageLevel(PCM_VCORE1);

        // Set 2 flash wait states for Flash bank 0 and 1
        FlashCtl_setWaitState(FLASH_BANK0, 2);
        FlashCtl_setWaitState(FLASH_BANK1, 2);
        
        // Initializes Clock System
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
        CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        
        _graphicsInit();
        _MPU6050SensorInit();
        _ledInit();
        _timerFlashInit();
        __delay_cycles(100000);
    }

    __attribute__ ((always_inline)) inline void frontLightUp()
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_FRONT_LIGHT,GPIO_PIN_FRONT_LIGHT);            // Set front light HIGH
    }

    __attribute__ ((always_inline)) inline void rearLightUp()
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_REAR_LIGHT,GPIO_PIN_REAR_LIGHT);              // Set rear light HIGH
    }

    __attribute__ ((always_inline)) inline void frontLightDown()
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_FRONT_LIGHT,GPIO_PIN_FRONT_LIGHT);             // Set front light LOW
    }

    __attribute__ ((always_inline)) inline void rearLightDown()
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_REAR_LIGHT,GPIO_PIN_REAR_LIGHT);               // Set rear light LOW
    }

    __attribute__ ((always_inline)) inline void toggleFrontLight()
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_FRONT_LIGHT,GPIO_PIN_FRONT_LIGHT);             // toggle front light
    }

    __attribute__ ((always_inline)) inline void toggleRearLight()
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_REAR_LIGHT,GPIO_PIN_REAR_LIGHT);               // toggle rear light
    }

#endif



double read_light_value(){
    // TODO: this function is implemented by Sofia (NOT FOR TEST SCAFFOLD)
    return 50.0;
    //randomNum(0, 100);
}

void accel_sample(accelReading* result){            // This function read accelerations along x, y, z axis and save it in result, that is a structured variable defined before. 
    #ifdef SIMULATE_HARDWARE                            
        result->x = readAccelX();                       // read random - HID x acceleration  ----- 
        result->y = readAccelY();                       // read random - HID y acceleration       |--->  TEST SCAFFOLD
        result->z = readAccelZ();                       // read random - HID z acceleration  -----
    #else
        result->x = MPU6050_readXvalue();               // read x acceleration from MPU6050 sensor
        result->y = MPU6050_readYvalue();               // read y acceleration from MPU6050 sensor
        result->z = MPU6050_readZvalue();               // read z acceleration from MPU6050 sensor
    #endif
}

void acquire_window(model_t* model){
    accelReading sample;                                // Istantiate an empty sample
    unsigned samplesInWindow = 0;                       // index for iterate sample on window array

    while(samplesInWindow < ACCEL_WINDOW_SIZE){         // Fill window array with ACCEL_WINDOW_SIZE sample element
        accel_sample(&sample);                          // Save in sample values read from sensor
        model->window[samplesInWindow++] = sample;      // Store element in index given, and increment it
    }  
}

void compute(model_t* model){
    float sum_acc_x = 0.0;                              // partial sum of x acceleration (initiated to zero)
    for(int i=0; i<ACCEL_WINDOW_SIZE; i++){             // compute sum of all x acceleration in window array
        sum_acc_x += model->window[i].x;
    }
    model->averageAcc = (float) sum_acc_x / ACCEL_WINDOW_SIZE;      // save Average acceleration in model variable

    #ifdef SIMULATE_HARDWARE
        model->temp = rand_temp();
        model->light = rand_light();
    #else 
        model->temp = MPU6050_readTemp_chip();
        model->light = read_light_value();
    #endif  
}


void classify(model_t* model){                          // establish model class
    switch(model->class){
        case CLASS_IDLE:
            count_flash = 0;
            if(model->temp > T_MAX){
                model->class = CLASS_ERROR;
            }
            else if(model->averageAcc < ACC_THREASHOLD){
                model->class = CLASS_BRAKING;
            }
            else if(model->light < LIGHT_THREASHOLD){
                model->class = CLASS_LOW_AMBIENT_LIGHT;
            }else{
                model->class = CLASS_MOVING;
            }
            break;
        case CLASS_ERROR:
            if(count_flash < NUM_FLASH){
                model->class = CLASS_ERROR;
            }else{
                if(model->temp > T_MAX){
                    model->class = CLASS_ERROR;
                }else{
                    model->class = CLASS_IDLE;
                    count_flash = 0;
                }
            }
            break;
        case CLASS_BRAKING:
            if(count_flash < NUM_FLASH){
                model->class = CLASS_BRAKING;
            }else{
                model->class = CLASS_IDLE;
                count_flash = 0;
            }
            break;
        case CLASS_MOVING:
            frontLightDown();
            rearLightDown();
            count_flash = 0;
            model->class = CLASS_IDLE;
            break;
        case CLASS_LOW_AMBIENT_LIGHT:
            rearLightUp();
            frontLightUp();
            count_flash = 0;
            model->class = CLASS_IDLE;
            break;
    }
}
    

#ifdef SIMULATE_HARDWARE

    void execute(model_t* model){
        fflush(stdout);
        switch(model->class){
            case CLASS_ERROR:
                printf("\t\t |\n\t\t ------> ");
                printf(" BUZZER: flash, REAR LIGHT: flash, FRONT LIGHT: flash, LCD SCREEN: error_page");
                break;
            case CLASS_BRAKING:
                printf("\t\t |\n\t\t ------> ");
                printf(" BUZZER: low, REAR LIGHT: flash, FRONT LIGHT: -, LCD SCREEN: None");
                break;
            case CLASS_LOW_AMBIENT_LIGHT:
                printf("\t\t |\n\t\t ------> ");
                printf(" BUZZER: low, REAR LIGHT: High, FRONT LIGHT: high, LCD SCREEN: None");
                break;
            case CLASS_MOVING:
                printf("\t\t |\n\t\t ------> ");
                printf(" BUZZER: low, REAR LIGHT: low, FRONT LIGHT: low, LCD SCREEN: None");
                break;
            default:
                break;
        }
        fflush(stdout);
    }

    void print_model(const model_t* model){
        printf("\n\n\n\n---------------------------------------------------------------------------------------------------------------");
        printf("\n Acc X:     [ ");
        for(int i=0; i<ACCEL_WINDOW_SIZE; i++){
            if(i == ACCEL_WINDOW_SIZE-1){
                printf("%0.3f ]", model->window[i].x);
            }else{
                printf("%0.3f", model->window[i].x);
                printf(",  ");
            }
        }
        printf("\n Average Acceleration:  %0.3f", model->averageAcc);
        printf("\n Temperature:  %f", model->temp);
        printf("\n Light:  %f", model->light);
        printf("\n Class: %s", get_class_name(model->class));
        printf("\n---------------------------------------------------------------------------------------------------------------\n");
        fflush(stdout);
    }

#endif




int main(){
    model_t model;
    model.class = CLASS_IDLE;

    // TODO: create a function for doing this --> model_BSS will be declared in BSS.c
    model_BSS = &model;


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
        
        _hwInit();
        int counter = 0;
        char string[24];
        int len = 0;
        int id;

        while(1)
        {
            acquire_window(&model);
            compute(&model);
            classify(&model);

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

            len = sprintf(string, " xAcc : %3.3f g   ", model.averageAcc);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            70,
                                            OPAQUE_TEXT);

            sprintf(string, " temp : %3.3f ^C   ", model.temp);
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


#ifndef SIMULATE_HARDWARE

    void TA1_0_IRQHandler(void)
    {
        if(model_BSS->class == CLASS_ERROR){
            ++count_flash;
            toggleRearLight();
            toggleFrontLight();
        }else if (model_BSS->class == CLASS_BRAKING){
            ++count_flash;
            toggleRearLight();
        }
        Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }

#endif
