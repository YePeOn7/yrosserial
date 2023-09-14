#ifndef __YROSSERIAL_H__
#define __YROSSERIAL_H__

#include <stdint.h>
#include <stdio.h>

#include "ringBuffer.h"
#include "yRosSerial_mt.h"
#include "main.h" // from the project

typedef enum
{
	INS_NONE,
	INS_REQ_TOPIC,
	INS_RES_TOPIC,
	INS_REQ_SYNC,
	INS_RES_SYNC
} yRosSerial_instruction_t;;

typedef enum
{
	MT_STRING,
	MT_FLOAT32,
	MT_FLOAT64,
	MT_ODOMETRY2D,
	MT_TWIST2D,
} yRosSerial_MessageType_t;

typedef enum
{
	ACT_SUBSCRIBE,
	ACT_PUBLISH
} yRosSerial_action_t;

typedef struct
{
	size_t txBufSize;
	size_t rxBufSize;
	UART_HandleTypeDef *huart;
	DMA_HandleTypeDef *hdma_rx;
} yRosSerial_setting_t;

typedef struct
{
	uint8_t length;
	yRosSerial_instruction_t instruction;
	uint8_t topicId;
	uint8_t type;
	yRosSerial_action_t action;
	char topicName[252];
} yRosSerial_responseTopic_t;

typedef struct
{
	uint8_t length;
	uint8_t topicId;
	uint8_t type;
} yRosSerial_messageBase_t;

typedef void (*Callback_t)(void *);

typedef struct
{
	const char *name;
	uint8_t id;
	yRosSerial_MessageType_t type;
} yRosSerial_pubHandle_t;

typedef struct
{
	const char *name;
	uint8_t id;
	yRosSerial_MessageType_t type;
	Callback_t callback;
} yRosSerial_subHandle_t;

void yRosSerial_init(yRosSerial_setting_t *_setting);
void yRosSerial_spin();

yRosSerial_pubHandle_t* yRosSerial_advertise(const char* topicName,  yRosSerial_MessageType_t mType);
void yRosSerial_subscribe(const char* topicName, yRosSerial_MessageType_t mType, Callback_t callback);
void yRosSerial_publish(yRosSerial_pubHandle_t* hpub, void* message);

void yRosSerial_getRxBuffer(uint8_t *buffer);
void yRosSerial_getTxBuffer(uint8_t *buffer);
void yRosSerial_handleCompleteReceive(UART_HandleTypeDef *huart, uint16_t size);
void yRosSerial_handleCompleteTransmit(UART_HandleTypeDef *huart);
uint16_t yRosSerial_getRxCount();
uint16_t yRosSerial_getTxCount();

void responseTopic();
void check();
void transmitTest();

#endif /* __YROSSERIAL_H__ */
