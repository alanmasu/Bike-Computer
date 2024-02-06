/*!
    @file       main.c
    @brief      This file merge the functionalities of the SD-Part whit the GPS part.
    @details    The main function configure all hardware dipendent patr, start a comunication channel whit the L80 module on UART
                then start the SD card and mount the fatfs file system. After that the main function sample the GPS data and save
                it on the SD card in a file called "test.gpx" in the root directory. <br>

                For comunication whit the PC uses EUSCI_A0 and MSPIO library. <br>

                Hardware part: <br>
                @code
                        MSP432P401
                  -------------------
                  |                 |
             RST -|     P3.3/UCA0TXD|----> PC RX at 115200 8N1\
                  |                 |                          >  =>  (XDS110 USB to UART)
                  |     P3.2/UCA0RXD|----< PC TX at 115200 8N1/
                  |                 |
                  |     P3.5/UCA2TXD|----> GPS RX at 9600 8N1
                  |     P3.6/UCA2RXD|----< GPS TX at 9600 8N1
                  |                 |
                  |   P1.7/UCAB0MISO|----> SD     SPI MISO
                  |   P1.6/UCAB0MOSI|----< SD/LCD SPI MOSI
                  |   P1.5/UCAB0SCLK|----< SD/LCD SPI CLK
                  |             P5.0|----> LCD    SPI CD
                  |             P5.2|----> SD     SPI CS
                  |                 |
                  |             P5.1|----< BTN Start
                  |             P3.5|----< BTN Stop
                  -------------------
             @endcode
 	@date       03/02/2024
    @authors    Federica Lorenzini, Alan Masutti, Sofia Zandonà, Alberto Dal Bosco
 */

#ifndef SIMULATE_HARDWARE
	#include <ti/devices/msp432p4xx/inc/msp.h>
	#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

	//SD
	#include <Hardware/SPI_Driver.h>
	#include <Hardware/GPIO_Driver.h>
	#include <Hardware/CS_Driver.h>
	#include <Hardware/TIMERA_Driver.h>
	#include <Hardware/SD_Driver.h>
	#include <fatfs/ff.h>
	#include <fatfs/diskio.h>
	#include <Devices/MSPIO.h>
    #include <DMAModule.h>
    //LCD
    #include "mainInterface.h"
    #include "temperature.h"
    #include "adc.h"
    //speed-photoresistor
    #include "photoresistor.h"
    #include "speed.h"
    //BSS
    #include "HAL_I2C.h"
    #include "MPU6050.h"
    #include "BSS.h"

#else
	#include <stdlib.h>
    #include <unistd.h>
    // seconds between readings in test scaffold mode. 10 is the lower value to be able to read all info.
    #define INTERVAL_BETWEEN_READING_TEST   10              
    #include <stdio.h>
    #include <stdint.h>
    #include <time.h>
    #include "BSS.h"
#endif

//Standard includes
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

//Testing includes
#include "Test/GPX_Points.h"


//Local includes
//GPX
#include "GPX.h"
//GPS
#include "GPS.h"

#ifndef SIMULATE_HARDWARE
    #if !defined(DEBUG) || defined(STAND_ALONE)
        #define PRINTF(...)
    #else
        #include <Devices/MSPIO.h>
        #define PRINTF(...) MSPrintf(EUSCI_A0_BASE ,__VA_ARGS__)
    #endif
#else
    #include <stdio.h>
    #define PRINTF(...) printf(__VA_ARGS__)
#endif

    //.
// volatile bool flagTemp;     //!< Flag to arise if a new temperature value is sampled
// volatile int16_t conRes;    //!< Intermediate temperature value sampled from ADC unit

typedef enum{STOP = 0, START = 1} ComputerState_t;

ComputerState_t computerState = STOP;

#ifndef SIMULATE_HARDWARE

const Timer_A_ContinuousModeConfig speedContinuousModeConfig =
    {
        TIMER_A_CLOCKSOURCE_ACLK,              //frequency: 32,768kHz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,         //new frequency: 32,768 kHz
        TIMER_A_TAIE_INTERRUPT_ENABLE,
        TIMER_A_SKIP_CLEAR
    };

    /* Timer_A Continuous Mode Configuration Parameter */
    const Timer_A_UpModeConfig photoresistorUpModeConfig =
    {
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        100,
        TIMER_A_TAIE_INTERRUPT_ENABLE,       // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
    };

    /* Timer_A Compare Configuration Parameter */
    const Timer_A_CompareModeConfig photoresistorCompareConfig =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        100
    };

    const Timer_A_CaptureModeConfig speedCaptureModeConfig =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,        // CC Register 2
        TIMER_A_CAPTUREMODE_RISING_EDGE,          // Rising Edge
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,        // CCIxA Input Select
        TIMER_A_CAPTURE_SYNCHRONOUS,              // Synchronized Capture
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupt
        TIMER_A_OUTPUTMODE_OUTBITVALUE            // Output bit value
    };

    // THIS TIMER IS FOR LED FLASHING   --> Timer_A Up Configuration Parameter
    const Timer_A_UpModeConfig bssUpConfig =
    {
        TIMER_A_CLOCKSOURCE_ACLK,               // ACLK = 32,768 KhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_32,         // ACLK/32 = 1 KhZ --> 1 tick = 1 millisecond
        150,                                    // generate interrupt every 150 millisecond == 150 tick
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
    };

//Buttons defines
#define BTN_START_PORT      GPIO_PORT_P5
#define BTN_START_PIN       GPIO_PIN1
#define BTN_STOP_PORT       GPIO_PORT_P3
#define BTN_STOP_PIN        GPIO_PIN5

#define GPX_TEST_FILENAME   "test.gpx"
FIL file;
#define GPX_TEST_FILE       file

/*!
    @brief     UART Configuration Parameter.
    @details   These are the configuration parameters to
                make the eUSCI A UART module to operate with a 115200 baud rate. These
                values were calculated using the online calculator that TI provides
                at: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART0Config = {
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/*!
    @brief      SPI Configuration Parameter.
    @details    These are the configuration parameters to
                make the eUSCI B SPI module to operate with a 500KHz clock.
*/
eUSCI_SPI_MasterConfig SPI0MasterConfig = {
     EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
     3000000,
     500000,
     EUSCI_B_SPI_MSB_FIRST,
     EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
     EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
     EUSCI_B_SPI_3PIN
};

eUSCI_SPI_MasterConfig config = {
    EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
    LCD_SYSTEM_CLOCK_SPEED,
    LCD_SPI_CLOCK_SPEED,
    EUSCI_B_SPI_MSB_FIRST,
    EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
    EUSCI_B_SPI_3PIN
};

FATFS FS;
DIR DI;
FILINFO FI;
FIL file;

//Buttons Global Variables
bool btnStartStateP = true;
bool btnStopStateP = true;


/*!
    @brief      Main function
    @details    This function configure all hardware dipendent patr, start a comunication channel whit the L80
                module on UART then start the SD card and mount the FAT file system.
                The program waits a button press on BTNStart to start the GPS data sampling and stop this
                process when the BTNStop is pressed.
                After signal a sampling loop save it on the SD card in a file called "test<xxx>.gpx" in
                the root directory, where <xxx> is a progressive number whit runs;
                so if the file test.gpx alredy exists then the "test1.gpx" will be created.
*/
void main(void){
    model_t* model = get_model();
    model->class = CLASS_IDLE;
    //Variables
    //FILE
    bool defaultFile = true;
    char newFileName[15];
    //Buttons
    bool status;
    //GPS
    bool gpsAddPoint = false;



    WDT_A_holdTimer();	// stop watchdog timer
	CS_Init();

	//Configuring GPIO for leds!
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    //Set RGB led pins as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    //PC UART config
    UART_Init(EUSCI_A0_BASE, UART0Config);

    //Initialize all hardware required for the SD Card
    SPI_Init(EUSCI_B0_BASE, SPI0MasterConfig);
    SD_Init();

	//Enable GPS UART
	gpsUartConfig();
    //Enable DMA module
	dmaInit();
	//Enable DMA for EUSCI_A2 RX
	gpsDMAConfiguration();

    resultPos = 0;
    sendPos = 0;

    timerInit(&speedContinuousModeConfig, &speedCaptureModeConfig);

    //myParamStruct.distance=20.0;
    //myParamStruct.speed=30.6;


    //Enabling NVIC
    Interrupt_enableMaster();

    //Mounting SDCard
    FRESULT r;
    /*First we should mount the SD Card into the Fatfs file system*/
    r = f_mount(&FS, "0", 1);
    /*Check for errors. Trap MSP432 if there is an error*/
    if(r != FR_OK){
        PRINTF("Error mounting SD Card, mount function returned: %d \r\n", (int)r);
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }else{
        PRINTF("Mounted SD Card!\r\n");
    }

    //Open the root directory on the SD Card
    r = f_opendir(&DI, "/");
    if(r != FR_OK){
        PRINTF("Could not open root directory, returned: %d\r\n", (int)r);
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
//        while(1);
    }else{
        PRINTF("Opened DIR!\r\n");
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }

    //Configuring GPIO for buttons
    MAP_GPIO_setAsInputPin(BTN_START_PORT, BTN_START_PIN);
    MAP_GPIO_setAsInputPin(BTN_STOP_PORT, BTN_STOP_PIN);

    //LCD configuration
    graphicsInitSelected(&SPI0MasterConfig);
    graphicsInitBigFont(&SPI0MasterConfig);
    graphicsInit(&SPI0MasterConfig);
    drawGrid1();

    //adcInit();
    temperatureInit();
    ADC14Init(&photoresistorUpModeConfig, &photoresistorCompareConfig);

    //BSS Init();
    _BSSInit(&bssUpConfig);
    Interrupt_enableMaster();   // Enabling MASTER interrupts

    uint8_t i;
    uint_fast16_t samplingAverage = 0;
    uint_fast16_t lightToSendAverage = 0;

    while(1){ 

        // BSS functions
        acquire_window(model);
        compute(model);
        classify(model);

        id = MPU6050_readDeviceId();

        //if wheel has completed one round, compute speed and distance travelled
        if(speedFlag){

//            printf("Register value: %d\n", getTimerAcapturedValue());
            myParamStruct.speed = speedCompute(getTimerAcapturedValue());
            myParamStruct.distance = distanceCovered();
//            printf("Speed: %f Km/h \n", myParamStruct.speed);
//            printf("Distance: %f m \n", myParamStruct.distance);
            speedFlag = false;
        }
            

        //if 4 light values have been captured, calculate average value and scale it (0 to 1)
        if(photoresFlag){
            for(i=0; i<LIGHT_BUFFER_LENGTH; i++){
                samplingAverage = samplingAverage + getResultBuffer()[i];
                // printf("resultBuffer[%d]: %d\n",i,getResultBuffer()[i]);
            }
            //printf("\n");
            // printf("samplingAverage: %d\n",samplingAverage);
            samplingAverage /= LIGHT_BUFFER_LENGTH;
            // printf("samplingAverage averaged: %d\n",samplingAverage);

            if(sendPos < 60){
                lightToSend[sendPos] = samplingAverage;
                //  printf("lightToSend[%d]: %d\n",sendPos, lightToSend[sendPos]);
            } else {
                // printf("in else\n");
                MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
                for(i=0; i<MAX_LIGHT_SAMPLES; i++){
                    lightToSendAverage += lightToSend[i];
                }
                lightToSendAverage /= MAX_LIGHT_SAMPLES;
                // printf("lightToSendAverage averaged: %d\n",lightToSendAverage);
                set_light(photoresistorConverter(lightToSendAverage));
                // printf("Converted average: %f \n\n",convertedAverage);
                sendPos = 0;
            }
            //ADC14_disableInterrupt(ADC_INT0);
            resultPos = 0;
//           printf("Average: %d\n",average);

            photoresFlag = false;

        }

        //if data is present, parse it
        if(gpsStringEnd == true){
            gpsParseData((char*)&gpsUartBuffer);
            getGpsData(&myParamStruct.sats, &myParamStruct2.speed, &myParamStruct.altitude, &myParamStruct2.hdop);
            gpsStringEnd = false;
            gpsAddPoint = true;
            gpsDMARestoreChannel();
        }
        //If temperature is read then convert it
        if (flagTemp){
            myParamStruct.temp = (conRes / calDifference) + 30.0f;
            //tempF = tempC * 9.0f / 5.0f + 32.0f;
            flagTemp = false;
        }

        //Computer FSM
        switch (computerState){
            case STOP:
                //Leds
                MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
                MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

                //Detecting Start button press
                status = !MAP_GPIO_getInputPinValue(BTN_START_PORT, BTN_START_PIN);
                if(status && !btnStartStateP){
                    btnStartStateP = true;
                    //Open the file
                    int fileIndex = 1;
                    r = f_stat(GPX_TEST_FILENAME, &FI);                     //Check if file already exists
                    if(r == FR_OK){                                         //If file already exists
                        defaultFile = false;
                        do{
                            snprintf(newFileName, 14, "test%d.gpx", fileIndex);
                            fileIndex++;
                            r = f_stat(newFileName, &FI);
                        }while(r != FR_NO_FILE && fileIndex <= 999);
                        r = f_open(&GPX_TEST_FILE, newFileName, FA_WRITE | FA_CREATE_ALWAYS);
                    }else if(r == FR_NO_FILE){
                        r = f_open(&GPX_TEST_FILE, GPX_TEST_FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
                    }
                    // r = f_open(&GPX_TEST_FILE, GPX_TEST_FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
                    /*Check for errors. Trap MSP432 if there is an error*/
                    if(r != FR_OK){
                        PRINTF("Could not open file, returned: %d\r\n", (int)r);
                        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                        while(1);
                    }

                    if(defaultFile){
                        GPXInitFile(&GPX_TEST_FILE, GPX_TEST_FILENAME);
                    }else{
                        GPXInitFile(&GPX_TEST_FILE, newFileName);
                    }
                    GPXAddTrack(&GPX_TEST_FILE, "2024-01-10T00:00:00Z");
                    GPXAddTrackSegment(&GPX_TEST_FILE);
                    computerState = START;
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
                    PRINTF("START TRACKING!!\r\n");
                }
                //LCD Update
                if(gpsAddPoint){
                    scrollPages();
                    showPages();
                    GrFlush(&g_sContext);
                    Interrupt_enableInterrupt(INT_ADC14);
                    gpsAddPoint = false;
                }

                btnStartStateP = status;
                break;
            case START:
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
                //Detecting Stop button press
                status = !MAP_GPIO_getInputPinValue(BTN_STOP_PORT, BTN_STOP_PIN);
                //Add point to GPX file and update LCD
                if(gpsAddPoint){
                    //GPX
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    bool pointAdded = addPointToGPXFromGPS(&GPX_TEST_FILE);
                    if(!pointAdded){
                        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
                    }

                    //LCD
                    scrollPages();
                    showPages();
                    GrFlush(&g_sContext);
                    Interrupt_enableInterrupt(INT_ADC14);
                    
                    gpsAddPoint = false;
                }
                if(status){
                    btnStopStateP = true;
                    GPXCloseTrackSegment(&GPX_TEST_FILE);
                    GPXCloseTrack(&GPX_TEST_FILE);
                    GPXCloseFile(&GPX_TEST_FILE);
                    computerState = STOP;
                    PRINTF("STOP TRACKING!!\r\n");

                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
                }
                //Go to sleep
                MAP_Interrupt_enableSleepOnIsrExit();
                MAP_PCM_gotoLPM0InterruptSafe();
                btnStopStateP = status;
                break;
        }

        //MAP_Interrupt_enableSleepOnIsrExit();
    }
}

/*! 
    @brief This interrupt happens every time a conversion has completed.
*/

/*void ADC14_IRQHandler(void)
{
    flagTemp = true;
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); 
    if (status & ADC_INT0){
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
    }
    Interrupt_disableInterrupt(INT_ADC14);
    Interrupt_disableSleepOnIsrExit();
}*/

/*void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    flagTemp = true;

    if (status & ADC_INT0){
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
        Interrupt_disableSleepOnIsrExit();
    } else if (status & ADC_INT3) {
        if(resultPos < 5) {
            resultsBuffer[resultPos++] = MAP_ADC14_getResult(ADC_MEM3);
        } else {
            photoresFlag = true;
            MAP_Interrupt_disableSleepOnIsrExit();
        }
    }
    

}*/
#else

#define GPX_TEST_FILENAME   "Test/results/test.gpx"
#define GPX_TEST_FILE       file
int main(void){
    FILE* NMEA;
    FILE* GPX;
    NMEA = fopen("Test/NMEAFileCorrected.txt", "r");
    if(NMEA == NULL){
        printf("Error opening NMEA file!\r\n");
        return -1;
    }
    GPX = fopen(GPX_TEST_FILENAME, "w");
    do{
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
        PRINTF("Get command: P/s [Play/Stop]\r\n");
        fflush(stdin);
        char cmd = getchar();
        switch(cmd){
            case 'P':
            case 'p':
                PRINTF("Start!\r\n");
                GPXInitFile(GPX, GPX_TEST_FILENAME);
                GPXAddTrack(GPX, "Test Track", "Test Description", "2024-01-10T00:00:00Z");
                GPXAddTrackSegment(GPX);
                computerState = START;
                break;
            case 'C':
            case 'c':
                PRINTF("Continue!\r\n");
                break;
            case 'S':
            case 's':
                PRINTF("Stop!\r\n");
                GPXCloseTrackSegment(&GPX);
                GPXCloseTrack(&GPX);
                GPXCloseFile(&GPX);
                computerState = STOP;
                break;
        }
        if(computerState == START){
            char gpsData[RX_BUFFER_SIZE];
            fread(gpsData, sizeof(char), RX_BUFFER_SIZE, NMEA);
            gpsStringEnd = true;
            addPointToGPXFromGPS(gpsData, &GPX);
        }
    }while(computerState != STOP);
	return 0;
}
#endif
