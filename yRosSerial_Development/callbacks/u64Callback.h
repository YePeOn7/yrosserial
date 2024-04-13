/*
 * u64Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U64CALLBACK_H_
#define U64CALLBACK_H_

extern uint64_t subU64;

void u64Callback(void *message)
{
	subU64 = *((uint64_t*)message);
}

#endif /* U64CALLBACK_H_ */
