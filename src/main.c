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
  // voltmeter
  UserInterface();

  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
