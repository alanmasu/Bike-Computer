/*!
    @file   BSS.h
    @brief  Functions for initializing BSS, compute, classify and manage status and lights of bike. 
    @date   14/12/2023
    @author Alberto Dal Bosco
*/


#ifndef __BSS_H__
#define __BSS_H__

/*!
    @defgroup BSS_module BSS
    @{
*/

// port 6.6 front light
#define GPIO_PORT_FRONT_LIGHT   GPIO_PORT_P6            
#define GPIO_PIN_FRONT_LIGHT    GPIO_PIN6
// port 6.7 rear light
#define GPIO_PORT_REAR_LIGHT    GPIO_PORT_P6            
#define GPIO_PIN_REAR_LIGHT     GPIO_PIN7

#define ACCEL_WINDOW_SIZE 10
#define ACC_THREASHOLD -0.5
#define LIGHT_THREASHOLD 30            
#define ACC_MIN -4.5
#define ACC_MAX 1.0
#define NUM_FLASH 4
#define T_MIN -20
#define T_MAX 60




/*!
    @brief acceleration struct
*/
typedef struct {
    double x;
    double y;
    double z;
}threeAxis_t;

/*!
    @brief bike class struct
*/
typedef enum {
    CLASS_IDLE,                     // State for evaluating class
    CLASS_ERROR,                    // Sensor temperature exceed MAX Temperature
    CLASS_BRAKING,                  // BSS active, so flash!
    CLASS_MOVING,                   // normal functionality, light off. 
    CLASS_LOW_AMBIENT_LIGHT,        // normal functionality, light on.
}class_t;

typedef threeAxis_t accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];    //!< Windows of accelerations

/*!
    @brief Contains all the necessary informations of the status of the bike. 
*/
typedef struct {
    accelWindow window;     //!< Windows of accelerations
    class_t class;          //!< class
    float averageAcc;       //!< average acceleration 
    double temp;            //!< temperature of sensor
    double light;           //!< light measure
}model_t;


/*!
    @brief setter for the light
    @param[in] model: model struct.
    @param[in] light: light from photoresistor.
*/
void set_light(model_t* model, float light);

/*!
    @brief getter for the model
    @param[out] punt_model: model pointer.
*/
model_t* get_model();

/*!
    @brief getter for class name
    @param[out] class_name: model class name as string.
    @param[in] class_enum: enum of class
*/
const char* get_class_name(class_t class_enum);


#ifdef SIMULATE_HARDWARE
    /*!
        @brief Return random number
        @param[out] random_num: random number.
        @param[in] min: minimum to start.
        @param[in] max: maximum to reach.

    */
    float randomNum(float min, float max);

    /*!
        @brief Test scaffold function --> return random acc
        @param[out] rand_x_acc: random acceleration along x axis.
    */
    float readAccelX();

    /*!
        @brief Test scaffold function --> return random acc
        @param[out] rand_y_acc: random acceleration along y axis.
    */
    float readAccelY();

    /*!
        @brief Test scaffold function --> return random acc
        @param[out] rand_z_acc: random acceleration along z axis.
    */
    float readAccelZ();

    /*!
        @brief Test scaffold function --> return random temp
        @param[out] rand_temp: random temperature.
    */
    double rand_temp();

    /*!
        @brief Test scaffold function --> return random light value
        @param[out] rand_light: random light value.
    */
    double rand_light();
#else

    /*!
        @brief Initialize MPU6050 sensor
    */
    void _MPU6050SensorInit();

    /*!
        @brief Initialize front and rear lights
    */
    void _ledInit();

    /*!
        @brief Initialize timer for flashing.
        @param[in] timer_config: configuration of the timer to flash each 150 milliseconds.
    */
    void _timerFlashInit(const Timer_A_UpModeConfig* upConfig);

    /*!
        @brief Initialize BSS.
        @param[in] timer_config: configuration of the timer to flash each 150 milliseconds.
    */
    void _BSSInit(const Timer_A_UpModeConfig* upConfig);

    /*!
        @brief Set HIGH front light
    */
    __attribute__ ((always_inline)) inline void frontLightUp();

    /*!
        @brief Set HIGH rear light
    */
    __attribute__ ((always_inline)) inline void rearLightUp();

    /*!
        @brief Set DOWN front light
    */
    __attribute__ ((always_inline)) inline void frontLightDown();

    /*!
        @brief Set DOWN rear light
    */
    __attribute__ ((always_inline)) inline void rearLightDown();

    /*!
        @brief Toggle front light
    */
    __attribute__ ((always_inline)) inline void toggleFrontLight();

    /*!
        @brief Toggle rear light
    */
    __attribute__ ((always_inline)) inline void toggleRearLight();

    /*!
        @brief Read light value from photoresistor
        @param[out] light: light value.
    */
    double read_light_value();

    /*!
        @brief Read light value from photoresistor.
        @param[in] three_acc: set of x,y,z accelerations to sample.
    */
    void accel_sample(accelReading* result);

    /*!
        @brief Acquire a window of accelerations and save it into model struct passed.
        @param[in] model: model of an instant.
    */
    void acquire_window(model_t* model);

    /*!
        @brief Compute average acceleration and save light and temperature of the sensor into model passed.
        @param[in] model: model of an instant.
    */
    void compute(model_t* model);

    /*!
        @brief Classify status of the bike and manage it.
        @param[in] model: model of an instant.
    */
    void classify(model_t* model);
#endif 

#ifdef SIMULATE_HARDWARE
    /*!
        @brief Test scaffold function --> print status in stdout
        @param[in] model: model of an instant.
    */
    void execute(model_t* model);

    /*!
        @brief Test scaffold function --> print model in stdout
        @param[in] model: model of an instant.
    */
    void print_model(const model_t* model);
#endif

/*
    @}
*/


#endif
