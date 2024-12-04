#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <uart.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include "hd44780pcf8574.h" // LCD i2C library
#include <stdio.h>          // Standardní knihovna
#include "twi.h"            //I2C knihovna Tomáše Fryzy
#include "UserInterface.h"  // hlavičkový soubor této knihovny
#include "variables.h"      // globální proměnné


// -- Konstanty a proměnné ------------------------------------------
#define SCREEN_1 0
#define SCREEN_2 4
#define SCREEN_3 3
#define SCREEN_4 2
#define SCREEN_5 1
#define TOTAL_SCREENS 5
#define SETTINGS_COUNT 12



// Pole pro zobrazení časových plánů
const char *control_options[8] = {"Temp.  ", "AirHum.", "None   "};
const char *autowater_options[5] = {"Cont.  ", "Once ", "Off  "};
const char *ONOFF_options[3] = {"off", "ON"};
const char *HILO_options[2] = {"HI", "lo"};

// Další proměnné
char addr = PCF8574_ADDRESS; // Adresa LCD
uint8_t current_screen = SCREEN_1;
uint8_t last_screen = 255;  // Poslední zobrazená obrazovka (pro detekci změny)
uint8_t selected_setting = 0;

//Flagy pro interrupty
volatile uint8_t flag_update_lcd = 0;
volatile uint8_t flag_tick = 0;


// -- Funkce pro vykreslování ---------------------------------------
void LCD_DrawScreen2() {
    char buffer[13];
    char buffer2[11];

    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "2/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "5");

    switch (selected_setting) {
        case 0:
            snprintf(buffer, 13, "SETTINGS    ");
            snprintf(buffer2, 5, "udrl     ");

            break;
        case 1: // Max Temp
            snprintf(buffer, 13, "Max Temp.");
            snprintf(buffer2, 6, "%d%cC ", max_temp, 223);
            break;
        case 2: // Min Temp
            snprintf(buffer, 13, "Min Temp.");
            snprintf(buffer2, 6, "%d%cC ", min_temp, 223);
            break;
        case 3: // Max AirHum
            snprintf(buffer, 13, "Max Air Hum.");
            snprintf(buffer2, 6, "%d %% ", max_airhum);
            break;
        case 4: // Min AirHum
            snprintf(buffer, 13, "Min Air Hum.");
            snprintf(buffer2, 6, "%d %% ", min_airhum);
            break;
        case 5: // Control 
            snprintf(buffer, 13, "Control     ");
            snprintf(buffer2, 11, "%s", control_options[control]);
            break;    
        case 6: // Light ON time (Hour)
            snprintf(buffer, 13, "Sunrise ");
            snprintf(buffer2, 7, "%d:00  ", sunrise);
            break;
        case 7: //Light OFF time (HOUR)
            snprintf(buffer, 13, "Sunset ");
            snprintf(buffer2, 7, "%d:00  ", sunset);
            break;
        case 8: // Max SoilHum
            snprintf(buffer, 13, "Max Soil Hum");
            snprintf(buffer2, 6, "%d %% ", max_soilhum);
            break;
        case 9: // Min SoilHum
            snprintf(buffer, 13, "Min Soil Hum");
            snprintf(buffer2, 6, "%d %% ", min_soilhum);
            break;
        case 10: //watering time (HOUR)
            snprintf(buffer, 13, "Watering Time");
            snprintf(buffer2, 7, "%d:00 ", water_time);
            break;
        case 11: // Autowatering time options
            snprintf(buffer, 13, "Watering Mode");
            snprintf(buffer2, 11, "%s", autowater_options[autowater]);
            break;
    }
    //řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    HD44780_PCF8574_DrawString(addr, buffer2);
}

void LCD_DrawScreen1() {
    char buffer[16];

    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "1/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "1");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 16, "Ha:%2d%% Hs:%2d%%", HUM1, HUM2);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    snprintf(buffer, 16, "Tair:%2d%cC", TEMP1, 223);
    HD44780_PCF8574_DrawString(addr, buffer);
    HD44780_PCF8574_PositionXY(addr, 11, 1);
    if (flag_tick == 1)
    {
        snprintf(buffer, 16, "%02d:%02d", hours, minutes);
    }
    else
    {
        snprintf(buffer, 16, "%02d %02d", hours, minutes);
    }
    HD44780_PCF8574_DrawString(addr, buffer);
}

void LCD_DrawScreen3() {
    char buffer[17];
    
    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "3/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "4");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "EMPTY SCREEN");
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    snprintf(buffer, 16, "%d = %c", chartest, chartest);
    HD44780_PCF8574_DrawString(addr, buffer);
}

void LCD_DrawScreen4() {
    char buffer[17];
    
    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "4/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "3");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "FAN1:%d%%", fan_big);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    snprintf(buffer, 17, "FN2:%d%% FN3:%d%%", fan_small, fan_led);
    HD44780_PCF8574_DrawString(addr, buffer);
}

void LCD_DrawScreen5() {
    char buffer[17];
    
    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "5/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "2");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "LED:%s %d%cC",ONOFF_options[LED], TEMP2, 223);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    snprintf(buffer, 17, "PUMP:%s lvl:%s", ONOFF_options[pump], HILO_options[wlevel]);
    HD44780_PCF8574_DrawString(addr, buffer);
}


// -- Funkce pro ovládání -------------------------------------------
void HandleInput(char input) {
    if (input == 'b') { // Přechod na další obrazovku
        current_screen = (current_screen + 1) % TOTAL_SCREENS;
    } else if (current_screen == SCREEN_2) {
        if (input == 'u') { // Nahoru - změna vybraného nastavení
            selected_setting = (selected_setting + SETTINGS_COUNT - 1) % SETTINGS_COUNT;
        } else if (input == 'd') { // Dolů - změna vybraného nastavení
            selected_setting = (selected_setting + 1) % SETTINGS_COUNT;
        } else if (input == 'l') { // Snížení hodnoty aktuálního nastavení
            switch (selected_setting) {
                case 0:
                    break;
                case 1: // Max Temp
                    if (max_temp > min_temp) {
                        max_temp--; } break;
                case 2: // Min Temp
                    if (min_temp > 0) {
                        min_temp--;
                    }
                    if (min_temp > max_temp) { // Zajištění platného intervalu
                        min_temp = max_temp; } break;
                case 3: // Max airhum
                    if (max_airhum > min_airhum) {
                        max_airhum--; } break;
                case 4: // Min airhum
                    if (min_airhum > 0) {
                        min_airhum--;
                    }
                    if (min_airhum > max_airhum) { // Zajištění platného intervalu
                        min_airhum = max_airhum; } break;
                case 5: // Control
                    if (control > 0) {
                        control--; } break;
                case 6: // Sunrise
                    if (sunrise > 0) {
                        sunrise--; } break;
                case 7: // Sunset
                    if (sunset > 0) {
                        sunset--; } break;
                case 8: // Max soilhum
                    if (max_soilhum > min_soilhum) {
                        max_soilhum--; } break;
                case 9: // Min soilhum
                    if (min_soilhum > 0) {
                        min_soilhum--;
                    }
                    if (min_soilhum > max_soilhum) { // Zajištění platného intervalu
                        min_soilhum = max_soilhum; } break;
                case 10: // watering time
                    if (water_time > 0) {
                        water_time--; } break;
                case 11: // watering time
                    if (autowater > 0) {
                        autowater--; } break;
            }
        } else if (input == 'r') { // Zvýšení hodnoty aktuálního nastavení
            switch (selected_setting) {
                case 0:
                    break;
                case 1: // Max Temp
                    if (max_temp < 50) {
                        max_temp++;
                    }
                    if (max_temp < min_temp) { // Zajištění platného intervalu
                        max_temp = min_temp; } break;
                case 2: // Min Temp
                    if (min_temp < max_temp) {
                        min_temp++; } break;
                case 3: // Max airhum
                    if (max_airhum < 100) {
                        max_airhum++;
                    }
                    if (max_airhum < min_airhum) { // Zajištění platného intervalu
                        max_airhum = min_airhum; } break;
                case 4: // Min airhum
                    if (min_airhum < max_airhum) {
                        min_airhum++; } break;
                case 5: // Control
                    if (control < 2) {
                        control++; } break;
                case 6: // Sunrise
                    if (sunrise < 23) {
                        sunrise++; } break;
                case 7: // Sunrise
                    if (sunset < 23) {
                        sunset++; } break;
                case 8: // Max soilhum
                    if (max_soilhum < 100) {
                        max_soilhum++;
                    }
                    if (max_soilhum < min_soilhum) { // Zajištění platného intervalu
                        max_soilhum = min_soilhum; } break;
                case 9: // Min soilhum
                    if (min_soilhum < max_soilhum) {
                        min_soilhum++; } break;
                case 10: // watering time
                    if (water_time < 23) {
                        water_time++; } break;
                case 11: // watering mode
                    if (autowater < 2) {
                        autowater++; } break;
                
                    
            }
        }
    }
    else if (current_screen == SCREEN_3) {
        if (input == 'r') {
            chartest++;
        }
        else if (input == 'l')
        {
            chartest--;
        }
        
    }
}

// -- Hlavní smyčka -------------------------------------------------
int UserInterface_init(void) {
    // Inicializace LCD a UART
    HD44780_PCF8574_Init(addr);
    HD44780_PCF8574_DisplayClear(addr);
    HD44780_PCF8574_DisplayOn(addr);
    
    
    char uart_msg[10];
    uart_puts("Scanning I2C... ");
    for (uint8_t sla = 8; sla < 120; sla++)
    {
        if (twi_test_address(sla) == 0)  // If ACK from Slave
        {
            sprintf(uart_msg, "\r\n0x%x", sla);
            uart_puts(uart_msg);
        }
    }
    uart_puts("\r\nDone");
    return 0;
    }
    
    // Hlavní smyčka
int UserInterface_loop (void)
    {
        // Zpracování vstupu
        uint16_t value = uart_getc();
        if ((value & 0xff00) == 0) {
            uart_putc(value); // Výpis přijatého znaku do konzole
            HandleInput((char)value);
        }

        // Aktualizace displeje
        if (flag_update_lcd) {
            // Smazání displeje při změně obrazovky
            if (current_screen != last_screen) {
                HD44780_PCF8574_DisplayClear(addr);
                last_screen = current_screen;
            }

            // Zobrazení aktuální obrazovky
            if (current_screen == SCREEN_1) {
                LCD_DrawScreen1(); 
            } else if (current_screen == SCREEN_2) {
                LCD_DrawScreen2();
            } else if (current_screen == SCREEN_3) {
                LCD_DrawScreen3();
            } else if (current_screen == SCREEN_4) {
                LCD_DrawScreen4();
            } else if (current_screen == SCREEN_5) {
                LCD_DrawScreen5();
            }
            flag_update_lcd = 0;
        }
        return 0;
    }
    
void UserInterface_interrupt (uint8_t n_ovfs)
{
    flag_update_lcd = 1;
    if (n_ovfs % 50 == 0)
    {
        flag_tick = 1;
    }
    if (n_ovfs % 100 == 0)
    {
        flag_tick = 0;
    }
    if (n_ovfs == 180)
    {
        if (current_screen < 3)
        {
            current_screen++;
        }
        else if (current_screen == 3)
        {
            current_screen = 0;
        }
    }
}