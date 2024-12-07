// -- Defines --------------------------------------------------------
#define LED_LIGHT PD4  // On-board LED


// -- Includes -------------------------------------------------------
#include <avr/io.h>      // AVR device-specific IO definitions
#include <util/delay.h>  // Functions for busy-wait delay loops
#include <gpio.h>        // GPIO library for AVR-GCC
#include "variables.h"


// -- Function definitions -------------------------------------------
/*
 * Function: Main function where the program execution begins
 * Purpose:  Toggle LED(s) using delay.
 * Returns:  none
 */
void outputControl_init(void)
{
    // Set output pin(s) in Data Direction Register
    GPIO_mode_output(&DDRD, LED_LIGHT);

    // Set pin(s) LOW in Data Register
    GPIO_write_low(&PORTD, LED_LIGHT);
}

    // Infinite loop
void outputControl_loop(void)
    {
        GPIO_write(!LED, &PORTD, LED_LIGHT);
    }