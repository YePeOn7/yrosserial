/*
 * cb1.h
 *
 *  Created on: Apr 9, 2024
 *      Author: yp7
 */

#ifndef STRINGCALLBACK_H_
#define STRINGCALLBACK_H_
#include <stdio.h>

void stringCallback(void *message)
{
	printf("%s\n", (uint8_t*)message);
}

#endif /* STRINGCALLBACK_H_ */
