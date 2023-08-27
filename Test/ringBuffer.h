#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint8_t* buffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} RingBuffer_t;


RingBuffer_t* RingBuffer_create(size_t size);
void RingBuffer_free(RingBuffer_t* rb);

size_t RingBuffer_append(RingBuffer_t* rb, uint8_t* data, size_t len);
size_t RingBuffer_pop(RingBuffer_t* rb, uint8_t* data, size_t len);
size_t RingBuffer_flush(RingBuffer_t* rb, uint8_t* data);
 
#endif /* __RING_BUFFER_H__ */
