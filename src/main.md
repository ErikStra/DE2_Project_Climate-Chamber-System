# **Main System for Environmental Control**

## **Overview**
This project implements an environmental monitoring and control system using an AVR microcontroller. The system integrates temperature, humidity, and pressure sensors, along with fans and other actuators, to maintain and monitor desired environmental conditions.

---

## **Features**
1. **Sensor Integration**:
   - **DHT Sensor**: Measures temperature and humidity.
   - **BMP280 Sensor**: Measures temperature and pressure.

2. **Actuator Control**:
   - **Fans**: Controlled using PID logic for precise environmental regulation.
   - **GPIO Outputs**: Manage additional peripherals such as lights or pumps.

3. **Real-Time Clock (RTC)**:
   - Synchronizes time-based operations and schedules.

4. **User Interface**:
   - UART communication for command input and status updates.
   - A modular interface for ease of interaction and control.

5. **Interrupt-Driven System**:
   - Efficiently manages periodic tasks using timer-based interrupts.

---

## **Dependencies**
### **Header Files**
- `<avr/io.h>`: Defines I/O registers and port operations for AVR.
- `<avr/interrupt.h>`: Provides interrupt handling functionality.
- `<avr/eeprom.h>`: Enables EEPROM read/write operations.
- `"uart.h"`: UART communication library.
- `"timer.h"`: Timer setup and management library.
- `"rtc_control.h"`: Handles RTC-based operations.
- `"UserInterface.h"`: Manages user interactions.
- `"fan_PID.h"`: Implements fan speed control using PID logic.
- `"outputControl.h"`: Controls GPIO outputs.
- `"DHT.h"`: Interfaces with the DHT sensor for temperature and humidity.
- `"bmp280.h"`: Interfaces with the BMP280 sensor for temperature and pressure.

---

## **Code Structure**
### **Flags**
- `flag_UI_getcommand`: Signals the need to process a UART command.
- `flag_UI_display_loop`: Triggers periodic updates of the user interface.
- `flag_fan_PID`: Indicates the need to update fan control.
- `flag_RTC`: Activates RTC-related operations.
- `flag_outputControl`: Signals output GPIO updates.
- `flag_dht_update_temp1`: Indicates the need to update DHT sensor readings.
- `flag_bmp280_update_temp_press`: Signals BMP280 sensor updates.

### **Variables**
- `n_ovfs`: Counts timer overflows for scheduling tasks.
- `DHT_error_count`: Tracks consecutive DHT sensor errors.
- `IDK`: Adjusts error reporting thresholds for the DHT sensor.
- `command_buffer`: Stores UART commands for processing.

---

## **Main Functions**

### **1. Initialization**
**Purpose**: Sets up all system components and peripherals.

- **Modules Initialized**:
  - Timers using `TIM2_ovf_16ms()` and `TIM2_ovf_enable()`.
  - UART communication at 9600 baud using `uart_init()`.
  - Peripheral modules:
    - `UserInterface_init()`
    - `rtc_control_init()`
    - `fan_PID_init()`
    - `outputControl_init()`

---

### **2. Main Loop**
**Purpose**: Executes the primary control logic in a loop.

- **Tasks**:
  - Processes UART commands if `flag_UI_getcommand` is set.
  - Updates the user interface display when `flag_UI_display_loop` is set.
  - Executes fan control logic when `flag_fan_PID` is set.
  - Reads time and executes RTC control if `flag_RTC` is set.
  - Updates GPIO outputs when `flag_outputControl` is set.
  - Reads and processes DHT sensor data when `flag_dht_update_temp1` is set.
  - Reads BMP280 data for temperature and pressure when `flag_bmp280_update_temp_press` is set.

---

### **3. Timer Overflow ISR**
**Purpose**: Manages task scheduling through periodic interrupts.

- **Intervals**:
  - Every **130 ms**: Sets `flag_UI_getcommand`.
  - Every **200 ms**: Sets `flag_UI_display_loop` and `flag_fan_PID`.
  - Every **500 ms**: Sets `flag_RTC`.
  - Every **1 second**: Sets `flag_outputControl` and `flag_dht_update_temp1`.
  - Every **4 seconds**: Reserved for future tasks.

- **Counter Management**:
  - Increments `n_ovfs` and resets it to `0` after reaching `500`.

---

## **Sensor Functions**

### **1. DHT Sensor**
- **Purpose**: Reads temperature and humidity.
- **Error Handling**:
  - Logs errors if data cannot be read consecutively.
  - Dynamically adjusts error reporting thresholds.

### **2. BMP280 Sensor**
- **Purpose**: Reads temperature and pressure.
- **Output**:
  - Updates `TEMP2` and `PRESSURE` with the sensor readings.
  - Sends data via UART for debugging.

---

## **Actuator Control**

### **Fan Control**
- **Logic**: Utilizes PID control for precise speed adjustments.
- **Updates**:
  - Triggered periodically based on the `flag_fan_PID`.

### **GPIO Outputs**
- **Purpose**: Manages peripheral devices like lights or pumps.
- **Updates**:
  - Triggered periodically using the `flag_outputControl`.

---

## **UART Interface**
- **Commands**: Processes UART inputs for user control.
- **Outputs**:
  - Sends system status and debug messages via UART.

---

## **Potential Enhancements**
1. Add dynamic configuration for sensor intervals via UART.
2. Implement more advanced error handling for sensor faults.
3. Extend support for additional sensors or actuators.

---

## **Conclusion**
This environmental control system combines modular design, efficient interrupt-based task scheduling, and comprehensive sensor integration. Its flexible design allows for further expansion, making it suitable for various embedded applications.
