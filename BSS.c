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

    volatile int count_flash = 0;

#else

    #include "BSS.h"
    #include <unistd.h>
    // seconds between readings in test scaffold mode. 10 is the lower value to be able to read all info.
    #define INTERVAL_BETWEEN_READING_TEST   10              
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <time.h>

#endif

volatile static model_t model_BSS;


void set_light(model_t* model, float light){
    model->light = light;
} 

model_t* get_model(){
    return (model_t*) &model_BSS;
}

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

    void _timerFlashInit(const Timer_A_UpModeConfig* upConfig)
    {
        /* Configuring Timer_A1 for Up Mode */
        Timer_A_configureUpMode(TIMER_A2_BASE, upConfig);

        /* Enabling interrupts and starting the timer */
        Interrupt_enableInterrupt(INT_TA2_0);

        Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    }

    void _BSSInit(const Timer_A_UpModeConfig* upConfig)
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
        
        _MPU6050SensorInit();
        _ledInit();
        _timerFlashInit(upConfig);
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


#ifndef SIMULATE_HARDWARE

    void TA2_0_IRQHandler(void)
    {
        if(model_BSS.class == CLASS_ERROR){
            ++count_flash;
            toggleRearLight();
            toggleFrontLight();
        }else if (model_BSS.class == CLASS_BRAKING){
            ++count_flash;
            toggleRearLight();
        }
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }

#endif



