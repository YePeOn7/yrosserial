/*
 * s8mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S8MCALLBACK_H_
#define S8MCALLBACK_H_

extern int8_t subS8M[L_ARRAY];

void s8mCallback(void *message)
{
	size_t l = ((yRosSerial_int8mul_t*)message)->length;
	memcpy(subS8M, ((yRosSerial_int8mul_t*)message)->data, l);
}

#endif /* S8MCALLBACK_H_ */
