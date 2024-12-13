/** 
 * ---------------------------------------------------+ 
 * @desc        RTC library c
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Vít Vašenka.
 * @email       246961@vutbr.cz
 * @author      Vít Vašenka
 * @datum       12.12.2024
 * @file        rtc_control.c
 * @version     3.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */


#include <avr/io.h>                 // AVR device-specific IO definitions
#include <avr/interrupt.h>          // Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>             // Pro práci s EEPROM
#include <stdlib.h>                 // C library. Needed for number conversions
#include "../../include/timer.h"    // Timer library for AVR-GCC
#include <twi.h>                    // I2C knihovna Tomáše Fryzy
#include <uart.h>                   // Peter Fleury's UART library
#include <rtc_control.h>            // Header file
#include <variables.h>              // Global variables 

//RTC modul structure 
#define RTC_ADR 0x68    //Adresa modulu na I2C sběrnici

//Registers for seconds, minutes, hours
#define RTC_SEC_MEM 0
#define RTC_MIN_MEM 1
#define RTC_HOUR_MEM 2

// EEPROM adress
#define EEPROM_SUNRISE_HOURS_ADDR 0x00
#define EEPROM_SUNRISE_MINUTES_ADDR 0x01
#define EEPROM_SUNSET_HOURS_ADDR 0x02
#define EEPROM_SUNSET_MINUTES_ADDR 0x03
#define EEPROM_LIGHT_STATE_ADDR 0x04


char string[3];                         //Převod čísel na řetězce

volatile uint8_t new_sensor_data = 0;   //Indikace nových dat ze senzorů


//Variables for sunrise and sunset 
uint8_t sunrise_hours, sunrise_minutes;
uint8_t sunset_hours, sunset_minutes;

uint8_t bcdToDec(uint8_t val);          //Převod z BCD na desítkové číslo
uint8_t decToBcd(uint8_t val);          //Přeovd z desítkového čísla na BCD
void rtc_read_time();                   //Čtení času z RTC
uint8_t compare_time_with_sun(uint8_t currentHour, uint8_t sunrise, uint8_t sunset); //Porovnání času východu a západu slunce s aktuálním časem
void load_settings_from_eeprom();       //Načtení stavu LED z EEPROM
void save_settings_to_eeprom();         //Uložení stavu LED do EEPROM

//RTC initialization
void rtc_control_init(void) {
    
    load_settings_from_eeprom();

    rtc_initialize();

    //Test RTC
    if (twi_test_address(RTC_ADR) != 0) {
        uart_puts("[ERROR] RTC not detected\r\n");  //Výpis chybové hlášky na uart
    }
}

//A loop to compare the current time with the sunrise and sunset time
void rtc_control_loop(void) {
        compare_time_with_sun(hours, sunrise, sunset);         
            
            /* // Výpis na UART pro kontrolu času 
            itoa((hours), string, 10);
            uart_puts(string);
            uart_puts(":");
            itoa((minutes), string, 10);
            uart_puts(string);
            uart_puts(":");
            itoa((secs), string, 10);
            uart_puts(string);
            uart_puts(" ");
            uart_puts(LED ? "ON\r\n" : "OFF\r\n"); */
        
    }

//Setting the RTC time using I2C
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t secs) {
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM);                 
    twi_write(decToBcd(secs));
    twi_write(decToBcd(minutes));
    twi_write(decToBcd(hours));
    twi_stop();
}

//RTC initialization function
 void rtc_initialize(void) {
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM); 
    twi_stop();

    twi_start();
    twi_write((RTC_ADR << 1) | TWI_READ);
    uint8_t secs = twi_read(TWI_NACK);  
    twi_stop(); 

  if (1)
    {   
        // Sets time from compilation
        uint8_t hours = atoi(__TIME__);
        uint8_t minutes = atoi(__TIME__ + 3);
        uint8_t secs = atoi(__TIME__ + 6);

        rtc_set_time(hours, minutes, secs);
        uart_puts("[INFO] RTC initialized with compile time\r\n");
    } else {
        uart_puts("[INFO] RTC time is valid \r\n");
    }
} //Kontroluje, zda je RTC čas platný, a pokud ne, inicializuje ho na čas kompilace




//Determines whether the LED should be on, based on the time
uint8_t compare_time_with_sun(uint8_t currentHour, uint8_t sunrise, uint8_t sunset) {
    

    if (sunset < sunrise) {
        LED = (currentHour >= sunrise || currentHour < sunset);
    } else {
        LED = (currentHour >= sunrise && currentHour < sunset);
    }

   /*  //Výpis na uart pro kontrolu času východu a západu slunce
    itoa(sunrise, string, 10);
    uart_puts(string);
    uart_puts("         ");
    itoa(sunset, string, 10);
    uart_puts(string);
    uart_puts("         ");
    itoa(currentHour, string, 10);
    uart_puts(string);
    uart_puts("         ");
    itoa(LED, string, 10);
    uart_puts(string);
    uart_puts("\n"); */


    eeprom_write_byte((uint8_t*)EEPROM_LIGHT_STATE_ADDR, LED);
    return LED;
}

//Starts communication with the RTC and sets the register
void rtc_read_time() {
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM);
    twi_stop();

    twi_start();
    twi_write((RTC_ADR << 1) | TWI_READ);
    secs = bcdToDec(twi_read(TWI_ACK));
    minutes = bcdToDec(twi_read(TWI_ACK));
    hours = bcdToDec(twi_read(TWI_NACK));
    twi_stop();
} // Funkce je určena pro průběžné čtení aktuálního času během běhu programu


//Loads settings from EEPROM
void load_settings_from_eeprom() {
    sunrise_hours = eeprom_read_byte((uint8_t*)EEPROM_SUNRISE_HOURS_ADDR);
    sunrise_minutes = eeprom_read_byte((uint8_t*)EEPROM_SUNRISE_MINUTES_ADDR);
    sunset_hours = eeprom_read_byte((uint8_t*)EEPROM_SUNSET_HOURS_ADDR);
    sunset_minutes = eeprom_read_byte((uint8_t*)EEPROM_SUNSET_MINUTES_ADDR);

    // Pokud EEPROM obsahuje neplatné hodnoty, potom nastaví výchozí hodnoty
    if (sunrise_hours == 0xFF || sunrise_minutes == 0xFF ||
        sunset_hours == 0xFF || sunset_minutes == 0xFF) {
        sunrise_hours = sunrise;  
        sunrise_minutes = 0;
        sunset_hours = sunset;                 
        sunset_minutes = 0;
        save_settings_to_eeprom();
    }
}

//Stores sunrise and sunset time in EEPROM.
void save_settings_to_eeprom() {
    eeprom_write_byte((uint8_t*)EEPROM_SUNRISE_HOURS_ADDR, sunrise_hours);
    eeprom_write_byte((uint8_t*)EEPROM_SUNRISE_MINUTES_ADDR, sunrise_minutes);
    eeprom_write_byte((uint8_t*)EEPROM_SUNSET_HOURS_ADDR, sunset_hours);
    eeprom_write_byte((uint8_t*)EEPROM_SUNSET_MINUTES_ADDR, sunset_minutes);
}

//Converts numbers between BCD and decimal formats
uint8_t bcdToDec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

uint8_t decToBcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}
