/*
 * u64mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U64MCALLBACK_H_
#define U64MCALLBACK_H_
#include "yrosserial.h"
#include "main.h"
#include <string.h>

extern uint64_t subU64M[L_ARRAY];

void u64mCallback(void *message)
{
	size_t l = ((yRosSerial_uint64mul_t*)message)->length;
	memcpy(subU64M, ((yRosSerial_uint64mul_t*)message)->data, l*sizeof(uint64_t));
}

#endif /* U64MCALLBACK_H_ */
