// --- Variable types defined by length ---
#include "serial.h"
#include "serial.c"

typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

void setup() {
    serial_default_mode();
    serial_set_speed(9600);
}


void read_16_bytes(int8* to) {
    for(int i = 0; i < 16; i++) {
        to[i] = serial_read_byte_blocking();
    }
}

void loop() {
    char buffer[16];
    read_16_bytes(buffer);

    serial_write_n_bytes((uint8*)buffer, 16);
    serial_break_line();
}
