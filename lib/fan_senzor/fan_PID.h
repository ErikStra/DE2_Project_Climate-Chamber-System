/** 
 * ---------------------------------------------------+ 
 * @desc        User Interface Library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024  Kostenko, Zobac
 * @email       
 * @author      
 * @datum       25.11.2024
 * @file        
 * @version     1.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#include <stdio.h>          // Standard library

/** @definition */
#ifndef __FAN_PID_H__
#define __FAN_PID_H__

  /**
   * @desc   PID control function
   *
   * @param  target_temp Target temperature
   * @param  current_temp Current temperature
   * @param  prev_error Pointer to the previous error value
   * @param  integral Pointer to the integral value
   *
   * @return uint8_t Calculated PWM duty cycle
   */
  uint8_t pid_control(float target_temp, float current_temp, float *prev_error, float *integral);

  /**
   * @desc   Controls the fans using PID
   *
   * @param  void
   *
   * @return void
   */
  void fan_control_pid(void);

  /**
   * @desc   Initializes the fan PID system
   *
   * @param  void
   *
   * @return int Initialization status (0 for success)
   */
  int fan_PID_init(void);

  /**
   * @desc   Main loop for the fan PID system
   *
   * @param  void
   *
   * @return int Loop execution status (0 for success)
   */
  int fan_PID_loop(void);

#endif
