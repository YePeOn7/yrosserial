/*
 * odometry2dCallback.h
 *
 *  Created on: Apr 10, 2024
 *      Author: yp7
 */

#ifndef ODOMETRY2DCALLBACK_H_
#define ODOMETRY2DCALLBACK_H_

#include "yrosserial.h"
extern yRosSerial_odometry2d odometry2d;

void odometry2dCallback(void *message)
{
	odometry2d = *((yRosSerial_odometry2d*)message);
}

#endif /* ODOMETRY2DCALLBACK_H_ */
