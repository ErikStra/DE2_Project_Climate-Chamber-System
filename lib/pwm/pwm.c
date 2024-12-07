#include "pwm.h"

void pwm_init_PD5(void) {
    // Set PD5 as output
    DDRD |= (1 << DDD5);

    // Configure Timer0
    TCCR0A = (1 << WGM00) | (1 << WGM01); // Set Fast PWM Mode
    TCCR0A |= (1 << COM0B1);              // Enable non-inverting mode for OC0B (PD5)
    TCCR0B = (1 << CS01);                 // Set prescaler to 8 (PWM frequency = F_CPU / (8 * 256))
    
    OCR0B = 0; // Initialize PWM with 0% duty cycle on OC0B (PD5)
}

void pwm_init_PD6(void) {
    // Set PD6 as output
    DDRD |= (1 << DDD6);

    // Configure Timer0
    TCCR0A = (1 << WGM00) | (1 << WGM01); // Set Fast PWM Mode
    TCCR0A |= (1 << COM0A1);              // Enable non-inverting mode for OC0A (PD6)
    TCCR0B = (1 << CS01);                 // Set prescaler to 8 (PWM frequency = F_CPU / (8 * 256))
    
    OCR0A = 0; // Initialize PWM with 0% duty cycle on OC0A (PD6)
}

void pwm_set_duty_cycle_1(uint8_t duty_cycle) {
    // Clamp the duty cycle to the maximum value of 255
    if (duty_cycle > 255) {
        duty_cycle = 255;
    }
    OCR0B = duty_cycle;  // Set the duty cycle for OC0B (PD5)
}

void pwm_set_duty_cycle_2(uint8_t duty_cycle) {
    // Clamp the duty cycle to the maximum value of 255
    if (duty_cycle > 255) {
        duty_cycle = 255;
    }
    OCR0A = duty_cycle;  // Set the duty cycle for OC0A (PD6)
}
