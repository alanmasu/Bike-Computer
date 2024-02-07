# BIKE COMPUTER

"Bike computer" is a university project for the course of Embedded Software for the IoT, at the University of Trento.

Authors:
- [@AlbertoDB20](https://github.com/AlbertoDB20)
- [@alanmasu](https://github.com/alanmasu)
- [@sofiaz17](https://github.com/Sofiaz17)
- [@kikalore](https://github.com/kikalore)

The goal of the project is the realization of a bike computer equipped with many functions common to many odometers on the market today, such as reporting instantaneous speed, average speed, clock, and distance traveled but also more advanced statistics such as altitude, GPS coordinates, automatic management of lights based on external ambient light and a safe system which, in the event of sudden braking, flashes the rear lights to warn other cyclists or cars of the emergency.

## Main functionality and hardware required
The computer will therefore be equipped with the following components/sensors:
- #### MSP432 and Boosterpack MKII
- #### LCD monitor
- #### Magnetic Hall Sensor
- #### Light Dependent Resistor
- #### Front and rear lights
- #### GPS Sensor
- #### MPU 6050 accellerometer sensor
- #### SD card
- #### battery


The functionality of this bike computer is identical to the commercial ones, there is a main interface where it’s possible to read trip statistics and a menù interface on the third page where the user can set technical data of the bicycle (for example the size of the wheel and choice the measurement units). Moreover, on the second page, you can check the GPS status.
Thanks to GPS, the user can download GPX tracks (possibly via Bluetooth on your phone) to create a map with an online tool.
All parts built to contain the project's components, like the MSP and other sensors, are made in PLA at the Fablab laboratory, at the University of Trento.

## Some improvements:
- Rather than using an LCD monitor, we can use the screen of a mobile phone connected to the bike computer via Bluetooth
	+ more “IoT” project
	+ more effective
	+ Everybody has his phone with him
	- more complex

## How to build the project:
  - Installing Code Composer studio
  - Download the DriverLib from Moodle and unpack it in a directory that you prefer
  - Open CCS Preferences (Window > Preferences)
  - Go to Code Composer Studio > Build > Variables
  - Add those ambient variables:
      - name: IOT_SDK_DRIVERLIB_FILE    type: file    value: add your local path where you have the
        following: / source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib in your simplelink directory.
      - name: IOT_SDK_GRLIB_FILE    type: file    value: add your local path where you have the following: 
        /source/ti/grlib/lib/ccs/m4f/grlib.a in your simplelink directory.
      - name: IOT_SDK_INCLUDE_PATH  type: directory   value: add your local path where you have your simplelink
        directory.
  - Click on Apply and Close for exit
  - The following steps are needed only if the project is new! 
      - Now Right click on the project > Show Build Settings > Arm Compiler > Include Options  and Add a new
        line (or edit if it is present) with Variables > select IOT_SDK_INCLUDE_PATH > OK, and then add /source at the end. So the result is
        ${IOT_SDK_INCLUDE_PATH}/source.
      - Go to Arm Linker > File Search Path  ADD a new library by clicking on the icon with a green +, now
        Variables > IOT_SDK_DRIVERLIB_FILE > OK
      - Repeat also for the GRLIB file
Here you can find some useful files, used during the development -> Google Drive link:
https://drive.google.com/drive/folders/1Llw7UDmaANK1lYZJ6caN6bu0Kb6bvCJu?usp=sharing

## Project Documentation

- [main.c](#main.c)
- Speed module
	- [speed.c](#speed.c)
	- [speed.h](#speed.h)
- GPX Library
  - [GPX.c](#gpx.c)
  - [GPX.h](#gpx.h)
- GPS API
  - [GPS.h](#GPS.h)
  - [GPS.c](#GPS.c)
- DMA API
  - [DMAModule.h](#DMAModule.h)
  - [DMAModule.c](#DMAModule.c)
- ADC module
  - [adc.c](#adc.c)
  - [adc.h](#adc.h)
- Temperature module
  - [temperature.c](#temperature.c)
  - [temperature.h](#temperature.h)
- LCD module
  - [mainInterface.c](#mainInterface.c)
  - [mainInterface.h](#mainInterface.h)

