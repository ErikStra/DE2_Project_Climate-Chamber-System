# BPC/DE2_Project_Climate_Chamber_System

## Description
 - The goal of the project would be to create a system that measures key environmental parameters (such as temperature, humidity, light levels, soil moisture) for tropical plants.
 - This system should also allow the user to control or adjust environmental conditions and visualize the data.

## Team members

- **Vít Vašenka**


- **Jindřich Zobač**


- **Erik Straka**


- **Artem Kostenko**


## Theoretical description and explanation

Tropical plants require specific environmental conditions to thrive, including:

 1. Temperature: Maintaining a specific temperature range.
 2. Humidity: Ensuring consistent moisture levels in the air.
 3. Light: Providing the right light intensity and duration.
 4. Soil Moisture: Monitoring and watering plants appropriately.

The goal is to build an automated system that:

 • Monitors these parameters using sensors.
 • Controls devices like fans, lights, and water pumps to maintain ideal conditions.
 • Allows users to set preferences and view current conditions.


## Hardware description of demo application

Insert descriptive text and schematic(s) of your implementation.

## Software description
Variables flowchart

![flowchart_diagram](images/DE_2_diagram.svg)

flowchart for fan_PID.c

![fan_PID](images/fan_PID.svg)

flowchart for rtc_control.c

![rtc_control](images/rtc_control.svg)

flowchart for main.c

![rtc_control](images/main.svg)









Put flowchats/state diagrams of your algorithm(s) and direct links to source/testbench files in src and sim folders.

## Component(s) simulation

Write descriptive text and put simulation screenshots of your components.

## Instructions

Write an instruction manual for your application, including photos and a link to a short app video.

## References
1. [ADC from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/archive/labx-adc)
2. [UART from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/lab5-uart)
3. [twi from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/solutions/lab6-i2c-scan/lib/twi)
4. [GPIO from GitHub of Tomas Fryza](https://github.com/tomas-fryza/avr-course/tree/master/lab2-gpio)
5. [Datasheet of ATmega328p](https://www.microchip.com/en-us/product/ATmega328p)
6. [Draw.io](https://app.diagrams.net)
7. [Mermaid Editor](https://mermaid-js.github.io)
8. [ChatGPT](https://chatgpt.com)
