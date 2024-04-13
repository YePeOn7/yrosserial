/*
 * u8mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U8MCALLBACK_H_
#define U8MCALLBACK_H_
#include "yrosserial.h"
#include "main.h"
#include <string.h>

extern uint8_t subU8M[L_ARRAY];

void u8mCallback(void *message)
{
	size_t l = ((yRosSerial_uint8mul_t*)message)->length;
	memcpy(subU8M, ((yRosSerial_uint8mul_t*)message)->data, l*sizeof(uint8_t));
}

#endif /* U8MCALLBACK_H_ */
