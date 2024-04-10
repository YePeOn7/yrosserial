/*
 * float32Callback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef FLOAT64CALLBACK_H_
#define FLOAT64CALLBACK_H_

extern double f64;

void float64Callback(void *message)
{
	f64 = *((double*)message);
}

#endif /* FLOAT64CALLBACK_H_ */
