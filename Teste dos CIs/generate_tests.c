#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef intptr_t isize;
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef size_t       usize;
typedef uint8_t      uint8;
typedef uint16_t     uint16;
typedef uint32_t     uint32;
typedef uint64_t     uint64;
typedef unsigned int uint;

void output_outpin(FILE* file, int value) {
    if(value) {
        fprintf(file, "H");
    } else {
        fprintf(file, "L");
    }
}

void output_inpin(FILE* file, int value) {
    if(value) {
        fprintf(file, "1");
    } else {
        fprintf(file, "0");
    }
}

void output_adder(FILE* file, uint8 a, uint8 b, uint8 carry_in, uint8 result) {
    output_outpin(file, result & 0x02);
    output_inpin(file, b & 0x02);
    output_inpin(file, a & 0x02);

    output_outpin(file, result & 0x01);
    output_inpin(file, a & 0x01);
    output_inpin(file, b & 0x01);

    output_inpin(file, carry_in);
    fprintf(file, "G");
    output_outpin(file, result & 0x10);

    output_outpin(file, result & 0x08);
    output_inpin(file, b & 0x08);
    output_inpin(file, a & 0x08);

    output_outpin(file, result & 0x04);
    output_inpin(file, a & 0x04);
    output_inpin(file, b & 0x04);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nand2(FILE* file, uint8 a, uint8 b) {
    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, !(a && b));

    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, !(a && b));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !(a && b));
    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, !(a && b));
    output_inpin(file, a);
    output_inpin(file, b);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nor2(FILE* file, uint8 a, uint8 b) {
    output_outpin(file, !(a || b));
    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, !(a || b));
    output_inpin(file, a);
    output_inpin(file, b);

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, !(a || b));

    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, !(a || b));

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_not(FILE* file, uint8 a) {
    output_inpin(file, a);
    output_outpin(file, !a);

    output_inpin(file, a);
    output_outpin(file, !a);

    output_inpin(file, a);
    output_outpin(file, !a);

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !a);
    output_inpin(file, a);

    output_outpin(file, !a);
    output_inpin(file, a);

    output_outpin(file, !a);
    output_inpin(file, a);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_and2(FILE* file, uint8 a, uint8 b) {
    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, (a && b));

    output_inpin(file, a);
    output_inpin(file, b);
    output_outpin(file, (a && b));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, (a && b));
    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, (a && b));
    output_inpin(file, a);
    output_inpin(file, b);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nand3(FILE* file, uint8 a, uint8 b, uint8 c) {
    output_inpin(file, a);
    output_inpin(file, b);

    output_inpin(file, a);
    output_inpin(file, b);
    output_inpin(file, c);
    output_outpin(file, !((a && b) && c));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !((a && b) && c));
    output_inpin(file, a);
    output_inpin(file, b);
    output_inpin(file, c);

    output_outpin(file, !((a && b) && c));
    output_inpin(file, c);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nand4(FILE* file, uint8 a, uint8 b, uint8 c, uint8 d) {
    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, 0); // Non-Connected

    output_inpin(file, c);
    output_inpin(file, d);

    output_outpin(file, !((a && b) && (c && d)));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !((a && b) && (c && d)));

    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, 0); // Non-Connected

    output_inpin(file, c);
    output_inpin(file, d);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_and4(FILE* file, uint8 a, uint8 b, uint8 c, uint8 d) {
    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, 0); // Non-Connected

    output_inpin(file, c);
    output_inpin(file, d);

    output_outpin(file, ((a && b) && (c && d)));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, ((a && b) && (c && d)));

    output_inpin(file, a);
    output_inpin(file, b);

    output_outpin(file, 0); // Non-Connected

    output_inpin(file, c);
    output_inpin(file, d);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nor3(FILE* file, uint8 a, uint8 b, uint8 c) {
    output_inpin(file, a);
    output_inpin(file, b);

    output_inpin(file, a);
    output_inpin(file, b);
    output_inpin(file, c);
    output_outpin(file, !((a || b) || c));

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !((a || b) || c));
    output_inpin(file, a);
    output_inpin(file, b);
    output_inpin(file, c);

    output_outpin(file, !((a || b) || c));
    output_inpin(file, c);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_nand8(FILE* file, uint8 value) {
    output_inpin(file, value & 0x01);
    output_inpin(file, value & 0x02);
    output_inpin(file, value & 0x04);
    output_inpin(file, value & 0x08);

    output_inpin(file, value & 0x10);
    output_inpin(file, value & 0x20);

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, value != 0xff);

    output_outpin(file, 0); // Non-Connected
    output_outpin(file, 0); // Non-Connected

    output_inpin(file, value & 0x40);
    output_inpin(file, value & 0x80);

    output_outpin(file, 0); // Non-Connected


}

void output_bin_counter(FILE* file, int expected, int clear) {
    output_outpin(file, expected & 0x800); // L
    output_outpin(file, expected & 0x020); // F
    output_outpin(file, expected & 0x010); // E
    output_outpin(file, expected & 0x040); // G
    output_outpin(file, expected & 0x008); // D
    output_outpin(file, expected & 0x004); // C
    output_outpin(file, expected & 0x002); // B

    fprintf(file, "G");

    output_outpin(file, expected & 0x001); // A

    fprintf(file, "c");

    output_inpin(file, clear);

    output_outpin(file, expected & 0x100); // I
    output_outpin(file, expected & 0x080); // H
    output_outpin(file, expected & 0x200); // J
    output_outpin(file, expected & 0x400); // K

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_decoder(FILE* file, uint8 value) {
    uint8 decoded = 1 << value;
    uint8 decoded_inv = ~decoded;
    output_inpin(file, value & 0x01);
    output_inpin(file, value & 0x02);
    output_inpin(file, value & 0x04);

    output_inpin(file, 0);
    output_inpin(file, 0);
    output_inpin(file, 1);

    output_outpin(file, decoded_inv & 0x80);

    fprintf(file, "G");

    output_outpin(file, decoded_inv & 0x40);
    output_outpin(file, decoded_inv & 0x20);
    output_outpin(file, decoded_inv & 0x10);
    output_outpin(file, decoded_inv & 0x08);
    output_outpin(file, decoded_inv & 0x03);
    output_outpin(file, decoded_inv & 0x02);
    output_outpin(file, decoded_inv & 0x01);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_multiplexer(FILE* file, uint8 a, uint8 b, uint8 select, uint8 result) {
    output_inpin(file, select);

    output_inpin(file, a & 0x01);
    output_inpin(file, b & 0x01);
    output_outpin(file, result & 0x01);

    output_inpin(file, a & 0x02);
    output_inpin(file, b & 0x02);
    output_outpin(file, result & 0x02);

    fprintf(file, "G");

    output_outpin(file, result & 0x04);
    output_inpin(file, b & 0x04);
    output_inpin(file, a & 0x04);

    output_outpin(file, result & 0x08);
    output_inpin(file, b & 0x08);
    output_inpin(file, a & 0x08);

    output_inpin(file, 0);
    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_dual_ff(FILE* file, uint8 clear, uint8 preset, uint8 d, uint q) {
    output_inpin(file, clear);
    output_inpin(file, d);

    fprintf(file, "C");

    output_inpin(file, preset);
    output_outpin(file, q);
    output_outpin(file, !q);

    fprintf(file, "G");
    fprintf(file, "G");
    fprintf(file, "G");

    output_outpin(file, !q);
    output_outpin(file, q);
    output_inpin(file, preset);

    fprintf(file, "C");

    output_inpin(file, d);
    output_inpin(file, clear);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_quad_ff(FILE* file, uint8 clear, uint8 d, uint8 q) {
    output_inpin(file, clear);

    output_outpin(file, q & 0x01);
    output_outpin(file, !(q & 0x01));
    output_inpin(file, d & 0x01);

    output_inpin(file, d & 0x02);
    output_outpin(file, !(q & 0x02));
    output_outpin(file, q & 0x02);

    fprintf(file, "G");
    fprintf(file, "C");

    output_outpin(file, q & 0x04);
    output_outpin(file, !(q & 0x04));
    output_inpin(file, d & 0x04);

    output_inpin(file, d & 0x08);
    output_outpin(file, !(q & 0x08));
    output_outpin(file, q & 0x08);

    fprintf(file, "V");
    fprintf(file, "\n");
}

void output_quad_register(FILE* file, uint8 clear, uint8 d, uint8 q) {
    output_inpin(file, 0);
    output_inpin(file, 0);

    output_outpin(file, q & 0x01);
    output_outpin(file, q & 0x02);
    output_outpin(file, q & 0x04);
    output_outpin(file, q & 0x08);

    fprintf(file, "C");
    fprintf(file, "G");

    output_inpin(file, 0);
    output_inpin(file, 0);

    output_inpin(file, q & 0x08);
    output_inpin(file, q & 0x04);
    output_inpin(file, q & 0x02);
    output_inpin(file, q & 0x01);

    output_inpin(file, clear);

    fprintf(file, "V");
    fprintf(file, "\n");
}

int main(int args_count, char** args_values) {
    FILE* file;

    file = fopen("7400.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            output_nand2(file, a, b);
        }
    }
    fclose(file);

    file = fopen("7402.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            output_nor2(file, a, b);
        }
    }
    fclose(file);

    file = fopen("7404.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        output_not(file, a);
    }
    fclose(file);

    file = fopen("7408.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            output_and2(file, a, b);
        }
    }
    fclose(file);

    file = fopen("7410.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            for(uint8 c = 0; c <= 1; c++) {
                output_nand3(file, a, b, c);
            }
        }
    }
    fclose(file);

    file = fopen("7414.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        output_not(file, a);
    }
    fclose(file);

    file = fopen("7420.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            for(uint8 c = 0; c <= 1; c++) {
                for(uint8 d = 0; c <= 1; c++) {
                    output_nand4(file, a, b, c, d);
                }
            }
        }
    }
    fclose(file);

    file = fopen("7421.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            for(uint8 c = 0; c <= 1; c++) {
                for(uint8 d = 0; c <= 1; c++) {
                    output_and4(file, a, b, c, d);
                }
            }
        }
    }
    fclose(file);

    file = fopen("7427.txt", "w");
    for(uint8 a = 0; a <= 1; a++) {
        for(uint8 b = 0; b <= 1; b++) {
            for(uint8 c = 0; c <= 1; c++) {
                output_nor3(file, a, b, c);
            }
        }
    }
    fclose(file);

    file = fopen("7430.txt", "w");
    {
        uint8 value = 0;
        do {
            output_nand8(file, value);
        } while(++value != 0);
    }
    fclose(file);

    file = fopen("7474.txt", "w");
    output_dual_ff(file, 0, 1, 1, 0);
    output_dual_ff(file, 1, 0, 0, 1);
    output_dual_ff(file, 1, 1, 0, 0);
    output_dual_ff(file, 1, 1, 1, 1);
    fclose(file);

    file = fopen("74112.txt", "w");
    // TODO(erick)
    fclose(file);

    file = fopen("74138.txt", "w");
    for(uint8 i = 0; i < 8; i++) {
        output_decoder(file, i);
    }
    fclose(file);

    file = fopen("74173.txt", "w");
    output_quad_register(file, 0, 0x0f, 0);
    for(uint8 i = 0; i < 16; i++) {
        output_quad_register(file, 0, i, i);
    }
    fclose(file);

    file = fopen("74175.txt", "w");
    output_quad_ff(file, 0, 0x0f, 0);
    for(uint8 i = 0; i < 16; i++) {
        output_quad_ff(file, 1, i, i);
    }
    fclose(file);

    file = fopen("74257.txt", "w");
    for(uint8 a = 0; a < 16; a++) {
        output_multiplexer(file, a, 0x00, 0, a);
    }
    for(uint8 a = 0; a < 16; a++) {
        output_multiplexer(file, a, 0x0f, 0, a);
    }

    for(uint8 b = 0; b < 16; b++) {
        output_multiplexer(file, 0x00, b, 1, b);
    }
    for(uint8 b = 0; b < 16; b++) {
        output_multiplexer(file, 0x0f, b, 1, b);
    }
    fclose(file);

    file = fopen("74283.txt", "w");
    for(uint8 a = 0; a < 16; a++) {
        for(uint8 b = 0; b < 16; b++) {
            output_adder(file, a, b, 0, a + b);
            output_adder(file, a, b, 1, a + b + 1);
        }
    }
    fclose(file);

    file = fopen("744040.txt", "w");
    output_bin_counter(file, 0, 1);
    for(int i = 0; i < (1 << 12); i++) {
        output_bin_counter(file, i, 0);
    }
    fclose(file);

    return 0;
}
