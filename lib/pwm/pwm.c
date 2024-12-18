/** 
 * ---------------------------------------------------+ 
 * @desc        PWM library c
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Artem Kostenko.
 * @email       238545@vutbr.cz
 * @author      Artem Kostenko
 * @datum       25.11.2024
 * @file        pwm.c
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#include "pwm.h"

void pwm_init(void) {
    // Set PD5 and PD6 as outputs
    DDRD |= (1 << DDD5) | (1 << DDD6);

    // Configure Timer0
    TCCR0A = (1 << WGM00) | (1 << WGM01); // Set Fast PWM Mode
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Enable non-inverting mode for OC0A (PD6) and OC0B (PD5)
    TCCR0B = (1 << CS01); // Set prescaler to 8 (PWM frequency = F_CPU / (8 * 256))

    // Initialize PWM with 50% duty cycle
    OCR0A = 128; // PD6 (OC0A)
    OCR0B = 128; // PD5 (OC0B)
}

void pwm_set_duty_cycle_1(uint8_t duty_cycle) {
    OCR0B = duty_cycle;  // Set the duty cycle for OC0B (PD5)
}

void pwm_set_duty_cycle_2(uint8_t duty_cycle) {
    OCR0A = duty_cycle;  // Set the duty cycle for OC0A (PD6)
}
