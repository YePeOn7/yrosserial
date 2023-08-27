#include <stdlib.h>
#include <string.h>
#include "ringBuffer.h"
#define RING_BUFFER_LOG_ENABLE  1

#if RING_BUFFER_LOG_ENABLE == 1
#include <stdarg.h>
#endif

static void logPrint(char* format, ...)
{
#if RING_BUFFER_LOG_ENABLE
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

RingBuffer_t* RingBuffer_create(size_t size)
{
    RingBuffer_t* rb = (RingBuffer_t*) malloc(sizeof(RingBuffer_t));
    rb->buffer = (uint8_t*) malloc(size);
    rb->size = size;
    rb->count = 0;
    rb->tail = 0;
    rb->head = 0;

    logPrint("Created a Ring Buffer with %ld bytes of size\n", size);

    return rb;
}

void RingBuffer_free(RingBuffer_t* rb)
{
    if(rb)
    {
        free(rb->buffer);
        free(rb);
        rb = NULL;
    }
}

size_t RingBuffer_append(RingBuffer_t* rb, uint8_t* data, size_t len)
{
    if(rb->size - rb->count >= len)
    {
        size_t toEnd = rb->size - rb->head;
        if(toEnd >= len) memcpy(&rb->buffer[rb->head], data, len);
        else
        {
            memcpy(&rb->buffer[rb->head], data, (size_t)toEnd);
            memcpy(&rb->buffer[0], &data[toEnd], (size_t)(len - toEnd));
        }
        rb->count += len;
        rb->head = (rb->head + len) % rb->size;
        return len;
    }
    
    return 0;
}

size_t RingBuffer_pop(RingBuffer_t* rb, uint8_t* data, size_t len)
{
    size_t toEnd = rb->size - rb->tail;
    size_t valToPop = rb->count >= len? len : (size_t)rb->count;

    if(rb->count > 0)
    {
        if(toEnd >= valToPop) memcpy(data, &rb->buffer[rb->tail], valToPop);
        else
        {
            memcpy(&data[0], &rb->buffer[rb->tail], toEnd);
            memcpy(&data[toEnd], &rb->buffer[0], valToPop - toEnd);
        }
        
        rb->count -= valToPop;
        rb->tail = (rb->tail + valToPop) % rb->size;

        return valToPop;
    }

    return 0;
}

size_t RingBuffer_flush(RingBuffer_t* rb, uint8_t* data)
{
    return RingBuffer_pop(rb, data, rb->count);
}