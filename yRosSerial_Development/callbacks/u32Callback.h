/*
 * u32Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U32CALLBACK_H_
#define U32CALLBACK_H_

extern uint32_t subU32;

void u32Callback(void *message)
{
	subU32 = *((uint32_t*)message);
}

#endif /* U32CALLBACK_H_ */
