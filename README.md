# Bike Computer

Embedded Software for the IoT project

Authors:
- [@AlbertoDB20](https://github.com/AlbertoDB20)
- [@alanmasu](https://github.com/alanmasu)
- [@sofiaz17](https://github.com/Sofiaz17)
- [@kikalore](https://github.com/kikalore)
- [@pietrocipriani](https://github.com/pietrocipriani) (as external collaborator)


The goal of the project is the realization of a bike computer equipped with many functions common to many odometers on market today, such as reporting instantaneous speed, average speed, clock, distance travelled but also more advanced statistics such as altitude, gps coordinates, automatic management of lights based on external ambient light and a safe system which, in the event of sudden braking, flashes the rear lights to warn other cyclists or cars of the emergency.

The computer will therefore be equipped with the following components/sensors:
- lcd monitor
- Arduino or esp32 or other
- KY-024 Linear magnetic Hall Sensor
- Light Dependent Resistor
- front and rear lights
- gps Sensor
- DHT 11 or other temperature sensor
- MPU 6050 accellerometer sensor
- functional botton
- battery
- ??? other sensors???

The functionality of this bike computer is identical to the commercial ones, there is a main interface where it’s possible to read trip statistics and a menù interface where the user can set technical data of bicycle (for example the size of the wheel), the unit of measurement, …

Ideas for Project:
1) Rather then using a lcd monitor, we can use the screen of a mobile phone connected to the bike computer via Bluetooth
	+ more “IoT” project
	+ more effective
	+ everybody has his phone with him
	- more complex
2) being able to download GPX tracks (possibly via bluetooth on your phone) to create a map with an online tool;
3) have the possibility to personalize the main interface of odometer
4) create a PLA body to contain all the component of the bike computer


Google Drive link:
https://drive.google.com/drive/folders/1Llw7UDmaANK1lYZJ6caN6bu0Kb6bvCJu?usp=sharing

## Project Documentation

- [main.c](#main.c)
- GPX Library
  - [gpx.c](#gpx.c)
  - [gpx.h](#gpx.h)
- GPS API
  - [GPS.h](#GPS.h)
  - [GPS.c](#GPS.c)
- DMA API
  - [DMAModule.h](#DMAModule.h)
  - [DMAModule.c](#DMAModule.c)