#include "serial.h"
#include "serial.c"

// --- Variable types defined by length ---
typedef char int8;
typedef byte uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

// Pins from 1 to 6 are handled by PORTB
// Pin 7 is handled by PORTD2
// Pin 8 is assumed to be GROUND
// Pins from 9 to 14 are handled by PORTC
// Pin 15 is handled by PORTD3
// Pin 16 is assumed to be VCC

#define nop() __asm__ volatile("nop\n\t")

void setup() {
    serial_default_mode();
    serial_set_speed(9600);

    DDRB &= 0xc0;
    DDRC &= 0xc0;
    DDRD &= 0xfc;
}

void print_str(char* str) {
    serial_write_n_chars(str, 16);
    serial_break_line();
}

uint8 input_or_output(char c) {
    if(c == '0' || c == '1' || c == 'C') {
        return 1;
    }

    return 0;
}

void set_ports(int8* str) {
    DDRB |=(input_or_output(str[0]) << 0);
    DDRB |=(input_or_output(str[1]) << 1);
    DDRB |=(input_or_output(str[2]) << 2);
    DDRB |=(input_or_output(str[3]) << 3);
    DDRB |=(input_or_output(str[4]) << 4);
    DDRB |=(input_or_output(str[5]) << 5);

    DDRD |= (input_or_output(str[6]) << 2);

    DDRC |=(input_or_output(str[8]) << 0);
    DDRC |=(input_or_output(str[9]) << 1);
    DDRC |=(input_or_output(str[10]) << 2);
    DDRC |=(input_or_output(str[11]) << 3);
    DDRC |=(input_or_output(str[12]) << 4);
    DDRC |=(input_or_output(str[13]) << 5);

    DDRD |= (input_or_output(str[14]) << 3);
}

void write_ports(int8* str) {
    PORTB &= 0xc0;
    PORTC &= 0xc0;
    PORTD &= 0xf3;

    if(input_or_output(str[0])) {
        PORTB |= (str[0] == '1' ? 1 : 0) << 0;
    }
    if(input_or_output(str[1])) {
        PORTB |= (str[1] == '1' ? 1 : 0) << 1;
    }
    if(input_or_output(str[2])) {
        PORTB |= (str[2] == '1' ? 1 : 0) << 2;
    }
    if(input_or_output(str[3])) {
        PORTB |= (str[3] == '1' ? 1 : 0) << 3;
    }
    if(input_or_output(str[4])) {
        PORTB |= (str[4] == '1' ? 1 : 0) << 4;
    }
    if(input_or_output(str[5])) {
        PORTB |= (str[5] == '1' ? 1 : 0) << 5;
    }

    if(input_or_output(str[6])) {
        PORTD |= (str[6] == '1' ? 1 : 0) << 2;
    }

    if(input_or_output(str[8])) {
        PORTC |= (str[8] == '1' ? 1 : 0) << 0;
    }
    if(input_or_output(str[9])) {
        PORTC |= (str[9] == '1' ? 1 : 0) << 1;
    }
    if(input_or_output(str[10])) {
        PORTC |= (str[10] == '1' ? 1 : 0) << 2;
    }
    if(input_or_output(str[11])) {
        PORTC |= (str[11] == '1' ? 1 : 0) << 3;
    }
    if(input_or_output(str[12])) {
        PORTC |= (str[12] == '1' ? 1 : 0) << 4;
    }
    if(input_or_output(str[13])) {
        PORTC |= (str[13] == '1' ? 1 : 0) << 5;
    }

    if(input_or_output(str[14])) {
        PORTD |= (str[14] == '1' ? 1 : 0) << 3;
    }
}

void unclock_ports(char* str) {
    //
    // uprising clock.
    //
    if(str[0] == 'C') {
        PORTB &= ~(1 << 0);
    }

    if(str[1] == 'C') {
        PORTB &= ~(1 << 1);
    }

    if(str[2] == 'C') {
        PORTB &= ~(1 << 2);
    }

    if(str[3] == 'C') {
        PORTB &= ~(1 << 3);
    }

    if(str[4] == 'C') {
        PORTB &= ~(1 << 4);
    }

    if(str[5] == 'C') {
        PORTB &= ~(1 << 5);
    }

    if(str[6] == 'C') {
        PORTD &= ~(1 << 2);
    }

    if(str[8] == 'C') {
        PORTC &= ~(1 << 0);
    }

    if(str[9] == 'C') {
        PORTC &= ~(1 << 1);
    }

    if(str[10] == 'C') {
        PORTC &= ~(1 << 2);

    }

    if(str[11] == 'C') {
        PORTC &= ~(1 << 3);
    }

    if(str[12] == 'C') {
        PORTC &= ~(1 << 4);
    }

    if(str[13] == 'C') {
        PORTC &= ~(1 << 5);
    }

    if(str[14] == 'C') {
        PORTD &= ~(1 << 3);
    }

    //
    // Down-going clock
    //
    if(str[0] == 'c') {
        PORTB |= (1 << 0);
    }

    if(str[1] == 'c') {
        PORTB |= (1 << 1);
    }

    if(str[2] == 'c') {
        PORTB |= (1 << 2);
    }

    if(str[3] == 'c') {
        PORTB |= (1 << 3);
    }

    if(str[4] == 'c') {
        PORTB |= (1 << 4);
    }

    if(str[5] == 'c') {
        PORTB |= (1 << 5);
    }

    if(str[6] == 'c') {
        PORTD |= (1 << 2);
    }

    if(str[8] == 'c') {
        PORTC |= (1 << 0);
    }

    if(str[9] == 'c') {
        PORTC |= (1 << 1);
    }

    if(str[10] == 'c') {
        PORTC |= (1 << 2);
    }

    if(str[11] == 'c') {
        PORTC |= (1 << 3);
    }

    if(str[12] == 'c') {
        PORTC |= (1 << 4);
    }

    if(str[13] == 'c') {
        PORTC |= (1 << 5);
    }

    if(str[14] == 'c') {
        PORTD |= (1 << 3);
    }
}


void clock_ports(char* str) {
    //
    // uprising clock.
    //
    if(str[0] == 'C') {
        PORTB |= (1 << 0);
        nop();
        PORTB &= ~(1 << 0);
        nop();
    }
    if(str[1] == 'C') {
        PORTB |= (1 << 1);
        nop();
        PORTB &= ~(1 << 1);
        nop();
    }
    if(str[2] == 'C') {
        PORTB |= (1 << 2);
        nop();
        PORTB &= ~(1 << 2);
        nop();
    }
    if(str[3] == 'C') {
        PORTB |= (1 << 3);
        nop();
        PORTB &= ~(1 << 3);
        nop();
    }
    if(str[4] == 'C') {
        PORTB |= (1 << 4);
        nop();
        PORTB &= ~(1 << 4);
        nop();
    }
    if(str[5] == 'C') {
        PORTB |= (1 << 5);
        nop();
        PORTB &= ~(1 << 5);
        nop();
    }

    if(str[6] == 'C') {
        PORTD |= (1 << 2);
        nop();
        PORTD &= ~(1 << 2);
        nop();
    }

    if(str[8] == 'C') {
        PORTC |= (1 << 0);
        nop();
        PORTC &= ~(1 << 0);
        nop();
    }
    if(str[9] == 'C') {
        PORTC |= (1 << 1);
        nop();
        PORTC &= ~(1 << 1);
        nop();
    }
    if(str[10] == 'C') {
        PORTC |= (1 << 2);
        nop();
        PORTC &= ~(1 << 2);
        nop();

    }
    if(str[11] == 'C') {
        PORTC |= (1 << 3);
        nop();
        PORTC &= ~(1 << 3);
        nop();
    }
    if(str[12] == 'C') {
        PORTC |= (1 << 4);
        nop();
        PORTC &= ~(1 << 4);
        nop();
    }
    if(str[13] == 'C') {
        PORTC |= (1 << 5);
        nop();
        PORTC &= ~(1 << 5);
        nop();
    }

    if(str[14] == 'C') {
        PORTD |= (1 << 3);
        nop();
        PORTD &= ~(1 << 3);
        nop();
    }

    //
    // Down-going clock
    //
    if(str[0] == 'c') {
        PORTB &= ~(1 << 0);
        nop();

        PORTB |= (1 << 0);
        nop();
    }
    if(str[1] == 'c') {
        PORTB &= ~(1 << 1);
        nop();
        PORTB |= (1 << 1);
        nop();
    }
    if(str[2] == 'c') {
        PORTB &= ~(1 << 2);
        nop();
        PORTB |= (1 << 2);
        nop();
    }
    if(str[3] == 'c') {
        PORTB &= ~(1 << 3);
        nop();
        PORTB |= (1 << 3);
        nop();
    }
    if(str[4] == 'c') {
        PORTB &= ~(1 << 4);
        nop();
        PORTB |= (1 << 4);
        nop();
    }
    if(str[5] == 'c') {
        PORTB &= ~(1 << 5);
        nop();
        PORTB |= (1 << 5);
        nop();
    }

    if(str[6] == 'c') {
        PORTD &= ~(1 << 2);
        nop();
        PORTD |= (1 << 2);
        nop();
    }

    if(str[8] == 'c') {
        PORTC &= ~(1 << 0);
        nop();
        PORTC |= (1 << 0);
        nop();
    }
    if(str[9] == 'c') {
        PORTC &= ~(1 << 1);
        nop();
        PORTC |= (1 << 1);
        nop();
    }
    if(str[10] == 'c') {
        PORTC &= ~(1 << 2);
        nop();
        PORTC |= (1 << 2);
        nop();

    }
    if(str[11] == 'c') {
        PORTC &= ~(1 << 3);
        nop();
        PORTC |= (1 << 3);
        nop();
    }
    if(str[12] == 'c') {
        PORTC &= ~(1 << 4);
        nop();
        PORTC |= (1 << 4);
        nop();
    }
    if(str[13] == 'c') {
        PORTC &= ~(1 << 5);
        nop();
        PORTC |= (1 << 5);
        nop();
    }

    if(str[14] == 'c') {
        PORTD &= ~(1 << 3);
        nop();
        PORTD |= (1 << 3);
        nop();
    }
}

// NOTE(erick): Returns 0 on success and the char corresponding
//  the current state of the pin otherwise.
int8 check_pin(uint8 pin, char expected) {
    if(pin) {
        if(expected == 'H' || expected == 'V' || expected == 'X') {
            return 0;
        } else {
            return 'H';
        }
    } else {
        if(expected == 'L' || expected == 'G' || expected == 'X') {
            return 0;
        } else {
            return 'L';
        }
    }
}

void check_ports(char* str) {
    uint8 success = true;
    char temp[17] = "________________";
    char value = '_';

    if(!input_or_output(str[0])) {
        if((value = check_pin(PINB & (1 << 0), str[0]))) {
            temp[0] = value;
            success = false;
        }
    }
    if(!input_or_output(str[1])) {
        if((value = check_pin(PINB & (1 << 1), str[1]))) {
            temp[1] = value;
            success = false;
        }
    }
    if(!input_or_output(str[2])) {
        if((value = check_pin(PINB & (1 << 2), str[2]))) {
            temp[2] = value;
            success = false;
        }
    }
    if(!input_or_output(str[3])) {
        if((value = check_pin(PINB & (1 << 3), str[3]))) {
            temp[3] = value;
            success = false;
        }
    }
    if(!input_or_output(str[4])) {
        if((value = check_pin(PINB & (1 << 4), str[4]))) {
            temp[4] = value;
            success = false;
        }
    }
    if(!input_or_output(str[5])) {
        if((value = check_pin(PINB & (1 << 5), str[5]))) {
            temp[5] = value;
            success = false;
        }
    }

    if(!input_or_output(str[6])) {
        if((value = check_pin(PIND & (1 << 2), str[6]))) {
            temp[6] = value;
            success = false;
        }
    }

    if(!input_or_output(str[8])) {
        if((value = check_pin(PINC & (1 << 0), str[8]))) {
            temp[8] = value;
            success = false;
        }
    }
    if(!input_or_output(str[9])) {
        if((value = check_pin(PINC & (1 << 1), str[9]))) {
            temp[9] = value;
            success = false;
        }
    }
    if(!input_or_output(str[10])) {
        if((value = check_pin(PINC & (1 << 2), str[10]))) {
            temp[10] = value;
            success = false;
        }
    }
    if(!input_or_output(str[11])) {
        if((value = check_pin(PINC & (1 << 3), str[11]))) {
            temp[11] = value;
            success = false;
        }
    }
    if(!input_or_output(str[12])) {
        if((value = check_pin(PINC & (1 << 4), str[12]))) {
            temp[12] = value;
            success = false;
        }
    }
    if(!input_or_output(str[13])) {
        if((value = check_pin(PINC & (1 << 5), str[13]))) {
            temp[13] = value;
            success = false;
        }
    }

    if(!input_or_output(str[14])) {
        if((value = check_pin(PIND & (1 << 3), str[14]))) {
            temp[14] = value;
            success = false;
        }
    }

    if(success) {
        serial_write_terminated_string("Success");
        serial_break_line();
    } else {
        serial_write_terminated_string("Fail:    ");
        print_str(temp);
        serial_break_line();
    }

}

static int8 buffer[16];
void read_16_bytes(int8* to) {
    for(int i = 0; i < 16; i++) {
        to[i] = (int8) serial_read_byte_blocking();
    }
}

void loop() {
    serial_write_terminated_string("READY");
    serial_break_line();

    read_16_bytes(buffer);

    set_ports(buffer);
    unclock_ports(buffer);

    while(true) {
        serial_write_terminated_string("Testing: ");
        print_str(buffer);

        write_ports(buffer);
        clock_ports(buffer);
        check_ports(buffer);

        serial_write_terminated_string("READY");
        serial_break_line();
        read_16_bytes(buffer);
    }
}
