#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <stdlib.h>

#define DHT_PIN PD2  // Connect the DHT sensor to pin PD2
#define FAN_PIN PB1  // Connect the fan to pin D9 (PB1)

// -- Global Variables --
volatile uint8_t dht_data[5];  // Array to store data from DHT sensor

// -- Functions --
void dht_request(void) {
    DDRD |= (1 << DHT_PIN);  // Set DHT_PIN as output
    PORTD &= ~(1 << DHT_PIN); // Low level for 20 ms
    _delay_ms(20);
    PORTD |= (1 << DHT_PIN);  // High level for 20-40 µs
    _delay_us(40);
    DDRD &= ~(1 << DHT_PIN);  // Set DHT_PIN as input
}

uint8_t dht_response(void) {
    uint8_t response = 0;
    _delay_us(40);
    if (!(PIND & (1 << DHT_PIN))) {  // Wait for low level
        _delay_us(80);
        if (PIND & (1 << DHT_PIN)) {  // Wait for high level
            response = 1;
        }
        _delay_us(40);
    }
    return response;
}

uint8_t dht_read_byte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        while (!(PIND & (1 << DHT_PIN)));  // Wait for high level
        _delay_us(30);
        if (PIND & (1 << DHT_PIN)) {
            byte |= (1 << (7 - i));  // If high level, write 1
        }
        while (PIND & (1 << DHT_PIN));  // Wait for end of bit
    }
    return byte;
}

void dht_read_data(void) {
    dht_request();
    if (dht_response()) {
        for (uint8_t i = 0; i < 5; i++) {
            dht_data[i] = dht_read_byte();
        }
        uint8_t checksum = dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3];
        if (checksum != dht_data[4]) {
            uart_puts("Checksum error\r\n");
        }
    } else {
        uart_puts("No response from sensor\r\n");
    }
}

void fan_control(uint8_t temperature) {
    if (temperature >= 30) {
        OCR1A = 255;  // Set PWM to 100%
        uart_puts("Fan at 100%\r\n");
    } else if (temperature >= 25) {
        OCR1A = 128;  // Set PWM to 50%
        uart_puts("Fan at 50%\r\n");
    } else {
        OCR1A = 0;  // Turn off the fan
        uart_puts("Fan OFF\r\n");
    }
}

int main(void) {
    char buffer[10];
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();

    // Set FAN_PIN as output and PWM
    DDRB |= (1 << FAN_PIN);
    
    // Set timer for PWM on FAN_PIN (PB1/OC1A)
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);  // PWM, fast mode
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // No prescaler
    ICR1 = 255;  // Top value for 8-bit PWM

    while (1) {
        dht_read_data();

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
