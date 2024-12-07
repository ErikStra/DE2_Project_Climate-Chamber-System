#ifndef DHT_H
#define DHT_H

#include <avr/io.h>
#include <util/delay.h>

// Pin definitions
#define DHT_PIN PD2  // Define the pin for DHT sensor

// Function declarations
void dht_request(void);
uint8_t dht_response(void);
uint8_t dht_read_byte(void);
void dht_read_data(uint8_t *data);
void dht_update_temp1(void);

#endif  // DHT_H
