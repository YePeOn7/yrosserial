#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "serial.h"

static int fd;

int Serial_init(char* port, long long baudrate)
{
    const char *serial_port = port; // Replace with your serial port
    int bOption = 0;

    if(baudrate == 2400) bOption = B2400;
    else if(baudrate == 4800) bOption = B4800;
    else if(baudrate == 9600) bOption = B9600;
    else if(baudrate == 19200) bOption = B19200;
    else if(baudrate == 38400) bOption = B38400;
    else if(baudrate == 57600) bOption = B57600;
    else if(baudrate == 115200) bOption = B115200;
    else if(baudrate == 230400) bOption = B230400;
    else if(baudrate == 460800) bOption = B460800;
    else if(baudrate == 500000) bOption = B500000;
    else if(baudrate == 921600) bOption = B921600;
    else if(baudrate == 1000000) bOption = B1000000;
    else if(baudrate == 1152000) bOption = B1152000;
    else if(baudrate == 1500000) bOption = B1500000;
    else return -1; // baudrate is not supported

    fd = open(serial_port, O_RDWR | O_NOCTTY);

    if (fd == -1)
    {
        perror("Error opening serial port");
        return 1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, bOption);        // Set baud rate to 9600 bps
    cfsetospeed(&options, bOption);        // Set baud rate to 9600 bps
    options.c_cflag |= (CLOCAL | CREAD); // Enable receiver and ignore modem control lines
    options.c_cflag &= ~PARENB;          // Disable parity bit
    options.c_cflag &= ~CSTOPB;          // Use one stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;           // Set data bits to 8
    tcsetattr(fd, TCSANOW, &options); // Apply settings immediately

    return 0;
}

int serialWrite(uint8_t *data, size_t len)
{
    return write(fd, data, len);
}

ssize_t serialWriteStr(char* format, ...)
{
    char buffer[512];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return write(fd, buffer, strlen(format));
}

ssize_t serialRead(uint8_t *data, size_t len)
{
    return read(fd, data, len);
}