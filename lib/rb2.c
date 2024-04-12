/*
 * rb.c
 *
 *  Created on: Apr 11, 2024
 *      Author: yp7
 */


#include "rb2.h"
#include <string.h>

size_t rb2_getHead(Rb2_t* rb)
{
	return (rb->size - rb->hdma->Instance->NDTR);
}

size_t rb2_getAvailable(Rb2_t* rb)
{
	int32_t s = (int32_t)rb2_getHead(rb) - (int32_t)rb->tail;
	if(s < 0) s += rb->size;

	return s;
}

size_t rb2_pop(Rb2_t *rb, uint8_t *out, size_t len)
{
	size_t a = rb2_getAvailable(rb);
	uint8_t *b = rb->buffer;
	size_t t = rb->tail;
	size_t s = rb->size;

	size_t valToPop = a > len? len: a;
	size_t toEnd = s - t;

	if(valToPop > 0)
	{
		if(toEnd > valToPop)
		{
			memcpy(out, b+t, valToPop);
		}
		else
		{
			memcpy(out, b+t, toEnd);
			memcpy(out+toEnd, b, valToPop-toEnd);
		}

		rb->tail = (t + valToPop) % s;
		rb->count = rb2_getAvailable(rb);
	}

	return valToPop;
}
