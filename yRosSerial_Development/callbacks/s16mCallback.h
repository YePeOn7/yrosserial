/*
 * s16mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S16MCALLBACK_H_
#define S16MCALLBACK_H_

extern int16_t subS16M[L_ARRAY];

void s16mCallback(void *message)
{
	size_t l = ((yRosSerial_int16mul_t*)message)->length;
	memcpy(subS16M, ((yRosSerial_int16mul_t*)message)->data, l*sizeof(int16_t));
}

#endif /* S16MCALLBACK_H_ */
