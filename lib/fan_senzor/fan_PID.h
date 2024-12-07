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
#ifndef __FAN_PID_H__
#define __FAN_PID_H__

  /**
   * @desc   Voltmeter
   *
   * @param  void
   *
   * @return void
   */

  uint8_t pid_control(float target_temp, float current_temp, float *prev_error, float *integral);
 void fan_control_pid(void);
 int fan_PID_init(void);
 int fan_PID_loop(void);


#endif
