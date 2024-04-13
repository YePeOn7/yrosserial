/*
 * s64mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S64MCALLBACK_H_
#define S64MCALLBACK_H_

extern int64_t subS64M[L_ARRAY];

void s64mCallback(void *message)
{
	size_t l = ((yRosSerial_int64mul_t*)message)->length;
	memcpy(subS64M, ((yRosSerial_int64mul_t*)message)->data, l*sizeof(int64_t));
}

#endif /* S64MCALLBACK_H_ */
