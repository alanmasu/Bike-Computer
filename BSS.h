#ifndef __BSS_H__
#define __BSS_H__

// port 6.6 front light
#define GPIO_PORT_FRONT_LIGHT   GPIO_PORT_P6            
#define GPIO_PIN_FRONT_LIGHT    GPIO_PIN6
// port 6.7 rear light
#define GPIO_PORT_REAR_LIGHT    GPIO_PORT_P6            
#define GPIO_PIN_REAR_LIGHT     GPIO_PIN7

#define ACCEL_WINDOW_SIZE 1
#define ACC_THREASHOLD -0.1
#define LIGHT_THREASHOLD 40            // TODO: DEFINE IT WITH REAL VALUE --> 40 stands for 40% of light
#define ACC_MIN -4.5
#define ACC_MAX 1.0
#define NUM_FLASH 4
#define T_MIN -20
#define T_MAX 60


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

void set_light(float light);
model_t* get_model();
const char* get_class_name(class_t class_enum);
#ifdef SIMULATE_HARDWARE
    float randomNum(float min, float max);
    float readAccelX();
    float readAccelY();
    float readAccelZ();
    double rand_temp();
    double rand_light();
#else
    void _MPU6050SensorInit();
    void _ledInit();
    void _timerFlashInit(const Timer_A_UpModeConfig* upConfig);
    void _BSSInit(const Timer_A_UpModeConfig* upConfig);
    __attribute__ ((always_inline)) inline void frontLightUp();
    __attribute__ ((always_inline)) inline void rearLightUp();
    __attribute__ ((always_inline)) inline void frontLightDown();
    __attribute__ ((always_inline)) inline void rearLightDown();
    __attribute__ ((always_inline)) inline void toggleFrontLight();
    __attribute__ ((always_inline)) inline void toggleRearLight();
    double read_light_value();
    void accel_sample(accelReading* result);
    void acquire_window(model_t* model);
    void compute(model_t* model);
    void classify(model_t* model);
#endif 

#ifdef SIMULATE_HARDWARE
    void execute(model_t* model);
    void print_model(const model_t* model);
#endif


#endif
