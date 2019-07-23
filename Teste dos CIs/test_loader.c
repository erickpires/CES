#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <termios.h>
#include <fcntl.h>

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

typedef struct dirent dir_ent;

typedef enum {
    NONE,
    ODD,
    EVEN
} parity_mode_t;

#define sizeof_array(array) (sizeof(array) / sizeof(array[0]))

uint select_speed(char* str) {
    uint speeed = atoi(str);

    switch(speeed) {
    case 50: return B50;
    case 75: return B75;
    case 110: return B110;
    case 134: return B134;
    case 150: return B150;
    case 200: return B200;
    case 300: return B300;
    case 600: return B600;
    case 1200: return B1200;
    case 1800: return B1800;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    case 460800: return B460800;
    case 500000: return B500000;
    case 576000: return B576000;
    case 921600: return B921600;
    case 1000000: return B1000000;
    case 1152000: return B1152000;
    case 1500000: return B1500000;
    case 2000000: return B2000000;
    case 2500000: return B2500000;
    case 3000000: return B3000000;
    case 3500000: return B3500000;
    case 4000000: return B4000000;
    default:
        fprintf(stderr, "Fail to found speed. Available speeds are:\n");
        fprintf(stderr, "\t50\n75\n110\n134\n150\n200\n300\n600\n1200\n"
                "1800\n2400\n4800\n9600\n19200\n38400\n57600\n115200\n"
                "230400\n460800\n500000\n576000\n921600\n1000000\n"
                "1152000\n1500000\n2000000\n2500000\n3000000\n"
                "3500000\n4000000");
        exit(1);
    }
}

uint select_number_of_bits(char c) {
    uint n_bits = c - '0';

    switch(n_bits) {
    case 5: return CS5;
    case 6: return CS6;
    case 7: return CS7;
    case 8: return CS8;
    default:
        fprintf(stderr, "Invalid number of bits. "
                "Available number of bits are: 5, 6, 7, 8.\n");
        exit(1);
     }
}

parity_mode_t select_parity(char c) {
    switch(c) {
    case 'N': return NONE;
    case 'O': return ODD;
    case 'E': return EVEN;
    default:
        fprintf(stderr, "Invalid parity. "
                "Available parities are: \"N - none\", "
                "\"O - odd\" and "
                "\"E - even\".\n");
        exit(1);
    }
}

uint select_stop_bits(char c) {
    uint stop_bits = c - '0';
    if(stop_bits != 1 && stop_bits != 2) {
        fprintf(stderr, "Invalid stop bits. Valid stop bits are: 1 or 2.");
        exit(1);
    }

    return stop_bits;
}

// TODO(erick): Reading one character at a time is VERY SLOW. We should buffer
//  the input.
usize read_until_linebreak(char* buf, usize max_len, int fd) {
    usize chars_read = 0;

    while(chars_read < max_len - 1) {
        read(fd, buf + chars_read, 1);
        char c = buf[chars_read];
        chars_read++;

        if(c == '\n') { break; }
    }

    buf[chars_read] = '\0';
    return chars_read;
}

int main(int args_count, char** args_values) {
    //
    // Parsing inputs
    //
    char* input_filename = "";
    char* speed_str = "9600";
    char* mode = "8N1";

    if(args_count == 1 || args_count > 4) {
        fprintf(stderr, "Usage: \n\t%s [speed [mode]] input_file\n", args_values[0]);
        exit(1);
    }

    if(args_count == 2) {
        input_filename = args_values[1];
    }

    if(args_count == 3) {
        speed_str = args_values[1];
        input_filename = args_values[2];
    }

    if(args_count == 4) {
        speed_str = args_values[1];
        mode = args_values[2];
        input_filename = args_values[3];
    }

    uint speed = select_speed(speed_str);
    uint n_bits = select_number_of_bits(mode[0]);
    parity_mode_t parity = select_parity(mode[1]);
    uint stop_bits = select_stop_bits(mode[2]);

    printf("Using: %s baud and %.3s mode.\n", speed_str, mode);

    //
    // Finding serial files
    //
    char* slash_dev = "/dev/";

    DIR* dev_dir = opendir(slash_dev);
    if(!dev_dir) {
        fprintf(stderr, "Could not open '/dev/' directory.\n");
        exit(1);
    }

    dir_ent* current_file;
    char* serial_ports[128] = {};
    usize serial_ports_count = 0;
    usize selected_port;

    char* tty_usb_basename = "ttyUSB";
    char* tty_acm_basename = "ttyACM";

    while((current_file = readdir(dev_dir))) {
        char* current_filename = current_file->d_name;

        if(strcmp(current_filename, ".") == 0 ||
           strcmp(current_filename, "..") == 0) {
            // Ignoring.
            continue;
        }

        if(strncmp(current_filename, tty_usb_basename, strlen(tty_usb_basename)) == 0 ||
           strncmp(current_filename, tty_acm_basename, strlen(tty_acm_basename)) == 0) {
            serial_ports[serial_ports_count] = (char*) malloc(strlen(slash_dev) +
                                                              strlen(current_filename) +
                                                              1);
            strcpy(serial_ports[serial_ports_count], slash_dev);
            strcat(serial_ports[serial_ports_count], current_filename);
            serial_ports_count++;
        }
    }

    if(serial_ports_count == 0) {
        fprintf(stderr, "Could not found any viable serial port.\n");
        exit(1);
    }

    if(serial_ports_count == 1) {
        printf("Found: %s.\n", serial_ports[0]);
        printf("Press any key to use it (or Crtl-C to exit)\n");
        getchar();

        selected_port = 0;
    } else {
        printf("Found the following serial ports:\n");
        for(usize i = 0; i < serial_ports_count; i++) {
            printf("[%ld]: %s\n", i + 1, serial_ports[i]);
        }
        printf("Please select one: ");

        uint _selected;
        if(!scanf("%d", &_selected)) {
            fprintf(stderr, "Could not parse a number.\n");
            exit(2);
        }

        if(_selected > serial_ports_count) {
            fprintf(stderr, "Number out-of-range!\n");
            exit(2);
        }

        selected_port = _selected - 1;

        printf("Selected: %s\n", serial_ports[selected_port]);
        printf("Press any key to continue (or Crtl-C to exit).\n");
        getchar();
    }

    //
    // Setting attributes
    //
    int serial_fd = open(serial_ports[selected_port], O_RDWR | O_NOCTTY | O_SYNC);
    if(serial_fd < 0) {
        fprintf(stderr, "Could not open file: %s\n", serial_ports[selected_port]);
        exit(3);
    }

    struct termios attributes;
    if(tcgetattr(serial_fd, &attributes) < 0) {
        fprintf(stderr, "Could not get attributes for: %s\n",
                serial_ports[selected_port]);
        exit(3);
    }

    cfsetispeed(&attributes, (speed_t)speed);
    cfsetospeed(&attributes, (speed_t)speed);

    attributes.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */

    attributes.c_cflag &= ~CSIZE;              /* Zeroing the word size */
    attributes.c_cflag |= n_bits;

    switch(parity) {
    case NONE :
        attributes.c_cflag &= ~(PARENB);
        break;
    case ODD :
        attributes.c_cflag |= PARENB | PARODD;
        break;
    case EVEN :
        attributes.c_cflag |= PARENB;
        break;
    }

    if(stop_bits == 2) {
        attributes.c_cflag |= CSTOPB;
    } else {
        attributes.c_cflag &= ~(CSTOPB);
    }

#ifdef __USE_MISC
    attributes.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */
#endif

    /* setup for non-canonical mode */
    attributes.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                            INLCR | IGNCR | ICRNL | IXON);
    // no signaling chars, no echo,
    // no canonical processing
    attributes.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    attributes.c_oflag &= ~OPOST; // no remapping, no delays

    /* fetch bytes as they become available */
    attributes.c_cc[VMIN] = 1;  // blocking
    attributes.c_cc[VTIME] = 0; // 0.5 seconds read timeout

    if(tcsetattr(serial_fd, TCSANOW, &attributes)) {
        fprintf(stderr, "Could not get attributes for: %s\n",
                serial_ports[selected_port]);
        exit(3);
    }

    // NOTE(erick): Flushing any data on the serial I/O buffer.
    tcflush(serial_fd, TCIOFLUSH);

    char input_buffer[256] = {};
    bool is_tester_ready = false;

    do {
        read_until_linebreak(input_buffer, sizeof_array(input_buffer), serial_fd);
        is_tester_ready = (strcmp("READY\n", input_buffer)  == 0);
    } while(!is_tester_ready);

    //
    // Opening input file
    //
    FILE* input_file = fopen(input_filename, "r");
    if(!input_file) {
        fprintf(stderr, "Could not open: %s.\n", input_filename);
        exit(4);
    }

    char buffer[256];
    while(!feof(input_file) && fgets(buffer, 256, input_file)) {
        usize len = strlen(buffer);
        if(buffer[len - 1] == '\n') { buffer[--len] = '\0'; }

        if(len != 16) {
            fprintf(stderr, "Lines must contain exactly 16 characters [%s].\n",
                    buffer);
            exit(5);
        }

        write(serial_fd, buffer, len);
        //        printf("Wrote: %s\n", buffer);

        while(true) {
            read_until_linebreak(input_buffer, sizeof_array(input_buffer), serial_fd);
            is_tester_ready = strcmp("READY\n", input_buffer) == 0;
            if(is_tester_ready) { break; }

            printf("%s", input_buffer);
        }
    }

    return 0;
}
