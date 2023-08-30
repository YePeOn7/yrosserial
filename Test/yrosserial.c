#include <string.h>

#include "yrosserial.h"

#define MAX_MESSAGE_SIZE    128 // maximum message size (Should not be more than buffer size)

static RingBuffer_t *rx;
static RingBuffer_t *tx;
static yRosSerial_setting_t setting = {0};
static uint8_t initialized;

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

//------------------ Interface Implementation --------------- //

void yRosSerial_init(yRosSerial_setting_t* _setting)
{
    rx = RingBuffer_create(_setting->rxBufSize);
    tx = RingBuffer_create(_setting->txBufSize);
    memcpy(&setting, _setting, sizeof(yRosSerial_setting_t));

    initialized = 1;

    Serial_init("/dev/pts/2", 9600);
}

void yRosSerial_receive()
{
    if(initialized)
    {
        size_t bytesRead;
        uint8_t rData[setting.rxBufSize];
        memset(rData,0, sizeof(rData));

        if ((bytesRead = serialRead(rData, sizeof(rData))) > 0)
        {
            RingBuffer_append(rx, rData, bytesRead);

            for (int i = 0; i < rx->count; i++)
            {
                printf("%d ", rx->buffer[rx->tail + i]);
            }
            printf("\n");
            fflush(stdout);
        }
    }
}

// void yRosSerial_handleCompleteReceive(UART_HANDLE uartHandle)
// {

// }

void yRosSerial_spin()
{
    rParser(rx);
}
