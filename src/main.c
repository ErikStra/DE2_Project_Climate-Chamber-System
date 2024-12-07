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
#include "DHT.h"

//---- NAŠE KNIHOVNY------------------------------------------------
#include "rtc_control.h"    // čízení reálného času
#include "UserInterface.h"  // UI a ovládání
#include "fan_PID.h"        // Ovládání ventilátorů
#include "outputControl.h"  // Ovládání GPIO pinů
#include "dht.h"


/**
 * @desc   Main function
 *
 * @param  Void
 *
 * @return int
 */

volatile uint8_t flag_UI_input_loop = 0;
volatile uint8_t flag_UI_display_loop = 0;
volatile uint8_t flag_fan_PID = 0;
volatile uint8_t flag_RTC = 0;
volatile uint8_t flag_outputControl = 0;
volatile uint8_t flag_dht_update_temp1 = 0;

uint16_t n_ovfs = 0;



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
    fan_PID_init();
    outputControl_init();
    

  while (1)
  {
    if(flag_UI_input_loop)
    {
      UserInterface_input_loop();
      flag_UI_input_loop=0;
    }

    if(flag_UI_display_loop)
    {
      UserInterface_display_loop(n_ovfs);
      flag_UI_display_loop=0;
    }
  
    if(flag_fan_PID)
    {
      fan_PID_loop();
      flag_fan_PID=0;
    }

    if(flag_RTC)
    {
      rtc_read_time();
      rtc_control_loop();
      flag_RTC = 0;
    }

    if(flag_outputControl)
    {
      outputControl_loop();
      flag_RTC = 0;
    }

    if(flag_dht_update_temp1)
    {
      dht_update_temp1();
      flag_RTC = 0;
    }


  
  
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
ISR(TIMER0_OVF_vect)
{
    if (n_ovfs % 6 == 0) //cca každých 100 ms
    {
      flag_UI_input_loop=1;
    }

    if (n_ovfs % 13 == 0) //cca každých 200 ms
    {
      flag_UI_display_loop=1;
    }

    if (n_ovfs % 31 == 0) //cca každých 500 ms
    {
      flag_RTC=1;
    }

    if (n_ovfs % 63 == 0) //cca každou 1s
    {
      //flag_fan_PID=1;
      flag_outputControl = 1;
      flag_dht_update_temp1 = 1;

    }

    if (n_ovfs % 4*63 == 0) //cca každou 1s
    {
      flag_dht_update_temp1 = 1;

    }
    
    n_ovfs++;
    if (n_ovfs >= 500)
    {
        n_ovfs = 0;
    }
}