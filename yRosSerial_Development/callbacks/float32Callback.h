/*
 * float32Callback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef FLOAT32CALLBACK_H_
#define FLOAT32CALLBACK_H_

extern float f32;

void float32Callback(void *message)
{
	f32 = *((float*)message);
}

#endif /* FLOAT32CALLBACK_H_ */
