#ifndef __YROSSERIAL_H__
#define __YROSSERIAL_H__

#include <stdint.h>
#include <stdio.h>

#include "ringBuffer.h"
//#include "serial.h"
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

void yRosSerial_init(yRosSerial_setting_t *_setting);
void yRosSerial_spin();

void yRosSerial_advertise(const char* topicName,  yRosSerial_MessageType_t mType);

void yRosSerial_getRxBuffer(uint8_t *buffer);
void yRosSerial_getTxBuffer(uint8_t *buffer);
void yRosSerial_handleCompleteReceive(UART_HandleTypeDef *huart, uint16_t size);
void yRosSerial_handleCompleteTransmit(UART_HandleTypeDef *huart);
void responseTopic();
void check();
void transmitTest();

#endif /* __YROSSERIAL_H__ */
