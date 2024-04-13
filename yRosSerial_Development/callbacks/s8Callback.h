/*
 * s8Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S8CALLBACK_H_
#define S8CALLBACK_H_

extern int8_t subS8;

void s8Callback(void *message)
{
	subS8 = *((int8_t*)message);
}

#endif /* S8CALLBACK_H_ */
