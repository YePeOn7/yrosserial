#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "ringBuffer.h"
#include "serial.h"

void* threadReadDataLoop()
{
    while(1)
    {
        sleep(1);
    }

    return NULL;
}

int rParser(uint8_t data, size_t len)
{
    return 0;
}

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

    RingBuffer_flush(t, tData2);

    //------------ Testing Serial --------------//
    RingBuffer_t* r = RingBuffer_create(512);
    uint8_t rData[512] = {0};
    Serial_init("/dev/pts/5", 9600);
    
    for(int i = 0; i < 10; i++) serialWriteStr("Hi, My name is Yohan: %d\n\r", i);

    sprintf((char*)rData, "Hi, I am Yohan\n");
    serialWrite(rData, strlen((char*)rData));
    size_t bytesRead = 0;
    while(1)
    {
        if((bytesRead = serialRead(rData, sizeof(rData))) > 0)
        {
            RingBuffer_append(r, rData, bytesRead);

            for(int i = 0; i < r->count; i++)
            {
                printf("%d ", r->buffer[r->tail+i]);
            }
            printf("\n");
        }
        fflush(stdout);
    }
    return 0;
}