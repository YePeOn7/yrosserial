/*
 * twist2dCallback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef TWIST2DCALLBACK_H_
#define TWIST2DCALLBACK_H_

#include "yrosserial.h"
extern yRosSerial_twist2d_t twist2d;

void twist2dCallback(void *message)
{
	twist2d = *((yRosSerial_twist2d_t*)message);
}


#endif /* TWIST2DCALLBACK_H_ */
