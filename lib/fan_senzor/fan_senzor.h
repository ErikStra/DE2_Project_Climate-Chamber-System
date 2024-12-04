/** 
 * ---------------------------------------------------+ 
 * @desc        User Interface Library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Straka, Kostenko.
 * @email       
 * @author      
 * @datum       25.11.2024
 * @file        
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#include <stdio.h>          // Standardní knihovna

/** @definition */
#ifndef __FAN_SENSOR_H__
#define __FAN_SENSOR_H__

  /**
   * @desc   Voltmeter
   *
   * @param  void
   *
   * @return void
   */

  void fan_control(uint8_t temperature);
  void SaveTemp (void);
  int FanSenzor_init(void);
  int FanSenzor_loop();

#endif
