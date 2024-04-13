/*
 * u8Callback.h
 *
 *  Created on: Apr 13, 2024
 *      Author: yp7
 */

#ifndef U8CALLBACK_H_
#define U8CALLBACK_H_

extern uint8_t subU8;

void u8Callback(void *message)
{
	subU8 = *((uint8_t*)message);
}

#endif /* U8CALLBACK_H_ */
