

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>  // Pro práci s EEPROM
#include <stdlib.h>
#include "../../include/timer.h"
#include <twi.h>
#include <uart.h>
#include <rtc_control.h>

#define RTC_ADR 0x68
#define RTC_SEC_MEM 0
#define RTC_MIN_MEM 1
#define RTC_HOUR_MEM 2

/* EEPROM adresy */
#define EEPROM_SUNRISE_HOURS_ADDR 0x00
#define EEPROM_SUNRISE_MINUTES_ADDR 0x01
#define EEPROM_SUNSET_HOURS_ADDR 0x02
#define EEPROM_SUNSET_MINUTES_ADDR 0x03
#define EEPROM_LIGHT_STATE_ADDR 0x04

/* Struktura RTC */
struct RTC_values_structure {
    uint8_t hours;
    uint8_t mins;
    uint8_t secs;
} RTC;

volatile uint8_t new_sensor_data = 0;
uint8_t svetlo = 0;  // Stav světla (0 = vypnuto, 1 = zapnuto)

/* Proměnné pro časy východu a západu slunce */
uint8_t sunrise_hours, sunrise_minutes;
uint8_t sunset_hours, sunset_minutes;

/* Funkční prototypy */
uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);
void rtc_read_time();
void compare_time_with_sun(uint8_t current_hours, uint8_t current_minutes);
void load_settings_from_eeprom();
void save_settings_to_eeprom();

int rtc_control(void) {
    char string[3];  // Pro převody čísel

    twi_init();
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();

    // Načtení časů východu a západu slunce z EEPROM
    load_settings_from_eeprom();

    rtc_initialize();

    // Test RTC
    if (twi_test_address(RTC_ADR) != 0) {
        uart_puts("[ERROR] RTC not detected\r\n");
        while (1);
    }


    // Nastavení časovače
    TIM0_ovf_16m();
    TIM0_ovf_enable();

    while (1) {
        if (new_sensor_data) {
            // Porovnání aktuálního času s časy východu a západu
            compare_time_with_sun(bcdToDec(RTC.hours), bcdToDec(RTC.mins));         
            
            // Výpis na UART
            itoa(bcdToDec(RTC.hours), string, 10);
            uart_puts(string);
            uart_puts(":");
            itoa(bcdToDec(RTC.mins), string, 10);
            uart_puts(string);
            uart_puts(":");
            itoa(bcdToDec(RTC.secs), string, 10);
            uart_puts(string);
            uart_puts(" ");
            uart_puts(svetlo ? "ON\r\n" : "OFF\r\n");

            new_sensor_data = 0;
        }
    }
}

void rtc_set_time(uint8_t hours, uint8_t mins, uint8_t secs) {
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM);
    twi_write(decToBcd(secs));
    twi_write(decToBcd(mins));
    twi_write(decToBcd(hours));
    twi_stop();
}

// Funkce pro inicializaci RTC
void rtc_initialize(void) {
    // Čtení sekund z RTC pro kontrolu platnosti času
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM);  // Nastavení na registr pro sekundy
    twi_stop();

    twi_start();
    twi_write((RTC_ADR << 1) | TWI_READ);
    uint8_t secs = twi_read(TWI_NACK);  // Čtení sekund
    twi_stop();

    // Kontrola, zda je čas platný
    if (secs == 0 || secs == 0xFF || bcdToDec(secs) > 59) {
        // Nastavení času z kompilace
        uint8_t hours = atoi(__TIME__);
        uint8_t mins = atoi(__TIME__ + 3);
        uint8_t secs = atoi(__TIME__ + 6);

        rtc_set_time(hours, mins, secs);
        uart_puts("[INFO] RTC initialized with compile time\r\n");
    } else {
        uart_puts("[INFO] RTC time is valid\r\n");
    }
}

void compare_time_with_sun(uint8_t current_hours, uint8_t current_minutes) {
    // Svítí, pokud je aktuální čas mezi východem a západem slunce
    if ((current_hours > sunrise_hours || 
         (current_hours == sunrise_hours && current_minutes >= sunrise_minutes)) &&
        (current_hours < sunset_hours || 
         (current_hours == sunset_hours && current_minutes < sunset_minutes))) {
        svetlo = 1;
    } else {
        svetlo = 0;
    }

    // Uložení stavu světla do EEPROM
    eeprom_write_byte((uint8_t*)EEPROM_LIGHT_STATE_ADDR, svetlo);
}

void rtc_read_time() {
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(RTC_SEC_MEM);
    twi_stop();

    twi_start();
    twi_write((RTC_ADR << 1) | TWI_READ);
    RTC.secs = twi_read(TWI_ACK);
    RTC.mins = twi_read(TWI_ACK);
    RTC.hours = twi_read(TWI_NACK);
    twi_stop();
}

void load_settings_from_eeprom() {
    sunrise_hours = eeprom_read_byte((uint8_t*)EEPROM_SUNRISE_HOURS_ADDR);
    sunrise_minutes = eeprom_read_byte((uint8_t*)EEPROM_SUNRISE_MINUTES_ADDR);
    sunset_hours = eeprom_read_byte((uint8_t*)EEPROM_SUNSET_HOURS_ADDR);
    sunset_minutes = eeprom_read_byte((uint8_t*)EEPROM_SUNSET_MINUTES_ADDR);

    // Pokud EEPROM obsahuje neplatné hodnoty, nastavíme výchozí
    if (sunrise_hours == 0xFF || sunrise_minutes == 0xFF ||
        sunset_hours == 0xFF || sunset_minutes == 0xFF) {
        sunrise_hours = 6;  // Výchozí východ slunce (6:00)
        sunrise_minutes = 0;
        sunset_hours = 18;  // Výchozí západ slunce (18:00)
        sunset_minutes = 0;
        save_settings_to_eeprom();
    }
}

void save_settings_to_eeprom() {
    eeprom_write_byte((uint8_t*)EEPROM_SUNRISE_HOURS_ADDR, sunrise_hours);
    eeprom_write_byte((uint8_t*)EEPROM_SUNRISE_MINUTES_ADDR, sunrise_minutes);
    eeprom_write_byte((uint8_t*)EEPROM_SUNSET_HOURS_ADDR, sunset_hours);
    eeprom_write_byte((uint8_t*)EEPROM_SUNSET_MINUTES_ADDR, sunset_minutes);
}

uint8_t bcdToDec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

uint8_t decToBcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}

ISR(TIMER0_OVF_vect) {
    rtc_read_time();
    new_sensor_data = 1;
}
