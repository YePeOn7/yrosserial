/*
 * s16Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S16CALLBACK_H_
#define S16CALLBACK_H_

extern int16_t subS16;

void s16Callback(void *message)
{
	subS16 = *((int16_t*)message);
}

#endif /* S16CALLBACK_H_ */
