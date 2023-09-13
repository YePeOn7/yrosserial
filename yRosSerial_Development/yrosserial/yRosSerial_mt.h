/*
 * yRosSerial_mt.h
 *
 *  Created on: Sep 4, 2023
 *      Author: yp7
 */

#ifndef YROSSERIAL_MT_H_
#define YROSSERIAL_MT_H_

typedef struct
{
	const char* data;
} yRosSerial_string;

typedef struct
{
	float data;
}yRosSerial_float32;

typedef struct
{
	float x;
	float y;
	float w;
}yRosSerial_odometry2d;

typedef struct
{
	float x;
	float y;
	float w;
}yRosSerial_twist2d;

#endif /* YROSSERIAL_MT_H_ */
