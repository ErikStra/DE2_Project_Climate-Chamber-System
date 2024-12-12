/** 
 * ---------------------------------------------------+ 
 * @desc        PID library c
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2024 Artem Kostenko.
 * @email       238545@vutbr.cz
 * @author      Artem Kostenko
 * @datum       25.11.2024
 * @file        fan_PID.h
 * @version     3.0
 * @tested      Arduino nano Atmega 328p
 * ---------------------------------------------------+
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h> // For memset
#include "uart.h"
#include "pwm.h"
#include "variables.h"
#include "fan_PID.h"

#define F_CPU 16000000L

int16_t prev_error1 = 0;
int32_t integral1 = 0;

int16_t prev_error2 = 0;
int32_t integral2 = 0;

// Funkce pro řízení ventilátoru
uint8_t pid_control(int16_t actual_temp, int16_t target_temp, int16_t *prev_error, int32_t *integral, uint8_t KP, uint8_t KI, uint8_t KD) {
    // Převod hodnot zintegrovaných vynásobením 10 zpět do odpovídající škály
    int16_t error = target_temp - actual_temp;  // Rozdíl mezi cílem a skutečností

    // Aktualizace integrální složky
    *integral += error;

    // Omezení pro integrální složku (anti-windup)
    if (*integral > 500) *integral = 500;
    if (*integral < -500) *integral = -500;

    // Výpočet derivační složky
    int16_t derivative = error - *prev_error;
    *prev_error = error;

    // PID výpočet
    float output = (KP * error) + (KI * (*integral / 10.0)) + (KD * derivative);

    // Oříznutí výstupu do rozsahu 0–255
    if (output > 255) output = 255;
    if (output < 0) output = 0;

    // Návrat jako střída PWM
    return (uint8_t)output;
}

// Initialize system
int fan_PID_init(void) {
    pwm_init();  // Initialize PWM
    uart_puts("[INFO] PID system initialized.\r\n");
    return 0;
}

// Main loop for fan control
int fan_PID_loop(void) { 
     // Fan 1 (TEMP1)
    if (TEMP1 > max_temp1) {
        uint8_t pwm1 = pid_control(max_temp1, TEMP1, &prev_error1, &integral1, KP1, KI1, KD1);
        // docasne vypis na uart
        

       // if (HUM1> max_airhum && pwm1<drying_speed){
            //fan_big = drying_speed;
        //} else {
            fan_big = pwm1; //sprintf(buffer, "Fan 1 ON at %d%%\r\n", (pwm1 * 100) / 255);
        //}
    } else {
            fan_big = 0;
    }

    // Fan 2 (TEMP2)
    if (TEMP2 > max_temp2*10) {
        uint8_t pwm2 = pid_control(max_temp2*10, TEMP2, &prev_error2, &integral2, KP2, KI2, KD2);
        fan_led = pwm2;
    } else {
        fan_led = 0;
    }

    return 0;
}
