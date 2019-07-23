#include <avr/io.h>
#include <stdio.h>
#include "serial.h"

void serial_set_mode(data_size_t size, parity_mode_t parity, uint8 two_stop_bits) {
    UCSR0B = 0;
    UCSR0C = 0;

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Enables reception and transmission.

    if(size & (1 << 2)) {
        UCSR0B |= (1 << UCSZ02);
    }

    UCSR0C |= (parity << UPM00);

    if(two_stop_bits) {
        UCSR0C |= (1 << USBS0);
    }

    UCSR0C |= ((size & 3) << UCSZ00);
}

void serial_default_mode() {
    serial_set_mode(DATA_SIZE_8, PARITY_NONE, 0);
}

void __serial_set_speed(uint16 ubrr) {
    UBRR0L = ubrr & 0xff;
    UBRR0H = ubrr >> 8;
}

void serial_write_byte(uint8 ch) {
    // NOTE(erick): Waiting for
    while(!(UCSR0A & (1 << UDRE0)));

    UDR0 = ch;
}

void serial_write_char(char ch) {
    serial_write_byte((uint8) ch);
}

void serial_write_terminated_string(char* str) {
    while(*str) {
        serial_write_byte(*str);
        str++;
    }
}

void serial_write_n_bytes(uint8* bytes, uint8 n_bytes) {
    for(uint8 i = 0; i < n_bytes; i++) {
        serial_write_byte(*bytes);
        bytes++;
    }
}

void serial_write_n_chars(char* chars, uint8 n_chars) {
    serial_write_n_bytes((uint8*) chars, n_chars);
}

void serial_break_line() {
    serial_write_byte('\n');
}

void serial_write_hex(uint8 n) {
    char buffer[4];
    sprintf(buffer, "%x", n);
    serial_write_terminated_string(buffer);
}

void serial_write_int8(int8 n) {
    char buffer[8];
    sprintf(buffer, "%d", n);
    serial_write_terminated_string(buffer);
}

void serial_write_uint8(uint8 n) {
    char buffer[8];
    sprintf(buffer, "%u", n);
    serial_write_terminated_string(buffer);
}

void serial_write_int16(int16 n) {
    char buffer[8];
    sprintf(buffer, "%d", n);
    serial_write_terminated_string(buffer);
}

void serial_write_uint16(uint16 n) {
    char buffer[8];
    sprintf(buffer, "%u", n);
    serial_write_terminated_string(buffer);
}

uint8 serial_has_data() {
    return (UCSR0A & (1 << RXC0));
}

uint8 serial_read_byte() {
    return UDR0;
}

uint8 serial_read_byte_blocking() {
    while(!(UCSR0A & (1 << RXC0)));

    return UDR0;
}
