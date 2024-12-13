/* 
Example code for reading and sending data from BMP280 over UART
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include "twi.h"
#include "uart.h" // Include Peter Fleury's UART library
#include "../../include/timer.h"
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "bmp280.h"

//Baud rate of UART communication
#define UART_BAUD_RATE 9600

int32_t temp;
char string[3];

// this function can also be used as BMP280_DEBUG(name, val) in bmp280.c.
#define UART_BUFLEN 10
void uart_print(char *name, long val)
{
	char debug_buffer[UART_BUFLEN];

        uart_puts(name);
        uart_puts(" = ");

        ltoa((val), debug_buffer, UART_BUFLEN);
        uart_puts(debug_buffer);
        uart_puts("\n");
}

int main(void) {
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	uart_puts("start\n");
	// twi_init(); not needed - called from bmp280_init()
	bmp280_init();

	// enable IRQs
	sei();

	while(1) {
		//uart_print("status", bmp280_get_status());
		bmp280_measure();
        	temp = bmp280_gettemperature();
        	itoa((temp), string, 10);
		//uart_print("temperature x 100", bmp280_gettemperature());
        	uart_puts(string);
		//uart_print("pressure x 100   ", bmp280_getpressure());
		//uart_print("altitude x 100   ", 100*bmp280_getaltitude());
		uart_puts("\n");
		_delay_ms(1000);
	}

	return 0;
}
