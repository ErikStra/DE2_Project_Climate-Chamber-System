# **Dual Fan PID Control System**

## **Overview**
This project implements a dual-fan control system using PID (Proportional-Integral-Derivative) logic on an AVR microcontroller. The system dynamically adjusts fan speeds based on temperature readings to maintain desired environmental conditions.

---

## **Features**
1. **Dual Fan Control**:
   - Independent control of two fans based on temperature inputs.
   - Precise regulation using PID logic.

2. **Modular Design**:
   - Clear separation of initialization, PID calculation, and control loop functionality.

3. **Output Scaling**:
   - Converts PID output to PWM signals for fan speed adjustment.

---

## **Dependencies**
### **Header Files**
- `<avr/io.h>`: I/O registers and ports for the AVR microcontroller.
- `<avr/interrupt.h>`: Support for enabling and managing interrupts.
- `"uart.h"`: UART communication library.
- `"pwm.h"`: PWM initialization and control.
- `"variables.h"`: Shared global variables.
- `"fan_PID.h"`: Fan control function declarations.

---

## **Definitions**
- **`#define F_CPU 16000000L`**: Specifies the microcontroller clock frequency (16 MHz).
- **Global Variables**:
  - `prev_error1, prev_error2`: Store previous error values for each fan.
  - `integral1, integral2`: Store accumulated integral error values for each fan.

---

## **Functions**

### **1. `uint8_t pid_control()`**
**Purpose**: Computes the PWM duty cycle for a fan using PID logic.

- **Parameters**:
  - `actual_temp` (int16_t): Current temperature reading.
  - `target_temp` (int16_t): Desired target temperature.
  - `prev_error` (int16_t*): Pointer to the previous error value.
  - `integral` (int32_t*): Pointer to the accumulated error value.
  - `KP, KI, KD` (uint8_t): PID constants for proportional, integral, and derivative terms.

- **Algorithm**:
  1. Calculate the error: `error = target_temp - actual_temp`.
  2. Update the integral term and limit it (anti-windup).
  3. Calculate the derivative term based on the error difference.
  4. Compute the PID output:
     \[
     \text{output} = K_P \times \text{error} + K_I \times (\text{integral} / 10) + K_D \times \text{derivative}
     \]
  5. Clamp the output to the range `[0, 255]`.

- **Returns**: A PWM duty cycle value (`uint8_t`) in the range `[0, 255]`.

---

### **2. `int fan_PID_init()`**
**Purpose**: Initializes the fan control system.

- **Actions**:
  - Calls `pwm_init()` to set up PWM peripherals.
  - Sends an initialization message via UART: `"[INFO] PID system initialized.\r\n"`.

- **Returns**: `0` upon successful initialization.

---

### **3. `int fan_PID_loop()`**
**Purpose**: Implements the main fan control logic.

- **Algorithm**:
  - **Fan 1**:
    - If `TEMP1 > max_temp1`, calculate PWM using `pid_control()`.
    - Set `fan_big` to the computed PWM value, or `0` if the condition fails.
  - **Fan 2**:
    - If `TEMP2 > max_temp2 * 10`, calculate PWM using `pid_control()`.
    - Set `fan_led` to the computed PWM value, or `0` if the condition fails.

- **Returns**: `0` after each iteration.

---

## **Usage**
1. **Initialization**:
   - Call `fan_PID_init()` to initialize the system, including PWM setup.

2. **Control Loop**:
   - Run `fan_PID_loop()` in a continuous loop to regulate environmental conditions.

---

## **Control Logic**
- **PID Parameters**:
  - `KP`: Proportional gain.
  - `KI`: Integral gain.
  - `KD`: Derivative gain.

- **Temperature Control**:
  - The system monitors `TEMP1` and `TEMP2` to adjust fan speeds and maintain desired temperatures.

- **Output Scaling**:
  - PID outputs are scaled to the range `[0, 255]` for compatibility with PWM.

---

## **Potential Enhancements**
1. Add humidity-based fan control for improved environmental regulation.


---

## **Conclusion**
This dual-fan PID control system is a robust and modular solution for temperature management. Its use of interrupts, PWM, and PID logic ensures precise and efficient operation, making it ideal for embedded climate control systems.
