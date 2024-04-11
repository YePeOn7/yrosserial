/*
 * rb2.h
 *
 *  Created on: Apr 11, 2024
 *      Author: yp7
 */

#ifndef RB2_H_
#define RB2_H_

#include "main.h"
#include <string.h>

typedef struct{
	uint8_t *buffer;
	size_t tail;
	size_t size;
	DMA_HandleTypeDef* hdma;
	UART_HandleTypeDef *huart;
}Rb2_t;

size_t rb2_getAvailable(Rb2_t* rb);
size_t rb2_getHead(Rb2_t *rb);
size_t rb2_pop(Rb2_t *rb, uint8_t *out, size_t len);

#endif /* RB2_H_ */
