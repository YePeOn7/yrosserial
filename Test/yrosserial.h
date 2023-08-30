#ifndef __YROSSERIAL_H__
#define __YROSSERIAL_H__

#include <stdint.h>
#include <stdio.h>

#include "ringBuffer.h"
#include "serial.h"

typedef struct
{
    size_t txBufSize;
    size_t rxBufSize;
} yRosSerial_setting_t;

void yRosSerial_init();
void yRosSerial_spin();
void yRosSerial_receive();

#endif /* __YROSSERIAL_H__ */
