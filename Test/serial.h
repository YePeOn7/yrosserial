#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "serial.h"

int Serial_init(char* port, long long baudrate);
int serialWrite(uint8_t *data, size_t len);
ssize_t serialWriteStr(char* format, ...);
ssize_t serialRead(uint8_t *data, size_t len);

#endif /* __SERIAL_H__ */
