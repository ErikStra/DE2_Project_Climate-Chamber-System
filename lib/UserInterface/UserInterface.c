#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <uart.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include "hd44780pcf8574.h" // LCD i2C library
#include <stdio.h>          // Standardní knihovna
#include <string.h>
#include <ctype.h>          // Pro funkci isdigit()

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
#define SETTINGS_COUNT 11



// Pole pro zobrazení časových plánů
const char *ONOFF_options[3] = {"off", "on "};

// Další proměnné
char addr = PCF8574_ADDRESS; // Adresa LCD
uint8_t current_screen = SCREEN_1;
uint8_t last_screen = 255;  // Poslední zobrazená obrazovka (pro detekci změny)
uint8_t selected_setting = 0;

//Flagy pro interrupty
volatile uint8_t flag_update_lcd = 0;
volatile uint8_t flag_tick = 0;

//UART
uint8_t awaiting_input = 0;
size_t getcommand_buffer_index = 0;  
char getcommand_buffer[75] = {0}; // Buffer pro příjem dat
uint8_t input_complete = 0;


// -- Funkce pro vykreslování ---------------------------------------
void LCD_DrawScreen2() {
    char buffer[14];
    char buffer2[11];

    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "2/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "5");

    switch (selected_setting) {
        case 0:
            snprintf(buffer, 14, "SETTINGS    ");
            snprintf(buffer2, 5, "udrl     ");

            break;
        case 1: // Max Temperature (TEMP1)
            snprintf(buffer, 14, "Max Temp.        ");
            snprintf(buffer2, 6, "%d.%d%cC    ", max_temp1/10, max_temp1%10, 223);
            break;
        case 2: // Max LED temperature (TEMP2)
            snprintf(buffer, 14, "Max LED temp.   ");
            snprintf(buffer2, 6, "%d.%d%cC    ", max_temp2/10, max_temp2%10, 223);
            break;
        case 3: // Max AirHum
            snprintf(buffer, 14, "Max Air Hum. ");
            snprintf(buffer2, 6, "%d %%    ", max_airhum);
            break;            
        case 4: // Control 
            snprintf(buffer, 14, "Climate Cont.");
            snprintf(buffer2, 11, "%s   ", ONOFF_options[control]);
            break;    
        case 5: // Light ON time (Hour)
            snprintf(buffer, 14, "Sunrise      ");
            snprintf(buffer2, 7, "%d:00  ", sunrise);
            break;
        case 6: //Light OFF time (HOUR)
            snprintf(buffer, 14, "Sunset       ");
            snprintf(buffer2, 7, "%d:00  ", sunset);
            break;
        case 7:
            snprintf(buffer, 14, "Light Cont.  ");
            snprintf(buffer2, 11, "%s   ", ONOFF_options[autolight]);
            break;   
        case 8: // Min SoilHum
            snprintf(buffer, 14, "Min Soil Hum.");
            snprintf(buffer2, 6, "%d %% ", min_soilhum);
            break;
        case 9: //watering time (HOUR)
            snprintf(buffer, 14, "Watering Time");
            snprintf(buffer2, 7, "%d:00 ", water_time);
            break;
        case 10: // Autowatering time options
            snprintf(buffer, 14, "Auto watering");
            snprintf(buffer2, 11, "%s   ", ONOFF_options[autowater]);
            break;
    }
    //řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    HD44780_PCF8574_DrawString(addr, buffer2);
}
//domovská obrazovka - zobrazuje teplotu 1, vlhkost 1 a 2 a čas
void LCD_DrawScreen1() {
    char buffer[16];
    char buffer2[4];

   /*  // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "1/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "1"); */

    // Humidity 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    int_to_string(HUM1, buffer2);
    snprintf(buffer, 16, "H1=%s%%", buffer2);
    HD44780_PCF8574_DrawString(addr, buffer);

    //humidity 2
    HD44780_PCF8574_PositionXY(addr, 10, 0);
    //int_to_string(HUM2, buffer2);
    snprintf(buffer, 16, "H2=%d%%", HUM2/10);
    HD44780_PCF8574_DrawString(addr, buffer);

    // teplota 1
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    int_to_string(TEMP1, buffer2);
    snprintf(buffer, 16, "T1=%s%c", buffer2, 223);
    HD44780_PCF8574_DrawString(addr, buffer);

    //hodiny    
    HD44780_PCF8574_PositionXY(addr, 8, 1);
    if (1)//flag_tick == 1)
    {snprintf(buffer, 16, "%02d:%02d:%02d", hours, minutes, secs);}
    else
    {snprintf(buffer, 16, "%02d %02d", hours, minutes);}
    HD44780_PCF8574_DrawString(addr, buffer);
}

void LCD_DrawScreen3() { //prázdná obrazovka, kterou možná na něco použiju pozdějc
    char buffer[17];
    uint8_t buffer2 = 0;
    
    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "3/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "4");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "PMP:%s S%d A%d", ONOFF_options[pump], min_soilhum, HUM2/10);
    HD44780_PCF8574_DrawString(addr, buffer);

    HD44780_PCF8574_PositionXY(addr, 0, 1);
    if (autowater)
    {
        if (wlevel)
        {
            if (water_time>hours)
                {buffer2 = water_time-hours;}
            else
                {buffer2 = 24-hours+water_time;}
            snprintf(buffer, 17, "watering in %dhr", buffer2);
        }
        else
        {
            snprintf(buffer, 17, "refill water");
        } 
    }
    else
    {
        snprintf(buffer, 17, "autowatering off");
    }
    HD44780_PCF8574_DrawString(addr, buffer);
    
    
}

void LCD_DrawScreen4() { // obrazovka se statistikami ventilátorů
    char buffer[17];
    
    // indikátor
    /* HD44780_PCF8574_PositionXY(addr, 15, 0);
    //snprintf(buffer, 3, "4/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "3"); */

    //ventilátor 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "F1=%-2d S%d.%dA%d.%d", (fan_big*100)/255, max_temp1/10, max_temp1 %10, TEMP1/10, TEMP1%10);
    HD44780_PCF8574_DrawString(addr, buffer);

    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    snprintf(buffer, 17, "F2=%-2d S%d.%dA%d.%d", (fan_led*100)/255, max_temp2/10, max_temp2 %10, TEMP2/10, TEMP2%10);
    HD44780_PCF8574_DrawString(addr, buffer);
}

void LCD_DrawScreen5() { //obrazovka se statistikami osvětlení
    char buffer[17];
    
    // indikátor
    HD44780_PCF8574_PositionXY(addr, 15, 0);
    snprintf(buffer, 3, "5/%d", TOTAL_SCREENS);
    HD44780_PCF8574_DrawString(addr, "2");

    // Řádek 1
    HD44780_PCF8574_PositionXY(addr, 0, 0);
    snprintf(buffer, 17, "LED:%3s %d.%dC",ONOFF_options[LED], TEMP2/100, TEMP2%100);
    HD44780_PCF8574_DrawString(addr, buffer);

    uint8_t buffer2 = 0;
    // Řádek 2
    HD44780_PCF8574_PositionXY(addr, 0, 1);
    if (LED)
    {
        if (sunset>hours)
        {buffer2 = sunset-hours;}
        else
        {buffer2 = 24-hours + sunset;}  
        snprintf(buffer, 17, "sunset in %dhr", buffer2);

    }
    else
    {
        if (sunrise>hours)
        {buffer2 = sunrise-hours;}
        else
        {buffer2 = 24-hours + sunrise;}
        snprintf(buffer, 17, "sunrise in %dhr", buffer2);
    }
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
                    if (max_temp1 > 0) {
                        max_temp1--; } break;
                case 2: // Max LED temp
                    if (max_temp2 > 0) {
                        max_temp2--;} break;
                case 3: // Max airhum
                    if (max_airhum > 0) {
                        max_airhum--; } break;        
                case 4: // Control
                    if (control > 0) {
                        control--; } break;
                case 5: // Sunrise
                    if (sunrise > 0) {
                        sunrise--; } break;
                case 6: // Sunset
                    if (sunset > 0) {
                        sunset--; } break;
                case 7: // autolight
                    if (autolight > 0) {
                        autolight--; } break;
                case 8: // Min soilhum
                    if (min_soilhum > 0) {
                        min_soilhum--; } break;
                case 9: // watering time
                    if (water_time > 0) {
                        water_time--; } break;
                case 10: // autowatering
                    if (autowater > 0) {
                        autowater--; } break;
            }
        } else if (input == 'r') { // Zvýšení hodnoty aktuálního nastavení
            switch (selected_setting) {
                case 0:
                    break;
                case 1: // Max Temp
                    if (max_temp1 < 500) {
                        max_temp1++;} break;
                case 2: // Min Temp
                    if (max_temp2 < 500) {
                        max_temp2++; } break;
                case 3: // Max airhum
                    if (max_airhum < 100) {
                        max_airhum++;} break;
                case 4: // Control
                    if (control < 1) {
                        control++; } break;
                case 5: // Sunrise
                    if (sunrise < 23) {
                        sunrise++; } break;
                case 6: // Sunrise
                    if (sunset < 23) {
                        sunset++; } break;
                case 7: // watering mode
                    if (autolight < 1) {
                        autolight++; } break;
                case 8: // Min soilhum
                    if (min_soilhum < 100) {
                        min_soilhum++; } break;
                case 9: // watering time
                    if (water_time < 23) {
                        water_time++; } break;
                case 10: // watering mode
                    if (autowater < 1) {
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
    uart_puts("[INFO]Scanning I2C... ");
    for (uint8_t sla = 8; sla < 120; sla++)
    {
        if (twi_test_address(sla) == 0)  // If ACK from Slave
        {
            sprintf(uart_msg, "\r\n0x%x", sla);
            uart_puts(uart_msg);
        }
    }
    uart_puts("\r\n...Done\r\n");
    return 0;
    }
    
    // Hlavní smyčka
void UserInterface_input_loop (void)
    {
        // Zpracování vstupu
        uint16_t value = uart_getc();
        if ((value & 0xff00) == 0) {
            uart_putc(value); // Výpis přijatého znaku do konzole
            HandleInput((char)value);
        }
    }

void UserInterface_display_loop (uint16_t n_ovfs){

            if (n_ovfs % 30 == 0)
            {
                flag_tick = 1;
            }
            if (n_ovfs % 60 == 0)
            {
                flag_tick = 0;
            }
            if (n_ovfs == 240)       // automatické přepínání obrazovek
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
        

        // Aktualizace displeje
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
                    
    }

void int_to_string(int16_t input, char *output) { //converts int temperature in format 223 to 22.3, asi se na ni vybodnu, je celkem na prd
sprintf(output, "%d.%d", input/ 10, input % 10);
}

void processCommand(char *command) {
    char buffer[75];
    remove_trailing_newline(command);
    
    if (strcmp(command, "upload") >= 0) {
        cleanString(command);
        if (count(';', command) == 9){
        char *token;
        int16_t num[9];
        // Get the first token (before the first comma)
        token = strtok(command, ";");
        uart_puts("\n\r");
        uint8_t i = 0;
        while (token != NULL) {// Loop through the string to get all tokens
            // Convert the token to an integer
            num[i] = atoi(token);
            
            // Print the integer (or you can store it in an array if needed)
            snprintf(buffer, 15, "Value %d : %2d", i, num[i]);
            uart_puts(buffer); uart_puts("\n\r");
            // Get the next token
            token = strtok(NULL, ";");
            i++;
            }
                max_temp1 = num[0];
                max_temp2 = num[1];
                max_airhum = num[2];
                min_soilhum = num[3];
                sunrise = num[4];
                sunset = num[5];
                water_time = num[6];
                // autowater = num[7];
                // autolight = num[8];
                // control = num[9];
                // water_delay = num[10];
                // drying_speed = num[11];
                KP1 = num[7]/10.0;
                KI1 = num[8]/10.0;
                KD1 = num[9]/10.0;
                // KP2 = num[15];
                // KI2 = num[16];
                // KD2 = num[17];


            snprintf(buffer, 100, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", max_temp1, max_temp2, max_airhum, min_soilhum, sunrise, sunset, water_time, autowater, autolight, control,  water_delay, drying_speed, KP1*10, KI1*10, KD1*10, KP2*10, KI2*10, KD2*10);
            uart_puts(buffer); 
            uart_puts("\r\n[INFO] Settings saved\n");
        } else{
                if (count(';', command) > 5)
                {
                    uart_puts("[ERROR] too many arguments.\r\n");
                } else{
                    uart_puts("[ERROR] not enough arguments.\r\n");
                }
                
                uart_puts("\r\n       enter parameters as int numbers searated by semicoln\n");
                //uart_puts("       set(MaxT1*10 MaxT2*10 MaxH1 MinH2 SR SS water_time autowater autolight control water_delay drying_speed KP1*10 KI1*10 KD1*10 KP2*10 KI2*10 KD2*10) \n");
                uart_puts("       set(MaxT1*10 MaxT2*10 MaxH1 MinH2 SR SS water_time KP1*10 KI1*10 KD1*10) \n");
        
        /* code */
        }   

                       
        memset(getcommand_buffer, '\0', sizeof(buffer)); // Vymaž buffer
        getcommand_buffer_index = 0;
    }
    
    else if (strcmp(command, "stats") >= 0) {
            uart_puts("\n\r");
            snprintf(buffer, sizeof(buffer), "%d.%d;%d.%d;%d.%d;%d.%d;%d;%d;%d;%d;%d;%d;%d", TEMP1/10, TEMP1 %10, TEMP2/100, TEMP2%100, HUM1/10, HUM1%10, HUM2/10, HUM2%10, LED, fan_big, fan_led, wlevel, hours, minutes, secs);
            uart_puts(buffer);
            uart_puts("\n\r");
            /* snprintf(buffer, 100, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", max_temp1, max_temp2, max_airhum, min_soilhum, sunrise, sunset, water_time, autowater, autolight, control,  water_delay, drying_speed, KP1, KI1, KD1, KP2, KI2, KD2);
            uart_puts(buffer); */
    }       
    else {
        uart_puts("\r\n[ERROR] Unknown command\n");
    } 

    memset(buffer, '\0', sizeof(buffer)); // Vymažeme buffer


}

void remove_trailing_newline(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) *pos = '\0';
    if ((pos = strchr(str, '\r')) != NULL) *pos = '\0';
}




int uart_getcommand(char *output){
    
     

    unsigned int received = uart_getc();
    if (output != NULL) {
        
    }
    // Zpracování, pokud přijde platný znak
    if (!(received & UART_NO_DATA)) {
        char c = (char)received;
        HandleInput(c);
        // Echo znaku zpět na UART
        uart_putc(c);

        if (c == '\n' || c == '\r') { // když stisknut Enter
            if (getcommand_buffer_index > 0) { //jestli buffer není prázdný
                getcommand_buffer[getcommand_buffer_index] = '\0';
                uart_puts("\r");   
                uart_puts("received : ");
                uart_puts(getcommand_buffer);   // Vypíšeme obsah bufferu pro debug
                uart_puts("\r\n"); 
                processCommand(getcommand_buffer);
                
                // Reset bufferu a buffer_indexu
                memset(getcommand_buffer, '\0', sizeof(getcommand_buffer)); // Vymažeme buffer
                getcommand_buffer_index = 0;
            }
        } else {
            // Přidáme znak do bufferu, pokud není buffer plný
            if (getcommand_buffer_index < sizeof(getcommand_buffer) - 1) {
                getcommand_buffer[getcommand_buffer_index++] = c;
            } else {
                uart_puts("\r\n[ERROR] Buffer overflow, resetting...\r\n");
                memset(getcommand_buffer, '\0', sizeof(getcommand_buffer)); // Vymažeme buffer
                getcommand_buffer_index = 0;
            }
        }
    }
}


void cleanString(char *input) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        // Pokud je znak číslice nebo středník, přidáme ho do výsledného řetězce
        if (isdigit(input[i]) || input[i] == ';') {
            input[j++] = input[i];
        }
        i++;
    }
    // Ukončíme řetězec na správném místě
    input[j] = '\0';
}

int count(char character, char *str) {
    int count = 0;
    while (*str) {
        if (*str == character) {
            count++;
        }
        str++;
    }
    return count;
}