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
#include <string.h>
#include "variables.h"      // globální proměnné
#include "DHT.h"

//---- NAŠE KNIHOVNY------------------------------------------------
#include "rtc_control.h"    // čízení reálného času
#include "UserInterface.h"  // UI a ovládání
#include "fan_PID.h"        // Ovládání ventilátorů
#include "outputControl.h"  // Ovládání GPIO pinů
#include "bmp280.h"         //bmp280 knihovna



/**
 * @desc   Main function
 *
 * @param  Void
 *
 * @return int
 */

volatile uint8_t flag_UI_getcommand = 0;
volatile uint8_t flag_UI_display_loop = 0;
volatile uint8_t flag_fan_PID = 0;
volatile uint8_t flag_RTC = 0;
volatile uint8_t flag_outputControl = 0;
volatile uint8_t flag_dht_update_temp1 = 0;
volatile uint8_t flag_bmp280_update_temp_press = 0;

uint16_t n_ovfs = 0;
uint16_t previous_n_ovfs = 0;
uint8_t DHT_error_count =0;
uint8_t IDK =2; // 10 100 1000...

uint16_t temperature_int = 0;
uint16_t humidity_int = 0;

char command_buffer[50] = {0}; // Buffer pro příjem dat

int main(void){
  // inits
  // Inicializace timerů
        TIM2_ovf_16ms();
        TIM2_ovf_enable();
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    //uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();
    uart_puts("[INFO] System loading...\r\n");

    //inicializace komponentů
    UserInterface_init();
    rtc_control_init();
    fan_PID_init();
    outputControl_init();
    //bmp280_init();
    
    


while (1) { // Nekonečná smyčka
    if(flag_UI_getcommand){
      uart_getcommand(NULL);
      
      //UserInterface_input_loop();
      flag_UI_getcommand=0;
    }

    if(flag_UI_display_loop){
      UserInterface_display_loop(previous_n_ovfs);
      
      flag_UI_display_loop=0;
    }
  
    if(flag_fan_PID){
      fan_PID_loop();

      /* char buffer[100];
      snprintf(buffer, sizeof(buffer), "KP1 = %d KI1 = %d KD1 = %d", (int) KP1*10, (int) KI1*10, (int) KD1*10);
      uart_puts(buffer);
      uart_puts("   \n\r");
 */
      flag_fan_PID=0;
    }

    if(flag_RTC){
      rtc_read_time();
      rtc_control_loop();
      flag_RTC = 0;
    }

    if(flag_outputControl){
      outputControl_loop();
      flag_outputControl = 0;
    }

    if(flag_dht_update_temp1) {
      //call DHT sensor function defined in DHT.c
      if (dht_GetTempUtil(&TEMP1, &HUM1) == -1) {	
        DHT_error_count ++;
        if (DHT_error_count %IDK == 0){
          uart_puts("\r\n[ERROR] Unable to read DHT data ");
          char buffer[3];
          itoa(DHT_error_count, buffer, 10);
          uart_puts(buffer);
          uart_puts(" times in a row\r\n");
          IDK = IDK*5;
        }        
      } else {
        DHT_error_count =0;
        IDK = 5;
      }
      flag_dht_update_temp1 = 0;
    }

    if(flag_bmp280_update_temp_press)
      {
        bmp280_measure();
        TEMP2 = bmp280_gettemperature();
        PRESSURE = bmp280_getpressure();
        char buffer[3];
        itoa((TEMP2), buffer, 10);
        uart_puts(buffer);
        flag_bmp280_update_temp_press = 0;
      }
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}

ISR(TIMER2_OVF_vect){
    if (n_ovfs % 10 == 0){ //cca každých 130 ms
      flag_UI_getcommand=1;
    }

    if (n_ovfs % 15 == 0){ //cca každých 200 ms
      flag_UI_display_loop=1;
      previous_n_ovfs = n_ovfs;
      flag_fan_PID=1;
    }

    if (n_ovfs % 30 == 0){ //cca každých 500 ms
      flag_RTC=1;
    }

    if (n_ovfs % 63 == 0){ //cca každou 1s
      flag_outputControl = 1;
      flag_dht_update_temp1 = 1;
      //flag_bmp280_update_temp_press = 1;
      
    }

    if (n_ovfs % 250 == 0){ //cca každé 4s
    }
    
    n_ovfs++;
    if (n_ovfs >= 500){//reset počítadla
        n_ovfs = 0; 
    }
}