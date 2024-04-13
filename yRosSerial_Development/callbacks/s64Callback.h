/*
 * s64Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S64CALLBACK_H_
#define S64CALLBACK_H_

extern int64_t subS64;

void s64Callback(void *message)
{
	subS64 = *((int64_t*)message);
}

#endif /* S64CALLBACK_H_ */
