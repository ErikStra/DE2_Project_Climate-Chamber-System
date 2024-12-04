#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <stdlib.h>
#include "dht.h"  // Include the DHT library
#include "pwm.h"  // Include the PWM library
#include "variables.h" // globální proměnné (teplota, vlhkost...)

//--- Proměnné--------------------------------------------
uint8_t dht_data[5];  // Array to store data from DHT sensor

// --- Lokální funkce
void fan_control(uint8_t temperature) {
 
    if (temperature >= 30) {
        pwm_set_duty_cycle(255);  // Set PWM to 100%
        uart_puts("Fan at 100%\r\n");
    } else if (temperature >= 25) {
        pwm_set_duty_cycle(128);  // Set PWM to 50%
        uart_puts("Fan at 50%\r\n");
    } else {
        pwm_set_duty_cycle(0);  // Turn off the fan
        uart_puts("Fan OFF\r\n");
    }
}

void SaveTemp (void)
{
        dht_read_data(dht_data);

        int8_t temp_int = dht_data[2];  // Integer part of temperature
        uint8_t temp_dec = dht_data[3];  // Decimal part of temperature

        //Save temperature in TEMP1
        // s teplotou budeme pracovat v jejích 100násobcích, abychom se vyhli floatům
        TEMP1 = 100*temp_int + temp_dec;
        //např: 10,23°C => 1023

        char buffer[10];
        // Send temperature data via UART
        uart_puts("Raw Temperature: ");
        itoa(temp_int, buffer, 10);
        uart_puts(buffer);
        uart_puts(".");
        itoa(temp_dec, buffer, 10);
        uart_puts(buffer);
        uart_puts(" °C\r\n");
}

// init funkce - v main.c se zavolá v sekci initů
int FanSenzor_init(void)
    {
    // Initialize PWM
    pwm_init();
    return 0;
    }

// smyčková funkce - volá se v main.c opakovaně v sekci nekonečné smyčky
int FanSenzor_loop()
    {
        //save temperature from sensor into TEMP1
        SaveTemp();
        // Control fan
        fan_control(TEMP1);
        return 0;
        
    }

    
