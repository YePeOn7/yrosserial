/*
 * u16Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U16CALLBACK_H_
#define U16CALLBACK_H_

extern uint16_t subU16;

void u16Callback(void *message)
{
	subU16 = *((uint16_t*)message);
}

#endif /* U16CALLBACK_H_ */
