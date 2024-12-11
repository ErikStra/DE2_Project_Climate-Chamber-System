# **RTC-Controlled System with EEPROM Support**

## **Overview**
This project implements a real-time clock (RTC) controlled system using an AVR microcontroller. It supports sunrise and sunset time settings stored in EEPROM and allows for automated actions based on time and environmental conditions.

---

## **Features**
1. **RTC Integration**:
   - Real-time clock (RTC) for timekeeping.
   - Synchronizes operations based on real-time data.

2. **EEPROM Support**:
   - Stores sunrise and sunset times for persistent configuration.
   - Automatically loads default values if EEPROM data is invalid.

3. **Light Control**:
   - Adjusts LED states based on sunrise and sunset times.

4. **Time Comparison**:
   - Compares current time with sunrise and sunset to determine actions.

---

## **Dependencies**
### Header Files
- `<avr/io.h>`: Defines I/O registers and ports for AVR.
- `<avr/interrupt.h>`: Provides support for enabling/disabling interrupts.
- `<avr/eeprom.h>`: Enables reading/writing data to EEPROM.
- `"uart.h"`: UART communication library for debugging.
- `"timer.h"`: Timer library for AVR-GCC.
- `"rtc_control.h"`: RTC management functions.
- `"variables.h"`: Shared variables and configurations.

---

## **Definitions**
- **`#define RTC_ADR 0x68`**: RTC I2C address.
- **EEPROM Addresses**:
  - `EEPROM_SUNRISE_HOURS_ADDR`: Address for sunrise hour data.
  - `EEPROM_SUNRISE_MINUTES_ADDR`: Address for sunrise minute data.
  - `EEPROM_SUNSET_HOURS_ADDR`: Address for sunset hour data.
  - `EEPROM_SUNSET_MINUTES_ADDR`: Address for sunset minute data.
  - `EEPROM_LIGHT_STATE_ADDR`: Address for LED state.

---

## **Functions**

### **1. rtc_control_init()**
**Purpose**: Initializes the RTC and loads sunrise/sunset times from EEPROM.

- **Actions**:
  - Reads sunrise and sunset settings from EEPROM.
  - Initializes the RTC hardware.
  - Verifies RTC communication and logs errors if unavailable.

---

### **2. rtc_control_loop()**
**Purpose**: Executes control logic based on the current time.

- **Algorithm**:
  - Calls `compare_time_with_sun()` to determine if the LED should be on or off.
  - Logs results over UART for debugging.

---

### **3. rtc_set_time()**
**Purpose**: Sets the RTC time to the provided values.

- **Parameters**:
  - `hours`: Hour to set.
  - `minutes`: Minute to set.
  - `secs`: Second to set.

- **Process**:
  - Sends the time data to the RTC over I2C.
  - Converts the input values to BCD format before sending.

---

### **4. rtc_initialize()**
**Purpose**: Validates and initializes the RTC time.

- **Process**:
  - Reads the current RTC seconds to check for valid time.
  - If invalid, sets the time using the compilation timestamp (`__TIME__`).

---

### **5. compare_time_with_sun()**
**Purpose**: Compares the current hour with sunrise and sunset times to control the LED.

- **Parameters**:
  - `currentHour`: Current hour.
  - `sunrise`: Sunrise hour.
  - `sunset`: Sunset hour.

- **Algorithm**:
  - If `sunset < sunrise`, LED is ON when the current time is before sunset or after sunrise.
  - Otherwise, LED is ON only between sunrise and sunset.
  - Updates the LED state in EEPROM.

---

### **6. rtc_read_time()**
**Purpose**: Reads the current time from the RTC.

- **Actions**:
  - Fetches seconds, minutes, and hours in BCD format.
  - Converts them to decimal format for processing.

---

### **7. load_settings_from_eeprom()**
**Purpose**: Loads sunrise and sunset settings from EEPROM.

- **Actions**:
  - Reads data from EEPROM addresses.
  - Sets default values if EEPROM data is invalid.

---

### **8. save_settings_to_eeprom()**
**Purpose**: Saves sunrise and sunset settings to EEPROM.

- **Actions**:
  - Writes the current sunrise and sunset settings to EEPROM addresses.

---

### **9. bcdToDec()**
**Purpose**: Converts a BCD-encoded value to decimal.

- **Algorithm**:
  - Converts BCD to decimal using:
    \[
    \text{decimal} = (\text{value} / 16 \times 10) + (\text{value} \% 16)
    \]

---

### **10. decToBcd()**
**Purpose**: Converts a decimal value to BCD.

- **Algorithm**:
  - Converts decimal to BCD using:
    \[
    \text{BCD} = (\text{value} / 10 \times 16) + (\text{value} \% 10)
    \]

---

## **Usage**

### Initialization
1. Call `rtc_control_init()` to initialize the RTC and load EEPROM data.
2. If the RTC is invalid, the system sets the time using the compile timestamp.

### Main Loop
- Continuously call `rtc_control_loop()` to:
  - Compare the current time with sunrise and sunset.
  - Update LED states based on time conditions.

---

## **Control Logic**
1. **Time Management**:
   - Uses `rtc_read_time()` to fetch the current time.
   - Converts BCD-encoded values to decimal for processing.

2. **EEPROM Persistence**:
   - Stores sunrise and sunset times for persistence across power cycles.

3. **LED Control**:
   - Turns LED ON/OFF based on the comparison between current time, sunrise, and sunset.

---

## **Potential Enhancements**
- Add support for dynamic time adjustments via UART commands.
- Implement logging for RTC synchronization events.
- Extend support for multiple lighting schedules.

---

## **Conclusion**
This RTC-controlled system provides a robust and efficient way to manage time-based operations, including sunrise/sunset lighting adjustments. Its integration with EEPROM ensures persistent settings, making it ideal for embedded time-sensitive applications.
