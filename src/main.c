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
#include "UserInterface.h"

/**
 * @desc   Main function
 *
 * @param  Void
 *
 * @return int
 */
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

  while (1)
  {
  UserInterface_loop();
  
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}