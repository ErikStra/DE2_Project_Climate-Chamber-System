#include "bmp280.h"
#include <twi.h>        // Peter Fleury's I2C library
#include <util/delay.h> // For delay functions

// BMP280 register addresses
#define BMP280_TEMP_MSB_REG   0xFA
#define BMP280_CTRL_MEAS_REG  0xF4

// BMP280 Initialization
void BMP280_Init(void) {
    twi_start();
    twi_write((BMP280_ADDR << 1) | TWI_WRITE);
    twi_write(BMP280_CTRL_MEAS_REG);
    twi_write(0x25); // Temp oversampling x1, Forced mode
    twi_stop();
}

// Read Temperature from BMP280
int32_t BMP280_Read_Temperature(void) {
    uint8_t buffer[3];
    int32_t adc_T;

    // Read temperature registers
    twi_readfrom_mem_into(BMP280_ADDR, BMP280_TEMP_MSB_REG, buffer, 3);

    adc_T = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | (buffer[2] >> 4);

    // Temperature compensation (Simplified)
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - (16384)) * 3038) >> 12);
    var2 = ((((adc_T >> 4) - (16384)) * ((adc_T >> 4) - (16384))) >> 12) * 2834;
    var2 = var2 >> 14;
    T = var1 + var2;
    return T / 100; // Return temperature in Celsius
}
