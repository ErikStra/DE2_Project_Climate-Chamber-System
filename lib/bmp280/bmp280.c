/*
 * Library for Bosch BMP280 pressure sensor for AVR MCUs in plain old C.
 *
 * This library uses the TWI library by doc. Ing. Tomáš Frýza, Ph.D..
 *
 * This library is rewritten to work with TWI library. 
 *
 * Original library used custom i2ch library written by Jan "Yenya" Kasprzak, https://www.fi.muni.cz/~kas/
 * 
 * The library is distributable under the terms of the GNU General
 * Public License, version 2 only.
 *
 * Written by Jan "Erik Straka, student at FEKT VUT
 *
 * Configuration can be done by editing the top of the bmp280.c file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include "bmp280.h"
#include "twi.h" // Include TWI library

// Address of BMP280 sensor, can support (0x77) or (0x76)
#define BMP280_ADDR 0x76

// Comment out if initialized elsewhere
#define BMP280_I2CINIT 

// Provide the debugging function for sending NAME=value pairs, if needed
#ifndef BMP280_DEBUG
#define BMP280_DEBUG(name, val) do { } while (0)
#endif

// ID register address (0xD0) contains a unique identifier for the BMP280 sensor (0x58).
#define BMP280_ID_REG        0xD0
#define BMP280_ID_VAL        0x58

// Calibration data is stored in registers from 0x88 to 0xA1
#define BMP280_CAL_REG_FIRST 0x88
#define BMP280_CAL_REG_LAST  0xA1
#define BMP280_CAL_DATA_SIZE (BMP280_CAL_REG_LAST + 1 - BMP280_CAL_REG_FIRST)


// Other Registers: BMP280_STATUS_REG, BMP280_CONTROL_REG, BMP280_CONFIG_REG manage the operational state and configuration of the sensor
#define BMP280_STATUS_REG    0xF3
#define BMP280_CONTROL_REG   0xF4
#define BMP280_CONFIG_REG    0xF5

// Raw Data Registers: Pressure (0xF7) and temperature (0xFA) data are read as 20-bit values
#define BMP280_PRES_REG      0xF7
#define BMP280_TEMP_REG      0xFA
#define BMP280_RAWDATA_BYTES 6 // 3 bytes pressure, 3 bytes temperature

// Constants for altitude computation
#define BMP280_SEA_LEVEL_PRESSURE 101325 // Pa
#define BMP280_ALTITUDE_CONSTANT 0.0000225577

// Global variables for simplicity
int32_t _bmp280_temp;
uint32_t _bmp280_pres;

// This function writes a single byte to a specific register of the BMP280 sensor over the I2C interface
static void bmp280_writemem(uint8_t reg, uint8_t value)
{
    twi_start();
    twi_write((BMP280_ADDR << 1) | TWI_WRITE);
    twi_write(reg);
    twi_write(value);
    twi_stop();
}

// This function reads multiple bytes from a specific BMP280 register
void bmp280_readmem(uint8_t reg, uint8_t buff[], uint8_t bytes)
{
    uint8_t i = 0;

    // Start TWI transmission and send the register address
    twi_start();
    twi_write((BMP280_ADDR << 1) | TWI_WRITE);
    twi_write(reg);

    // Start reading data
    twi_start();
    twi_write((BMP280_ADDR << 1) | TWI_READ);
    for (i = 0; i < bytes; i++) {
        if (i == bytes - 1) {
            buff[i] = twi_read(TWI_NACK); // Last byte
        } else {
            buff[i] = twi_read(TWI_ACK); // Acknowledge remaining bytes
        }
    }
    twi_stop();
}

// This union stores calibration coefficients (read from the BMP280) needed to convert raw sensor data into usable temperature and pressure values
static union _bmp280_cal_union {
    uint8_t bytes[BMP280_CAL_DATA_SIZE];
    struct {
        uint16_t dig_t1;
        int16_t  dig_t2;
        int16_t  dig_t3;
        uint16_t dig_p1;
        int16_t  dig_p2;
        int16_t  dig_p3;
        int16_t  dig_p4;
        int16_t  dig_p5;
        int16_t  dig_p6;
        int16_t  dig_p7;
        int16_t  dig_p8;
        int16_t  dig_p9;
    };
} bmp280_cal;

// Reads all calibration registers into the bmp280_cal structure and prints debugging information for each coefficient. The calibration data is vital for compensating raw sensor data.
static void bmp280_getcalibration(void)
{
    memset(bmp280_cal.bytes, 0, sizeof(bmp280_cal));
    bmp280_readmem(BMP280_CAL_REG_FIRST, bmp280_cal.bytes, BMP280_CAL_DATA_SIZE);

    BMP280_DEBUG("T1", bmp280_cal.dig_t1);
    BMP280_DEBUG("T2", bmp280_cal.dig_t2);
    BMP280_DEBUG("T3", bmp280_cal.dig_t3);
    BMP280_DEBUG("P1", bmp280_cal.dig_p1);
    BMP280_DEBUG("P2", bmp280_cal.dig_p2);
    BMP280_DEBUG("P3", bmp280_cal.dig_p3);
    BMP280_DEBUG("P4", bmp280_cal.dig_p4);
    BMP280_DEBUG("P5", bmp280_cal.dig_p5);
    BMP280_DEBUG("P6", bmp280_cal.dig_p6);
    BMP280_DEBUG("P7", bmp280_cal.dig_p7);
    BMP280_DEBUG("P8", bmp280_cal.dig_p8);
    BMP280_DEBUG("P9", bmp280_cal.dig_p9);
}

// Initializes the BMP280 sensor
uint8_t bmp280_init(void)
{
    uint8_t buffer[1];

#ifdef BMP280_I2CINIT
    twi_init(); // Initialize TWI
    _delay_us(10);
#endif

    // Check device ID
    bmp280_readmem(BMP280_ID_REG, buffer, 1);
    if (buffer[0] != BMP280_ID_VAL) {
        return 0; // BMP280 not found
    }

    bmp280_getcalibration(); // Read calibration data
    bmp280_set_config(0, 4, 0); // 0.5 ms delay, 16x filter, no 3-wire SPI
    bmp280_set_ctrl(2, 5, 3); // T oversample x2, P oversample x16, normal mode

    return 1;
}

// Get BMP280 status
uint8_t bmp280_get_status(void)
{
    uint8_t data[1];
    bmp280_readmem(BMP280_STATUS_REG, data, 1);
    BMP280_DEBUG("Status", data[0]);
    return data[0];
}

// Configures the control register
void bmp280_set_ctrl(uint8_t osrs_t, uint8_t osrs_p, uint8_t mode)
{
    bmp280_writemem(BMP280_CONTROL_REG,
        ((osrs_t & 0x7) << 5) |
        ((osrs_p & 0x7) << 2) |
        (mode & 0x3)
    );
}

// Configures the configuration register
void bmp280_set_config(uint8_t t_sb, uint8_t filter, uint8_t spi3w_en)
{
    bmp280_writemem(BMP280_CONFIG_REG,
        ((t_sb & 0x7) << 5) |
        ((filter & 0x7) << 2) |
        (spi3w_en & 0x1)
    );
}

#define bmp280_20bit_reg(b1, b2, b3)	( \
	((int32_t)(b1) << 12) \
	| ((int32_t)(b2) << 4) \
	| ((int32_t)(b3) >> 4) \
)

// The sensor provides raw temperature and pressure data as 20-bit values. These are read from contiguous registers.
void bmp280_measure(void)
{
    uint8_t data[BMP280_RAWDATA_BYTES];
    int32_t temp_raw, pres_raw, var1, var2, t_fine;

    // Read raw data
    bmp280_readmem(BMP280_PRES_REG, data, BMP280_RAWDATA_BYTES);
    pres_raw = bmp280_20bit_reg(data[0], data[1], data[2]);
	temp_raw = bmp280_20bit_reg(data[3], data[4], data[5]);

    BMP280_DEBUG("temp_raw", temp_raw);
    BMP280_DEBUG("pres_raw", pres_raw);

	// Compute the temperature
	// Calculate fine temperature offset (t_fine)
	// This intermediate variable is used for pressure compensation
	var1 = ((((temp_raw >> 3) - ((int32_t)bmp280_cal.dig_t1 << 1)))
		* ((int32_t)bmp280_cal.dig_t2)) >> 11;
	var2 = (((((temp_raw >> 4) - ((int32_t)bmp280_cal.dig_t1))
		* ((temp_raw >> 4) - ((int32_t)bmp280_cal.dig_t1))) >> 12)
		* ((int32_t)bmp280_cal.dig_t3)) >> 14;
	t_fine = var1 + var2;
	_bmp280_temp = (t_fine * 5 + 128) >> 8;
	BMP280_DEBUG("temperature x 100", _bmp280_temp);

    if (var1 == 0) {
        _bmp280_pres = 0; // Avoid division by zero
    } else {
        _bmp280_pres = (((uint32_t)(1048576 - pres_raw) - (var2 >> 12)) * 3125);
        if (_bmp280_pres < 0x80000000) {
            _bmp280_pres = (_bmp280_pres << 1) / ((uint32_t)var1);
        } else {
            _bmp280_pres = (_bmp280_pres / (uint32_t)var1) * 2;
        }
        var1 = (((int32_t)bmp280_cal.dig_p9) * ((int32_t)((_bmp280_pres >> 3) * (_bmp280_pres >> 3)) >> 13)) >> 12;
        var2 = (((int32_t)(_bmp280_pres >> 2)) * ((int32_t)bmp280_cal.dig_p8)) >> 13;
        _bmp280_pres = (uint32_t)((int32_t)_bmp280_pres + ((var1 + var2 + bmp280_cal.dig_p7) >> 4));
    }

    BMP280_DEBUG("pressure x100", _bmp280_pres);
}

// Calculate altitude based on pressure
double bmp280_getaltitude()
{
    double alt;
	alt = (1 - pow(_bmp280_pres/(double)BMP280_SEA_LEVEL_PRESSURE, 0.1903)) / BMP280_ALTITUDE_CONSTANT;

	BMP280_DEBUG("alt x 100", (long)(alt * 100));

	return alt;
}

