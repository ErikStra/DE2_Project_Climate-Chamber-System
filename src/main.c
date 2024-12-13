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
#include "rtc_control.h"    // Real time clock module library
#include "UserInterface.h"  // UI and control
#include "fan_PID.h"        // Fan control
#include "outputControl.h"  // GPIO control
#include "bmp280.h"         //bmp280 library



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

char command_buffer[50] = {0}; // Buffer for incoming data

int main(void){
  // inits
  // Inicialization of timer
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
    
    bmp280_init();
    outputControl_init();
    


while (1) { // infinte loop
    if(flag_UI_getcommand){ //get command from keyboard
      uart_getcommand(NULL);
      
      //UserInterface_input_loop(); //deprecated
      flag_UI_getcommand=0;
    }

    if(flag_UI_display_loop){ //refresh screen
      UserInterface_display_loop(previous_n_ovfs);
      
      flag_UI_display_loop=0;
    }
  
    if(flag_fan_PID){ //control fans according to temperature
      fan_PID_loop();

      /* char buffer[100]; //for debugging
      snprintf(buffer, sizeof(buffer), "KP1 = %d KI1 = %d KD1 = %d", (int) KP1*10, (int) KI1*10, (int) KD1*10);
      uart_puts(buffer);
      uart_puts("   \n\r");
 */
      flag_fan_PID=0;
    }

    if(flag_RTC){   //update time
      rtc_read_time();
      rtc_control_loop();
      flag_RTC = 0;
    }

    if(flag_outputControl){ //set GPIOs
      outputControl_loop();
      flag_outputControl = 0;
    }

    if(flag_dht_update_temp1) { //read temperature from DHT sensor
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

    if(flag_bmp280_update_temp_press) //read temperature from the other sensor
      {
        bmp280_measure();
        TEMP2 = bmp280_gettemperature();
        PRESSURE = bmp280_getpressure();
        /* char buffer[3];
        itoa((TEMP2), buffer, 10);
        uart_puts(buffer); */
        flag_bmp280_update_temp_press = 0;
      }
  }
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}

ISR(TIMER2_OVF_vect){ //inerrupts for task managing
    if (n_ovfs % 10 == 0){ //cca every 130 ms
      flag_UI_getcommand=1;
    }

    if (n_ovfs % 15 == 0){ //cca every 200 ms
      flag_UI_display_loop=1;
      previous_n_ovfs = n_ovfs;
      flag_fan_PID=1;
    }

    if (n_ovfs % 30 == 0){ //cca every 500 ms
      flag_RTC=1;
    }

    if (n_ovfs % 63 == 0){ //cca every 1s
      flag_outputControl = 1;
      flag_dht_update_temp1 = 1;
      flag_bmp280_update_temp_press = 1;
      
    }

    if (n_ovfs % 250 == 0){ //cca every 4s
    }
    
    n_ovfs++;
    if (n_ovfs >= 500){//reset of counter
        n_ovfs = 0; 
    }
}