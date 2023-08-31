#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <stdlib.h>

int serialWrite(uint8_t *data, size_t len);
ssize_t serialWriteStr(char* format, ...);
ssize_t serialRead(uint8_t *data, size_t len);

#endif /* __SERIAL_H__ */
