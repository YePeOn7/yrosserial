#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "ringBuffer.h"
#include "serial.h"

#define MAX_MESSAGE_SIZE    128 // maximum message size (Should be not more than buffer size)

int validateChecksum(uint8_t* message, size_t len)
{
    uint8_t sum = len;
    for(int i = 0; i < len -1; i++) sum += message[i];

    if(sum == message[len-1]) printf("Checksum is correct\n");
    else printf("Checksum get: %d, expected: %d\n", message[len-1], sum);

    return (sum == message[len-1]);
}

void responseTopic()
{
    printf("Receive request topic\n");
}

int processIncomingMessage(uint8_t* message, size_t len)
{
    enum
    {
        NO_INSTRUCTION,
        REQUESTING_TOPIC,
        RESPONSE_TOPIC,
        REQUEST_SYNC,
        RESPONSE_SYNC
    };

    // instruction
    if(message[0] < 10)
    {
        uint8_t instruction = message[0];
        if(validateChecksum(message, len))
        {
            printf("instruction: %d\n", instruction);
            switch(instruction)
            {
                case REQUESTING_TOPIC: responseTopic(); break;
            }
        }
        else return -1;
    }
    
    // Topic Id
    else
    {
        // uint8_t topicId = message[0];
    }

    return 0;
}

int rParser(RingBuffer_t *rb)
{
    enum
    {
        GET_HEADER1,
        GET_HEADER2,
        GET_LENGTH,
        GET_MESSAGE
    };

    static int state = GET_HEADER1;
    static uint8_t bufferMessage[MAX_MESSAGE_SIZE - 3] = {0}; // It will contain all packet excluding teh header 1, header 2, and length
    static size_t len = 0;
    uint8_t breakFor = 0;

    while(rb->count > 0)
    {
        uint8_t data;
        // printf("check rb->count: %d\n", rb->count);
        if(state!= GET_MESSAGE) RingBuffer_pop(rb, &data, 1); 

        switch (state)
        {
        case GET_HEADER1:
            printf("Checking Header 1\n");
            if(data == 5) state = GET_HEADER2;
            break;
        case GET_HEADER2:
            printf("Checking Header 2\n");
            if(data == 9) state = GET_LENGTH;
            else if(data == 5);         // keep the current state
            else state = GET_HEADER1;   // reset the state 
            break;
        case GET_LENGTH:
            printf("Getting length\n");
            len = data;
            state = GET_MESSAGE;
            break;
        case GET_MESSAGE:
            printf("Getting Message\n");
            if(rb->count >= len) 
            {
                RingBuffer_pop(rb, bufferMessage, len);
                processIncomingMessage(bufferMessage, len);
                state = GET_HEADER1;
                len = 0;
            }
            else 
            {
                printf("Break! rb count: %d\n", rb->count);
                breakFor = 1;
            }
            break;
        }
        
        if(breakFor) break;
    }
    return 0;
}

void *threadReadDataLoop(void *args)
{
    RingBuffer_t *rb = (RingBuffer_t *)args;
    while (1)
    {
        rParser(rb);
        // printf("%d\n", rb->count);
        sleep(5);
    }

    return NULL;
}

int main()
{
    // uint8_t tData[10] = {0};
    // uint8_t tData2[10] = {0};
    pthread_t threadParse;

    // RingBuffer_t* t = RingBuffer_create(10);

    // for(int i = 0; i < 5; i++) tData[i] = i;
    // RingBuffer_append(t, tData, 5);

    // RingBuffer_pop(t, tData2, 5);

    // for(int i = 0; i < 10; i++) tData[i] = i+5;
    // RingBuffer_append(t, tData, 10);

    // RingBuffer_flush(t, tData2);

    // for(int i = 0; i < 10; i++) tData[i] = i+15;
    // RingBuffer_append(t, tData, 10);

    // RingBuffer_flush(t, tData2);

    //------------ Testing Serial --------------//
    RingBuffer_t *r = RingBuffer_create(512);
    uint8_t rData[512] = {0};
    Serial_init("/dev/pts/2", 9600);

    // for(int i = 0; i < 10; i++) serialWriteStr("Hi, My name is Yohan: %d\n\r", i);

    // sprintf((char*)rData, "Hi, I am Yohan\n");
    // serialWrite(rData, strlen((char*)rData));

    pthread_create(&threadParse, NULL, threadReadDataLoop, r);
    size_t bytesRead = 0;
    while (1)
    {
        if ((bytesRead = serialRead(rData, sizeof(rData))) > 0)
        {
            RingBuffer_append(r, rData, bytesRead);

            for (int i = 0; i < r->count; i++)
            {
                printf("%d ", r->buffer[r->tail + i]);
            }
            printf("\n");
            fflush(stdout);
        }

        usleep(1000);
    }
    return 0;
}