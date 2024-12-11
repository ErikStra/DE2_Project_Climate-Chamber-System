# BPC/DE2_Project_Climate_Chamber_System

## Description
 - The goal of the project would be to create a system that measures key environmental parameters (such as temperature, humidity, light levels, soil moisture) for tropical plants.
 - This system should also allow the user to control or adjust environmental conditions and visualize the data.

## Team members

- **Vít Vašenka** Responsible for RTC, LED and pump control


- **Jindřich Zobač** responsible for UI, main.c, UART communication


- **Erik Straka** responsible for sensors readings


- **Artem Kostenko** responsible for fan control, Readme.md


## Theoretical description and explanation

Tropical plants require specific environmental conditions to thrive, including:

 1. Temperature: Maintaining a specific temperature range.
 2. Humidity: Ensuring consistent moisture levels in the air.
 3. Light: Providing the right light intensity and duration.
 4. Soil Moisture: Monitoring and watering plants appropriately.

The goal is to build an automated system that:

 1. Monitors these parameters using sensors.
 2. Controls devices like fans, lights, and water pumps to maintain ideal conditions.
 3. Allows users to set preferences and view current conditions.


## Hardware description of demo application

## 0. Block diagram of the whole setup
![Block diagram](images/schema.jpg)

## 1. Sensors for Monitoring
- **DHT Sensor**: Measures temperature and humidity to monitor the environment.
- **BMP280 Sensor**: Provides additional environmental data, including pressure and temperature.
- **RTC**: Measures real-time clock to support time-based operations.

## 2. Actuators for Control
- **Fans**: Regulate temperature and humidity.
- **Lights**: Provide artificial lighting for photosynthesis.
- **Water Pump**: Irrigates the soil automatically when moisture levels are low.

## 3. User Interface and communication with PC
- **LCD Display**: Displays the current environmental conditions to the user.
- **UART Communication**: Allows users to adjust settings via a terminal interface.
![inicialization after starup](images/screen_init.png)

stats command yelds string in csv-like format of current values of modules variables in format TEMP1;TEMP2;HUM1;HUM2;LED;fan_big;fan_led;wlevel;hours;minutes;secs
![stats command](images/screen_stats.png)

settings variables can be uploaded via upload command in format upload(MaxT1*10 MaxT2*10 MaxH1 MinH2 SR SS water_time KP1*10 KI1*10 KD1*10)
![upload command](images/screen_upload.png)

## 4. Control Logic
- **PID Control**: We implemented precise fan operation using PID control logic.
- **Lighting Schedule**: We used a real-time clock (RTC) to adjust lighting schedules dynamically based on sunrise and sunset times.
- **Automated Watering**: We automated soil irrigation based on moisture levels detected by sensors.

## 5. Microcontroller
- We used an **Arduino Nano (ATmega328P)** to interface with all sensors and control actuators.


## Software description
- We developed modular code with clear responsibilities:
  - **fan_PID.c**: Handles fan speed control using PID logic.
  - **rtc_control.c**: Manages time-based operations and schedules.
  - **outputControl.c**: Controls GPIO outputs, including lights and pumps.
  - **variables.c**: Stores shared variables between different modules.
 

- For efficient task management, we used flags and timers to organize and prioritize operations.

Variables flowchart

![flowchart_diagram](images/DE_2_diagram.svg)

flowchart for [fan_PID.c](lib/fan_PID.c)    [fan_PID README](lib/fan_senzor/fan_PID.md)

![fan_PID](images/fan_PID.svg)

flowchart for [rtc_control.c](lib/rtc_control.c)   [rtc_control README](lib/RTC/rtc_control.md)

![rtc_control](images/rtc_control.svg)

flowchart for [main.c](src/main.c)    [main README](src/main.md)

![rtc_control](images/main.svg)

## Instructions
###Screens
- 1: displays temp. of air, humidity of air and humidity of soil, time
- 2: informayion regarding LED light
- 3: information about state of fans
- 4: information about water level, pump and time of autowatering
- 5: Screen to adjust system variables like max. temp, max. humidity etc.

- Control: connect NANO to PC and inicialize UART communication.
- loop trough screens 1-5 by pressing b key
- on screen 5 (settings)

![box](images/box.jpg)
![roof](images/roof.jpg)
![mainboard](images/mainboard.jpg)
![homescreen](images/homescreen.jpg)


## Testing and development

![rtc_control](images/main.svg)

## References
1. [ADC from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/archive/labx-adc)
2. [UART from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/lab5-uart)
3. [twi from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/solutions/lab6-i2c-scan/lib/twi)
4. [GPIO from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/lab2-gpio)
5. [Datasheet of ATmega328p](https://www.microchip.com/en-us/product/ATmega328p)
6. [Draw.io](https://app.diagrams.net)
7. [Mermaid Editor](https://mermaid-js.github.io)
8. [ChatGPT](https://chatgpt.com)
