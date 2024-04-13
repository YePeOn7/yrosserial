/*
 * u16mCallback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U16MCALLBACK_H_
#define U16MCALLBACK_H_
#include "yrosserial.h"
#include "main.h"
#include <string.h>

extern uint16_t subU16M[L_ARRAY];

void u16mCallback(void *message)
{
	size_t l = ((yRosSerial_uint16mul_t*)message)->length;
	memcpy(subU16M, ((yRosSerial_uint16mul_t*)message)->data, l*sizeof(uint16_t));
}

#endif /* U16MCALLBACK_H_ */
