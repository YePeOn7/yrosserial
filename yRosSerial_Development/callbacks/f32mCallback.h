/*
 * s32mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef F32MCALLBACK_H_
#define F32MCALLBACK_H_

extern float subF32M[L_ARRAY];

void f32mCallback(void *message)
{
	size_t l = ((yRosSerial_float32mul_t*)message)->length;
	memcpy(subF32M, ((yRosSerial_float32mul_t*)message)->data, l*sizeof(float));
}

#endif /* F32MCALLBACK_H_ */
