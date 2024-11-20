#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

// BMP280 I2C address
#define BMP280_ADDR 0x76 // Adjust to 0x77 if needed

// Function Prototypes
void BMP280_Init(void);
int32_t BMP280_Read_Temperature(void);

#endif // BMP280_H
