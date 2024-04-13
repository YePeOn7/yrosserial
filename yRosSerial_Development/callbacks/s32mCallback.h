/*
 * s32mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S32MCALLBACK_H_
#define S32MCALLBACK_H_

extern int32_t subS32M[L_ARRAY];

void s32mCallback(void *message)
{
	size_t l = ((yRosSerial_int32mul_t*)message)->length;
	memcpy(subS32M, ((yRosSerial_int32mul_t*)message)->data, l*sizeof(int32_t));
}

#endif /* S32MCALLBACK_H_ */
