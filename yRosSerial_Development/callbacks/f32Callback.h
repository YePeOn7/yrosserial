/*
 * float32Callback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef F32CALLBACK_H_
#define F32CALLBACK_H_

extern float f32;

void f32Callback(void *message)
{
	f32 = *((float*)message);
}

#endif /* F32CALLBACK_H_ */
