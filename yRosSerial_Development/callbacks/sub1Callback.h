/*
 * cb1.h
 *
 *  Created on: Apr 9, 2024
 *      Author: yp7
 */

#ifndef SUB1CALLBACK_H_
#define SUB1CALLBACK_H_
#include <stdio.h>

void sub1(void *message)
{
	printf("%s\n", (uint8_t*)message);
}

#endif /* SUB1CALLBACK_H_ */
