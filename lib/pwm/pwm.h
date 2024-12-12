/** 
 * ---------------------------------------------------+ 
 * @desc        PWM library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Artem Kostenko.
 * @email       238545@vutbr.cz
 * @author      Artem Kostenko
 * @datum       25.11.2024
 * @file        pwm.h
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#ifndef PWM_H
#define PWM_H

#include <avr/io.h> //AVR device-specific IO definitions

// Pin definitions
#define FAN1_PIN PD5  // Define the pin for Fan 1 (OC0B, Arduino Nano pin D5)
#define FAN2_PIN PD6  // Define the pin for Fan 2 (OC0A, Arduino Nano pin D6)

// Function declarations
void pwm_init(void); // Initialize PWM

void pwm_set_duty_cycle_1(uint8_t duty_cycle); // Set duty cycle for Fan 1 (PD5)
void pwm_set_duty_cycle_2(uint8_t duty_cycle); // Set duty cycle for Fan 2 (PD6)

#endif  // PWM_H
