#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h> // For memset
#include "uart.h"
#include "pwm.h"
#include "variables.h"
#include "fan_PID.h"

#define F_CPU 16000000L

// PID variables
float kp = 5.0;  // Proportional gain
float ki = 0.5;  // Integral gain
float kd = 1.0;  // Derivative gain

float prev_error1 = 0;  // Previous error for TEMP1
float integral1 = 0;    // Integral term for TEMP1

float prev_error2 = 0;  // Previous error for TEMP2
float integral2 = 0;    // Integral term for TEMP2

//volatile uint8_t timer_flag = 0; // Flag for timer interrupt, no longer needed (handled in main)

/* 
// Timer overflow interrupt service routine 
void fan_PID_interrupt(void) {
    timer_flag = 1; // Set the flag when timer overflows 
}
*/

// PID control function
uint8_t pid_control(float target_temp, float current_temp, float *prev_error, float *integral) {
    float error = current_temp - target_temp;  // Error should be positive if temp > target
    *integral += error;                        // Calculate integral term

    // Clamp the integral term to avoid wind-up
    if (*integral > 50) *integral = 50;
    if (*integral < -50) *integral = -50;

    float derivative = error - *prev_error;    // Calculate derivative term
    *prev_error = error;                       // Update previous error

    float output = (kp * error) + (ki * *integral) + (kd * derivative); // PID formula

    // Clamp output to valid PWM range (0-255)
    if (output > 255) output = 255;
    if (output < 0) output = 0;

    return (uint8_t)output; // Return as 8-bit integer
}

// Fan control with conditional PID
void fan_control_pid(void) {
    char buffer[50];

    // Fan 1 (TEMP1)
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    if (TEMP1 > max_temp1) {
        uint8_t pwm1 = pid_control(max_temp1, TEMP1, &prev_error1, &integral1);
        pwm_set_duty_cycle_1(pwm1);  // Use the updated function for Fan 1 (PD5)
        sprintf(buffer, "Fan 1 ON at %d%%\r\n", (pwm1 * 100) / 255);
    } else {
        pwm_set_duty_cycle_1(0); // Turn off Fan 1
        sprintf(buffer, "Fan 1 OFF\r\n");
    }
    uart_puts(buffer);

    // Fan 2 (TEMP2)
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    if (TEMP2 > max_temp2) {
        uint8_t pwm2 = pid_control(max_temp2, TEMP2, &prev_error2, &integral2);
        pwm_set_duty_cycle_2(pwm2);  // Use the updated function for Fan 2 (PD6)
        sprintf(buffer, "Fan 2 ON at %d%%\r\n", (pwm2 * 100) / 255);
    } else {
        pwm_set_duty_cycle_2(0); // Turn off Fan 2
        sprintf(buffer, "Fan 2 OFF\r\n");
    }
    uart_puts(buffer);
}

// Initialize system
int fan_PID_init(void) {
    pwm_init_PD5();  // Initialize PWM for Fan 1 (PD5)
    pwm_init_PD6();  // Initialize PWM for Fan 2 (PD6)

    uart_puts("System initialized.\r\n");
    return 0;
}

// Main loop for fan control
int fan_PID_loop(void) {  // Renamed function to match file name consistency
    char buffer[20];

    // Output current TEMP1 and TEMP2
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    sprintf(buffer, "TEMP1: %d°C\r\n", TEMP1);
    uart_puts(buffer);

    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    sprintf(buffer, "TEMP2: %d°C\r\n", TEMP2);
    uart_puts(buffer);

    // Control fans with conditional PID
    fan_control_pid();

    return 0;
}

/* 
// Main function (runs in main program)
int main(void) {
    FanSenzor_init(); // Initialize system

    while (1) {
        if (timer_flag) { // Check timer flag
            timer_flag = 0; // Clear the flag
            FanSenzor_loop(); // Execute main loop logic
        }
    }

    return 0; // This will never be reached
}
*/
