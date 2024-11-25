#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

// Pin definitions
#define FAN_PIN PB1  // Define the pin for the fan (D9 or OC1A)

// Function declarations
void pwm_init(void);
void pwm_set_duty_cycle(uint8_t duty_cycle);

#endif  // PWM_H
