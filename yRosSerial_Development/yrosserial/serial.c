#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "serial.h"

static int fd;

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
