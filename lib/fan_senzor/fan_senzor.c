#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <stdlib.h>
#include "dht.h"  // Include the DHT library
#include "pwm.h"  // Include the PWM library
#include "variables.h" // global variables

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

int FanSenzor_init(void)
    {
    char buffer[10];
    uint8_t dht_data[5];  // Array to store data from DHT sensor
    // Initialize PWM
    pwm_init();
    return 0;
    }

int FanSenzor_loop()
    {
        dht_read_data(dht_data);

        uint8_t temp_int = dht_data[2];  // Integer part of temperature
        uint8_t temp_dec = dht_data[3];  // Decimal part of temperature

        // Send temperature data via UART
        uart_puts("Raw Temperature: ");
        itoa(temp_int, buffer, 10);
        uart_puts(buffer);
        uart_puts(".");
        itoa(temp_dec, buffer, 10);
        uart_puts(buffer);
        uart_puts(" °C\r\n");

        // Control fan
        fan_control(temp_int);

        _delay_ms(2000);  // Delay between readings
    }

    return 0;
}
