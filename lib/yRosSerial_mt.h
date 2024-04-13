/*
 * yRosSerial_mt.h
 *
 *  Created on: Sep 4, 2023
 *      Author: yp7
 */

#ifndef YROSSERIAL_MT_H_
#define YROSSERIAL_MT_H_

#include <stdint.h>

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
	uint8_t data;
}yRosSerial_uint8_t;

typedef struct
{
	int8_t data;
}yRosSerial_int8_t;

typedef struct
{
	uint16_t data;
}yRosSerial_uint16_t;

typedef struct
{
	int16_t data;
}yRosSerial_int16_t;

typedef struct
{
	uint32_t data;
}yRosSerial_uint32_t;

typedef struct
{
	int32_t data;
}yRosSerial_int32_t;

typedef struct
{
	uint64_t data;
}yRosSerial_uint64_t;

typedef struct
{
	int64_t data;
}yRosSerial_int64_t;

typedef struct
{
	uint16_t length;
	void *data;
}yRosSerial_voidmul_t;

typedef struct
{
	uint16_t length;
	uint8_t *data;
}yRosSerial_uint8mul_t;

typedef struct
{
	uint16_t length;
	int8_t *data;
}yRosSerial_int8mul_t;

typedef struct
{
	uint16_t length;
	uint16_t *data;
}yRosSerial_uint16mul_t;

typedef struct
{
	uint16_t length;
	int16_t *data;
}yRosSerial_int16mul_t;

typedef struct
{
	uint16_t length;
	uint32_t *data;
}yRosSerial_uint32mul_t;

typedef struct
{
	uint16_t length;
	int32_t *data;
}yRosSerial_int32mul_t;

typedef struct
{
	uint16_t length;
	uint64_t *data;
}yRosSerial_uint64mul_t;

typedef struct
{
	uint16_t length;
	int64_t *data;
}yRosSerial_int64mul_t;

typedef struct
{
	uint16_t length;
	float *data;
}yRosSerial_float32mul_t;

typedef struct
{
	uint16_t length;
	double *data;
}yRosSerial_float64mul_t;
#endif /* YROSSERIAL_MT_H_ */
