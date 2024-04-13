/*
 * s64mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef F64MCALLBACK_H_
#define F64MCALLBACK_H_

extern double subF64M[L_ARRAY];

void f64mCallback(void *message)
{
	size_t l = ((yRosSerial_float64mul_t*)message)->length;
	memcpy(subF64M, ((yRosSerial_float64mul_t*)message)->data, l*sizeof(double));
}

#endif /* F64MCALLBACK_H_ */
