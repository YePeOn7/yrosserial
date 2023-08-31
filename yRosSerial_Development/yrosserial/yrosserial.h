#ifndef __YROSSERIAL_H__
#define __YROSSERIAL_H__

#include <stdint.h>
#include <stdio.h>

#include "ringBuffer.h"
//#include "serial.h"
#include "main.h" // from the project

typedef struct
{
    size_t txBufSize;
    size_t rxBufSize;
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma_rx;
} yRosSerial_setting_t;

void yRosSerial_init(yRosSerial_setting_t* _setting);
void yRosSerial_spin();

void yRosSerial_getRxBuffer(uint8_t *buffer, size_t len);
void yRosSerial_handleCompleteReceive(UART_HandleTypeDef* huart, uint16_t size);
void check();

#endif /* __YROSSERIAL_H__ */
