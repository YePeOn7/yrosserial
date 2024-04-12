/*
 * yRosSerial_mt.h
 *
 *  Created on: Sep 4, 2023
 *      Author: yp7
 */

#ifndef YROSSERIAL_MT_H_
#define YROSSERIAL_MT_H_

typedef enum
{
	MT_STRING,
	MT_FLOAT32,
	MT_FLOAT64,
	MT_ODOMETRY2D,
	MT_TWIST2D,
	MT_UINT8,
	MT_INT8,
	MT_UINT16,
	MT_INT16,
	MT_UINT32,
	MT_INT32,
	MT_UINT64,
	MT_INT64,
	MT_UINT8_MULTIARRAY,
	MT_INT8_MULTIARRAY,
	MT_UINT16_MULTIARRAY,
	MT_INT16_MULTIARRAY,
	MT_UINT32_MULTIARRAY,
	MT_INT32_MULTIARRAY,
	MT_UINT64_MULTIARRAY,
	MT_INT64_MULTIARRAY,
	MT_FLOAT32_MULTIARRAY,
	MT_FLOAT64_MULTIARRAY
} yRosSerial_MessageType_t;

typedef struct
{
	const char* data;
} yRosSerial_string_t;

typedef struct
{
	float data;
}yRosSerial_float32_t;

typedef struct
{
	double data;
}yRosSerial_float64_t;

typedef struct
{
	float x;
	float y;
	float w;
}yRosSerial_odometry2d_t;

typedef struct
{
	float x;
	float y;
	float w;
}yRosSerial_twist2d_t;

typedef struct
{
	float data;
}yRosSerial_uint8_t;

#endif /* YROSSERIAL_MT_H_ */
