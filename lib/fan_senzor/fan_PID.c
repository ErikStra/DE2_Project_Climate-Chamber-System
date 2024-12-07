#include <avr/io.h>  
#include <avr/interrupt.h> 
#include <stdio.h> 
#include <string.h> // For memset 
#include "../../include/timer.h" 
#include <uart.h> 
#include <pwm.h> 
#include <variables.h> 
 
#define F_CPU 16000000L 
 
// PID variables 
float kp = 5.0;  // Proportional gain 
float ki = 0.5;  // Integral gain 
float kd = 1.0;  // Derivative gain 
 
float prev_error1 = 0;  // Previous error for TEMP1 
float integral1 = 0;    // Integral term for TEMP1 
 
float prev_error2 = 0;  // Previous error for TEMP2 
float integral2 = 0;    // Integral term for TEMP2 
 
int target_temp = 25;  // Target temperature 
 
volatile uint8_t timer_flag = 0; // Flag for timer interrupt 
 
// Timer overflow interrupt service routine 
ISR(TIMER1_OVF_vect) { 
    timer_flag = 1; // Set the flag when timer overflows 
} 
 
// PID control function 
uint8_t pid_control(float current_temp, float *prev_error, float *integral) { 
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
    if (TEMP1 > target_temp) { 
        uint8_t pwm1 = pid_control(TEMP1, &prev_error1, &integral1); 
        pwm_set_duty_cycle(pwm1);  
        sprintf(buffer, "Fan 1 ON at %d%%\r\n", (pwm1 * 100) / 255); 
    } else { 
        pwm_set_duty_cycle(0); // Turn off fan 
        sprintf(buffer, "Fan 1 OFF\r\n"); 
    } 
    uart_puts(buffer); 
 
    // Fan 2 (TEMP2) 
    memset(buffer, 0, sizeof(buffer)); // Clear buffer 
    if (TEMP2 > target_temp) { 
        uint8_t pwm2 = pid_control(TEMP2, &prev_error2, &integral2); 
        pwm_set_duty_cycle_2(pwm2);  
        sprintf(buffer, "Fan 2 ON at %d%%\r\n", (pwm2 * 100) / 255); 
    } else { 
        pwm_set_duty_cycle_2(0); // Turn off fan 
        sprintf(buffer, "Fan 2 OFF\r\n"); 
    } 
    uart_puts(buffer); 
} 
 
// Initialize system 
int FanSenzor_init(void) { 
    uart_init(UART_BAUD_SELECT(9600, F_CPU));  
    pwm_init();  
 
    // Timer initialization 
    TIM1_ovf_262ms();         // Set timer overflow to 262 ms 
    TIM1_ovf_enable();        // Enable timer overflow interrupt 
    sei();                    // Enable global interrupts 
 
    uart_puts("System initialized.\r\n"); 
    return 0;  
} 
 
// Main loop for fan control 
int FanSenzor_loop(void) {  
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
