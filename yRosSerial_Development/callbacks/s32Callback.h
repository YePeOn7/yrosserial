/*
 * s32Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef S32CALLBACK_H_
#define S32CALLBACK_H_

extern int32_t subS32;

void s32Callback(void *message)
{
	subS32 = *((int32_t*)message);
}

#endif /* S32CALLBACK_H_ */
