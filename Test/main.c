#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "ringBuffer.h"

int main()
{
    uint8_t tData[10] = {0};
    uint8_t tData2[10] = {0};

    RingBuffer_t* t = RingBuffer_create(10);

    for(int i = 0; i < 5; i++) tData[i] = i;
    RingBuffer_append(t, tData, 5);

    RingBuffer_pop(t, tData2, 5);

    for(int i = 0; i < 10; i++) tData[i] = i+5;
    RingBuffer_append(t, tData, 10);

    RingBuffer_flush(t, tData2);

    for(int i = 0; i < 10; i++) tData[i] = i+15;
    RingBuffer_append(t, tData, 10);

    return 0;
}