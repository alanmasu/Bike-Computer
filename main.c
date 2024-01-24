/*! @file main.c
 *  @brief Main function for the program
 *  @details This file contains the main function for the program
 *  @version 1.0
 *  @date 03/01/2024
 */

/*
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
*/
#include <stdio.h>
#include <stdint.h>
#include "HAL_I2C.h"
#include "MPU6050.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


// Graphic library context
//Graphics_Context g_sContext;
double xAcc, yAcc, zAcc, temp;
int id;

/*

// Timer_A Up Configuration Parameter
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 250 KhZ
        250,                                    // 40 ms tick period. The timer counts up to 20000, then 0
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };

// Timer_A Compare Configuration Parameter  (PWM)
//  Playing with the Duty Cycle, we modify the frequency, so the sound of the buzzer.

Timer_A_CompareModeConfig compareConfig_PWM = {
        TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output bit
        125                                         // 50% Duty Cycle initially (125/250 = 0.5)*100 = 50.
        };


void _buzzerInit()
{
    // Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);          // pin operate in PWM mode

    // Configuring Timer_A0 for Up Mode and starting 
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    // Initialize compare registers to generate PWM 
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.7
}

void _buzzerUp()
{
    // Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);          // pin operate in PWM mode

    Timer_A_setCompareValue(TIMER_A0_BASE, &compareConfig_PWM, 300);

    // Configuring Timer_A0 for Up Mode and starting
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    // Initialize compare registers to generate PWM 
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.7
}


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

void _ledInit(){
    // Set led RGB to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN6);
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
    _buzzerInit();
    __delay_cycles(100000);
    _buzzerUp();
    _ledInit();
}


*/



// _________________________________________________________________________________________________

// this part is added just for test, NON ALREADY TRIED IN HARDWARE BUT WORKS IN C COMPLER AS I WANT


// PSEUDO_CODE: acquire_window --> compute_class --> reaction_to_class
// THIS PART SHOULD BE HARDWARE INDEPENDENT --> USER USES THESE FUNCTIONS TO OBTAIN ACC VALUES.

#define ACCEL_WINDOW_SIZE 10
#define ACC_THREASHOLD -2.0
#define LIGHT_THREASHOLD 40            // TODO: DEFINE IT WITH REAL VALUE --> 40 stands for 40% of light
#define ACC_MIN -4.5
#define ACC_MAX 1.0
#define T_MIN -20
#define T_MAX 60

#define SIMULATE_HARDWARE

/*
    TEST SCAFFOLD for accelerometer Hardware Independent Part
*/

typedef struct {
    double x;
    double y;
    double z;
}threeAxis_t;

typedef enum {
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

const char* get_class_name(class_t class_enum) {
    switch (class_enum) {
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

double read_light_value(){
    // TODO: this function is implemented by Sofia
    return randomNum(0, 100);
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
    if(model->temp >= T_MAX){
        model->class = CLASS_ERROR;
    }
    else{                                               // Temperature ok: 
        if(model->averageAcc <= ACC_THREASHOLD){            
            model->class = CLASS_BRAKING;               
        }
        else{                                               // no braking:
            if (model->light <= LIGHT_THREASHOLD){
                model->class = CLASS_LOW_AMBIENT_LIGHT;
            }
            else{
                model->class = CLASS_MOVING;
            }
        }
    }
}
    

void execute(const class_t class){
    #ifdef SIMULATE_HARDWARE
        fflush(stdout);
        switch(class){
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

    #else 

        switch(class){
            case CLASS_ERROR:
                // init_funct();
                break;
            case CLASS_BRAKING:
                //rear_light_flash();
                break;
            
            case CLASS_LOW_AMBIENT_LIGHT:
                //rear_light_on();
                //front_light_on();
                break;
            case CLASS_MOVING:
                //no_light_on();
                break;
            default:
                break;
        }
    #endif
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



int main(){
    fflush(stdout);
    srand(time(NULL));
    model_t model;

    #ifdef SIMULATE_HARDWARE   
        
        while(1){
            acquire_window(&model);
            compute(&model);
            classify(&model);
            print_model(&model);
            execute(model.class);
            sleep(5);
        }
        return 0;

    #else
        
        int counter = 0;
        while (1){
            acquire_window(&model);
            compute(&model);
            light_management(model.class);
            
            if TEST_SCAFFOLD {
                printf("\n---------------------------------------------------------------------------------------------------------------");
                print_model(&model);
                printf("\n---------------------------------------------------------------------------------------------------------------\n\n");
                fflush(stdout);
            }

            char string[24];
            int len = 0;

            id = MPU6050_readDeviceId();
            xAcc = MPU6050_readXvalue();
            yAcc = MPU6050_readYvalue();
            zAcc = MPU6050_readZvalue();
            temp = MPU6050_readTemp_chip();

            len = sprintf(string, " MPU6050 I2C ");
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            15,
                                            OPAQUE_TEXT);

            len = sprintf(string, "ID : %5d ", id);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            35,
                                            OPAQUE_TEXT);

            len = sprintf(string, " xAcc : %3.3f g   ", xAcc);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            50,
                                            OPAQUE_TEXT);

            len = sprintf(string, " yAcc : %3.3f g   ", yAcc);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            65,
                                            OPAQUE_TEXT);

            len = sprintf(string, " zAcc : %3.3f g   ", zAcc);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,                         // 8 bits of string + 16 bits of acc
                                            64,
                                            80,
                                            OPAQUE_TEXT);

            sprintf(string, " temp : %3.3f ^C   ", temp);
            Graphics_drawStringCentered(&g_sContext,
                                            (int8_t *)string,
                                            len,
                                            64,
                                            95,
                                            OPAQUE_TEXT);

            sprintf(string, " counter : %d", counter);
                    Graphics_drawStringCentered(&g_sContext,
                                                    (int8_t *)string,
                                                    len,
                                                    64,
                                                    110,
                                                    OPAQUE_TEXT);

            ++counter;
            __delay_cycles(10000);
        }

    #endif
}


/*

void main(void)
{
    srand(time(NULL));
    model_t model;
    _hwInit();

    int counter = 0;

    while (1)
    {
        acquire_window(&model);
        compute(&model);
        light_management(model.class);
        
        if TEST_SCAFFOLD {
            printf("\n---------------------------------------------------------------------------------------------------------------");
            print_model(&model);
            printf("\n---------------------------------------------------------------------------------------------------------------\n\n");
            fflush(stdout);
        }

        char string[24];
        int len = 0;

        id = MPU6050_readDeviceId();
        xAcc = MPU6050_readXvalue();
        yAcc = MPU6050_readYvalue();
        zAcc = MPU6050_readZvalue();
        temp = MPU6050_readTemp_chip();

        len = sprintf(string, " MPU6050 I2C ");
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,
                                        64,
                                        15,
                                        OPAQUE_TEXT);

        len = sprintf(string, "ID : %5d ", id);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,
                                        64,
                                        35,
                                        OPAQUE_TEXT);

        len = sprintf(string, " xAcc : %3.3f g   ", xAcc);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,
                                        64,
                                        50,
                                        OPAQUE_TEXT);

        len = sprintf(string, " yAcc : %3.3f g   ", yAcc);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,
                                        64,
                                        65,
                                        OPAQUE_TEXT);

        len = sprintf(string, " zAcc : %3.3f g   ", zAcc);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,                         // 8 bits of string + 16 bits of acc
                                        64,
                                        80,
                                        OPAQUE_TEXT);

        sprintf(string, " temp : %3.3f ^C   ", temp);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        len,
                                        64,
                                        95,
                                        OPAQUE_TEXT);

        sprintf(string, " counter : %d", counter);
                Graphics_drawStringCentered(&g_sContext,
                                                (int8_t *)string,
                                                len,
                                                64,
                                                110,
                                                OPAQUE_TEXT);

        ++counter;
        __delay_cycles(10000);
    }
}

*/