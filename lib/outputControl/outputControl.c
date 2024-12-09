// -- Defines --------------------------------------------------------
#define D4 PD4  // 


// -- Includes -------------------------------------------------------
#include <avr/io.h>      // AVR device-specific IO definitions
#include <util/delay.h>  // Functions for busy-wait delay loops
#include <gpio.h>        // GPIO library for AVR-GCC
#include "variables.h"  // globální proměnné
#include "pwm.h"        //knihovna řízení pwm


// -- Function definitions -------------------------------------------
/*
 * Function: Main function where the program execution begins
 * Purpose:  Toggle LED(s) using delay.
 * Returns:  none
 */
void outputControl_init(void)
{
    // Set output pin(s) in Data Direction Register
    GPIO_mode_output(&DDRD, D4);

    // Set pin(s) LOW in Data Register
    GPIO_write_low(&PORTD, D4);
}

    // Infinite loop
void outputControl_loop(void)
    {
        GPIO_write(!LED, &PORTD, D4);
        pwm_set_duty_cycle_1(fan_big);
        pwm_set_duty_cycle_2(fan_led);
    }