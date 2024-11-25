#include "pwm.h"

void pwm_init(void) {
    // Set FAN_PIN as output
    DDRB |= (1 << FAN_PIN);

    // Configure timer for PWM on FAN_PIN (PB1/OC1A)
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);   // Fast PWM, non-inverting
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // No prescaler
    ICR1 = 255;  // Set TOP value for 8-bit resolution
}

void pwm_set_duty_cycle(uint8_t duty_cycle) {
    if (duty_cycle > 255) {
        duty_cycle = 255;  // Ensure the duty cycle stays within range
    }
    OCR1A = duty_cycle;  // Set the duty cycle
}
