#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <stdlib.h>

#define DHT_PIN PD2  // Подключите DHT-сенсор к пину PD2
#define FAN_PIN PB1  // Подключите вентилятор к пину D9 (PB1)

// -- Глобальные переменные --
volatile uint8_t dht_data[5];  // Массив для хранения данных от DHT

// -- Функции --
void dht_request(void) {
    DDRD |= (1 << DHT_PIN);  // Установить DHT_PIN как выход
    PORTD &= ~(1 << DHT_PIN); // Низкий уровень на 20 мс
    _delay_ms(20);
    PORTD |= (1 << DHT_PIN);  // Высокий уровень на 20-40 мкс
    _delay_us(40);
    DDRD &= ~(1 << DHT_PIN);  // Установить DHT_PIN как вход
}

uint8_t dht_response(void) {
    uint8_t response = 0;
    _delay_us(40);
    if (!(PIND & (1 << DHT_PIN))) {  // Ожидание низкого уровня
        _delay_us(80);
        if (PIND & (1 << DHT_PIN)) {  // Ожидание высокого уровня
            response = 1;
        }
        _delay_us(40);
    }
    return response;
}

uint8_t dht_read_byte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        while (!(PIND & (1 << DHT_PIN)));  // Ожидание высокого уровня
        _delay_us(30);
        if (PIND & (1 << DHT_PIN)) {
            byte |= (1 << (7 - i));  // Если высокий уровень, записать 1
        }
        while (PIND & (1 << DHT_PIN));  // Ожидание окончания бита
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
        OCR1A = 255;  // Установить ШИМ на 100%
        uart_puts("Fan at 100%\r\n");
    } else if (temperature >= 25) {
        OCR1A = 128;  // Установить ШИМ на 50%
        uart_puts("Fan at 50%\r\n");
    } else {
        OCR1A = 0;  // Вентилятор выключен
        uart_puts("Fan OFF\r\n");
    }
}

int main(void) {
    char buffer[10];
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    sei();

    // Настройка FAN_PIN как выход и ШИМ
    DDRB |= (1 << FAN_PIN);
    
    // Настройка таймера для ШИМ на пине FAN_PIN (PB1/OC1A)
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);  // ШИМ, быстрый режим
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // Без предделителя
    ICR1 = 255;  // Верхнее значение для 8-битного ШИМ

    while (1) {
        dht_read_data();

        uint8_t temp_int = dht_data[2];  // Целая часть температуры
        uint8_t temp_dec = dht_data[3];  // Дробная часть температуры

        // Отправить данные температуры через UART
        uart_puts("Raw Temperature: ");
        itoa(temp_int, buffer, 10);
        uart_puts(buffer);
        uart_puts(".");
        itoa(temp_dec, buffer, 10);
        uart_puts(buffer);
        uart_puts(" °C\r\n");

        // Управление вентилятором
        fan_control(temp_int);

        _delay_ms(2000);  // Задержка между чтениями
    }

    return 0;
}