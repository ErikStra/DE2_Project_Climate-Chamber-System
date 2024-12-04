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

#include "rtc_control.h"    // čízení reálného času
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
        TIM0_ovf_16m();
        TIM0_ovf_enable();
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    //uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();

    
    UserInterface_init();
    rtc_control_init();
    //FanSenzor_init();
    

  while (1)
  {
  UserInterface_loop();
  rtc_control_loop();

  if(flag_fansensor==1)
  {
    //FanSenzor_loop();
    flag_fansensor=0;
  }
  
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
ISR(TIMER0_OVF_vect)
{
    static uint8_t n_ovfs = 0;
    
    

    if (n_ovfs % 20 == 0)
    {
      UserInterface_interrupt(n_ovfs);
    }

    if (n_ovfs == 51)
    {
      rtc_control_interrupt();
      flag_fansensor=1;
      
    }
    
    n_ovfs++;
    if (n_ovfs >= 100)
    {
        n_ovfs = 0;
    }
    // Else do nothing and exit the ISR
}