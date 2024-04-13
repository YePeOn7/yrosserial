/*
 * float32Callback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef F64CALLBACK_H_
#define F64CALLBACK_H_

extern double f64;

void f64Callback(void *message)
{
	f64 = *((double*)message);
}

#endif /* F64CALLBACK_H_ */
