/*
 * u32mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U32MCALLBACK_H_
#define U32MCALLBACK_H_
#include "yrosserial.h"
#include "main.h"
#include <string.h>

extern uint32_t subU32M[L_ARRAY];

void u32mCallback(void *message)
{
	size_t l = ((yRosSerial_uint32mul_t*)message)->length;
	memcpy(subU32M, ((yRosSerial_uint32mul_t*)message)->data, l*sizeof(uint32_t));
}

#endif /* U32MCALLBACK_H_ */
