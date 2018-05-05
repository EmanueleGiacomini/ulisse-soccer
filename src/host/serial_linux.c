#include "serial_linux.h"
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//! sets the attributes
int serial_set_interface_attribs (int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof(tty));   //initialize tty struct
    // Carico parametri associati a fd in tty
    if(tcgetattr(fd, &tty) != 0) {
        printf("error %d from tcgetattr", errno);
        return -1;
    }
    // Check su baudrate
    switch(speed) {
        case 57600:
            speed=B57600;
            break;
        case 115200:
            speed=B115200;
            break;
        default:
            printf("cannot set baudrate %d\n", speed);
            return -1;
    }
    cfsetospeed(&tty, speed);   // set output speed
    cfsetispeed(&tty, speed);   // set input  speed
    // sets the terminal to "raw" mode (look at man)
    cfmakeraw(&tty);
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shuft off parity
    tty.c_cflag |= parity;
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars

    if(tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

//! puts the port in blocking/nonblocking mode
void serial_set_blocking (int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof(tty));   //initialize tty struct
    // Carico parametri associati a fd in tty
    if(tcgetattr(fd, &tty) != 0) {
        printf("error %d from tcgetattr", errno);
        return;
    }
    tty.c_cc[VMIN] = should_block?1:0;  // ?
    tty.c_cc[VTIME] = 5;                // 0.5 seconds read timeout

    if(tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("error %d from tcsetattr", errno);
    }
}

//! returns the descriptor of a serial port
int serial_open(const char* name) {
    int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf ("error %d opening serial, fd %d\n", errno, fd);
    }
    return fd;
}


  
