/** 
 * ---------------------------------------------------+ 
 * @desc        PID library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Artem Kostenko.
 * @email       238545@vutbr.cz
 * @author      Artem Kostenko
 * @datum       25.11.2024
 * @file        fan_PID.c
 * @version     3.0
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
  uint8_t pid_control(int16_t actual_temp, int16_t target_temp, int16_t *prev_error, int32_t *integral, uint8_t KP, uint8_t KI, uint8_t KD);
  /**

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
