/** 
 * ---------------------------------------------------+ 
 * @desc        Main file
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Jindřich Zobač.
 * @email       246975@vutbr.cz
 * @author      Jindřich Zobač
 * @datum       25.11.2024
 * @file        main.c
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#include <uart.h>           // Peter Fleury's UART library
#include "timer.h"        // Timer library for AVR-GCC
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions
#include <stdio.h>          // Standardní knihovna
#include "variables.h"      // globální proměnné


#include "UserInterface.h"
#include "fan_senzor.h"

/**
 * @desc   Main function
 *
 * @param  Void
 *
 * @return int
 */

uint8_t flag_fansensor = 0;
int main(void)
{
  // inits
  // Inicializace timerů
        TIM1_ovf_262ms();
        TIM1_ovf_enable();
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    //uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();
  
    UserInterface_init();
    FanSenzor_init();
    

  while (1)
  {
  UserInterface_loop();

  if(flag_fansensor==1)
  {
    FanSenzor_loop();
    flag_fansensor=0;
  }
  
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
ISR(TIMER1_OVF_vect)
{
    static uint8_t n_ovfs = 0;
    char buffer[10];
    UserInterface_interrupt(n_ovfs);
    uart_puts("n_ovf: ");
    itoa(n_ovfs, buffer, 10);
    uart_puts(buffer);

    if (n_ovfs % 50 == 0)
    {
        
    }

    if (n_ovfs % 100 == 0)
    {
      flag_fansensor=1;
    }
    
    

    n_ovfs++;
    if (n_ovfs >= 200)
    {
        n_ovfs = 0;
    }
    // Else do nothing and exit the ISR
}