/** 
 * ---------------------------------------------------+ 
 * @desc        User Interface Library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Jindřich Zobač.
 * @email       246975@vutbr.cz
 * @author      Jindřich Zobač
 * @datum       25.11.2024
 * @file        UserInterface.h
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */

/** @definition */
#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

  /**
   * @desc   Voltmeter
   *
   * @param  void
   *
   * @return void
   */
  int UserInterface_init (void);
  int UserInterface_loop (void);
  void LCD_DrawScreen1(void);
  void LCD_DrawScreen2(void);
  void LCD_DrawScreen3(void);
  void LCD_DrawScreen4(void);
  void LCD_DrawScreen5(void);
  void HandleInput(char input);

#endif
