#ifndef SERIAL_H
#define SERIAL_H 1
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

typedef enum {
    DATA_SIZE_5 = 0,
    DATA_SIZE_6 = 1,
    DATA_SIZE_7 = 2,
    DATA_SIZE_8 = 3,
    DATA_SIZE_9 = 7
} data_size_t;

typedef enum {
    PARITY_NONE = 0,
    PARITY_EVEN = 2,
    PARITY_ODD  = 3
} parity_mode_t;

#define FOSC 16000000L
#define __UBRR(BAUD) ((FOSC / (16L * BAUD)) - 1L)
#define serial_set_speed(BAUD) __serial_set_speed(__UBRR(BAUD))

void serial_set_mode(data_size_t size, parity_mode_t parity, uint8 two_stop_bits);
void serial_default_mode();
void __serial_set_speed(uint16 ubrr);

void serial_write_byte(uint8 ch);
void serial_write_char(char ch);
void serial_write_n_bytes(uint8* bytes, uint8 n_bytes);
void serial_write_n_chars(char* chars, uint8 n_chars);
void serial_write_terminated_string(char* str);
void serial_break_line();
void serial_write_hex(uint8 n);
void serial_write_int8(int8 n);
void serial_write_uint8(uint8 n);
void serial_write_int16(int16 n);
void serial_write_uint16(uint16 n);

uint8 serial_has_data();
uint8 serial_read_byte();
uint8 serial_read_byte_blocking();

#endif
