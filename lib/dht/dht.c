#include "dht.h"

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

void dht_read_data(uint8_t *data) {
    dht_request();
    if (dht_response()) {
        for (uint8_t i = 0; i < 5; i++) {
            data[i] = dht_read_byte();
        }
    }
}