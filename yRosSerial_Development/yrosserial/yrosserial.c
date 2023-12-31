#include <string.h>

#include "yrosserial.h"
#include <stdio.h>

#define MAX_MESSAGE_SIZE    128 // maximum message size (Should not be more than buffer size)
#define MAX_PUBLISHER_SIZE	10
#define MAX_SUBSRIBER_SIZE	10
#define HEADER				{0x05, 0x09}

static yRosSerial_pubHandle_t publisherList[MAX_PUBLISHER_SIZE] = { 0 };
yRosSerial_subHandle_t subscriberList[MAX_SUBSRIBER_SIZE] = { 0 };
RingBuffer_t *rx;
RingBuffer_t *tx;
static yRosSerial_setting_t setting = { 0 };
static uint8_t initialized;
static uint8_t topicId = 10; // topic Id start from 10

// temporary buffer
uint8_t rTemp[64];
uint8_t tTemp[64];

static int validateChecksum(RingBuffer_t *rb, size_t len)
{
	uint8_t sum = len;
	for (int i = 0; i < len - 1; i++)
		sum += rb->buffer[(rb->tail + i) % rb->size];

	uint8_t obtainedSum = rb->buffer[(rb->tail + len - 1) % rb->size];
	if (sum == obtainedSum) printf("Checksum is correct\n");
	else printf("Checksum get: %d, expected: %d\n", obtainedSum, sum);

	return (sum == obtainedSum);
}

static void txFlush()
{
	if (__HAL_UART_GET_FLAG(setting.huart, UART_FLAG_TC))
	{
		size_t bytesToFlush = RingBuffer_popCopy(tx, tTemp, sizeof(tTemp));
		HAL_UART_Transmit_DMA(setting.huart, tTemp, bytesToFlush);
//		return;
	}
}

/**
 * @brief add header and ack to the packet and put it to RingBuffer
 * @param message The message to serialize
 * @param mt Message Type
 * @return None
 */
//static void serialize(void* message, yRosSerial_MessageType_t mt,size_t len)
//{
//	uint8_t header={0x05, 0x09};
//	uint8_t ack = 0;
//
//	// get ack
//	if(mt == MT)
//}

void responseTopic()
{
	printf("Receive request topic. Responding topic...\n");
	for (int i = 0; i < MAX_PUBLISHER_SIZE; i++)
	{
		if (publisherList[i].id == 0) break;

		yRosSerial_responseTopic_t r = { 0 };
		r.length = strlen(publisherList[i].name) + 1 + 5; // 1: null terminator, 5: instruction, topic id, type, action, checksum
		r.action = ACT_PUBLISH;
		r.instruction = INS_RES_TOPIC;
		r.topicId = publisherList[i].id;
		r.type = publisherList[i].type;
		strcpy(r.topicName, publisherList[i].name);

		// calculate ack
		uint8_t ack = 0;
		ack += r.length;
		ack += r.instruction;
		ack += r.action;
		ack += r.topicId;
		ack += r.type;
		for(int i = 0; i < strlen(r.topicName) + 1; i++)
		{
			ack += r.topicName[i];
		}

		// pack header, message and ack to ring buffer
		uint8_t header[] = {0x05, 0x09};

		printf("Send Publisher[%d]: %s , l: %d, t: %d\n", r.topicId, r.topicName, r.length, r.type);
		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*) &r, r.length); // r.length measure topic name len + null terminator, action, instruction, topic id, and checksum
		RingBuffer_append(tx, &ack, sizeof(uint8_t));
	}
	for(int i = 0; i < MAX_SUBSRIBER_SIZE; i++)
	{
		if (subscriberList[i].id == 0) break;

		yRosSerial_responseTopic_t r = { 0 };
		r.length = strlen(subscriberList[i].name) + 1 + 5; // 1: null terminator, 5: instruction, topic id, type, action, checksum
		r.action = ACT_SUBSCRIBE;
		r.instruction = INS_RES_TOPIC;
		r.topicId = subscriberList[i].id;
		r.type = subscriberList[i].type;
		strcpy(r.topicName, subscriberList[i].name);

		// calculate ack
		uint8_t ack = 0;
		ack += r.length;
		ack += r.instruction;
		ack += r.action;
		ack += r.topicId;
		ack += r.type;
		for(int i = 0; i < strlen(r.topicName) + 1; i++)
		{
			ack += r.topicName[i];
		}

		// pack header, message and ack to ring buffer
		uint8_t header[] = {0x05, 0x09};

		printf("Send Subscriber[%d]: %s , l: %d, t: %d\n", r.topicId, r.topicName, r.length, r.type);
		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*) &r, r.length); // r.length measure topic name len + null terminator, action, instruction, topic id, and checksum
		RingBuffer_append(tx, &ack, sizeof(uint8_t));
	}
	txFlush();
}

static int processIncomingMessage(RingBuffer_t *rb, size_t len)
{
	if (rb->count < len) return -1;

	// instruction
	if (rb->buffer[rb->tail] < 10)
	{
		uint8_t instruction = rb->buffer[rb->tail];
		printf("tail:%d, instruction: %d, len:%d \n", rb->tail, instruction, len);
		if (validateChecksum(rb, len))
		{
			if (instruction == INS_REQ_TOPIC) responseTopic();
		}
		else
		{
			rx->tail = (rx->tail + len) % rx->size;
			rx->count -= len;
			return -2;
		}
	}

	// Topic Id
	else
	{
		// uint8_t topicId = message[0];
	}

	rx->count -= len;
	rx->tail = (rx->tail + len) % rx->size;

	return 0;
}

//------------------ Interface Implementation --------------- //

void yRosSerial_init(yRosSerial_setting_t *_setting)
{
	rx = RingBuffer_create(_setting->rxBufSize);
	tx = RingBuffer_create(_setting->txBufSize);
	memcpy(&setting, _setting, sizeof(yRosSerial_setting_t));

	initialized = 1;

	HAL_UARTEx_ReceiveToIdle_DMA(setting.huart, rTemp, sizeof(rTemp));
	__HAL_DMA_DISABLE_IT(setting.hdma_rx, DMA_IT_HT);
}

void yRosSerial_handleCompleteReceive(UART_HandleTypeDef *huart, uint16_t size)
{
	if (huart == setting.huart)
	{
		RingBuffer_append(rx, rTemp, size);
		HAL_UARTEx_ReceiveToIdle_DMA(setting.huart, rTemp, sizeof(rTemp));
		__HAL_DMA_DISABLE_IT(setting.hdma_rx, DMA_IT_HT);
		return;
	}
}

void yRosSerial_handleCompleteTransmit(UART_HandleTypeDef *huart)
{
	if (huart == setting.huart)
	{
		if(tx->count > 0)
		{
			txFlush();
		}
		return;
	}
}

void yRosSerial_spin()
{
	enum
	{
		GET_HEADER1,
		GET_HEADER2,
		GET_LENGTH,
		GET_MESSAGE
	};

	static int state = GET_HEADER1;
//    static uint8_t bufferMessage[MAX_MESSAGE_SIZE - 3] = {0}; // It will contain all packet excluding teh header 1, header 2, and length
	static size_t len = 0;
	uint8_t breakFor = 0;

	while (rx->count > 0)
	{
		uint8_t *data = NULL;
		// printf("check rb->count: %d\n", rb->count);
		if (state != GET_MESSAGE)
		{
			RingBuffer_pop(rx, &data, 1);
			printf("tail: %d, c: %d, getData: %d\n", rx->tail, rx->count, *data);
		}
		switch (state)
		{
		case GET_HEADER1:
			printf("Checking Header 1\n");
			if (*data == 5) state = GET_HEADER2;
			break;
		case GET_HEADER2:
			printf("Checking Header 2\n");
			if (*data == 9) state = GET_LENGTH;
			else if (*data == 5) ;         // keep the current state
			else state = GET_HEADER1;   // reset the state
			break;
		case GET_LENGTH:
			printf("Getting length\n");
			len = *data;
			state = GET_MESSAGE;
			break;
		case GET_MESSAGE:
			printf("Getting Message\n");
			if (rx->count >= len)
			{
//                RingBuffer_popCopy(rx, bufferMessage, len);
				processIncomingMessage(rx, len);
				state = GET_HEADER1;
				len = 0;
			}
			else
			{
				printf("Break! rb count: %d\n", rx->count);
				breakFor = 1;
			}
			break;
		}

		if (breakFor) break;

		printf("\n");
	}
}

yRosSerial_pubHandle_t* yRosSerial_advertise(const char *topicName, yRosSerial_MessageType_t mType)
{
	for (int i = 0; i < MAX_PUBLISHER_SIZE; i++)
	{
		if (publisherList[i].id == 0)
		{
			publisherList[i].id = topicId++;
			publisherList[i].name = topicName;
			publisherList[i].type = mType;
			return &publisherList[i];
		}
	}

	return NULL;
}

void yRosSerial_subscribe(const char* topicName, yRosSerial_MessageType_t mType, Callback_t callback)
{
	for (int i = 0; i < MAX_SUBSRIBER_SIZE; i++)
	{
		if (subscriberList[i].id == 0)
		{
			subscriberList[i].id = topicId++;
			subscriberList[i].name = topicName;
			subscriberList[i].type = mType;
			subscriberList[i].callback = callback;
			return;
		}
	}
}

void yRosSerial_publish(yRosSerial_pubHandle_t* hpub, void* message)
{
	uint8_t t[256];
	uint8_t header[] = HEADER;
	yRosSerial_messageBase_t messageBase = {0}; // handle packet data before main message

	messageBase.topicId = hpub->id;
	messageBase.type = hpub->type;
	if(hpub->type == MT_STRING)
	{
		yRosSerial_string* strMsg = (yRosSerial_string*) message;
		messageBase.length = strlen(strMsg->data) + 4; // add null terminator, topicId, Message type, and checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for (int i = 0; i < strlen(strMsg->data); i++)
		{
			checksum += strMsg->data[i];
		}
//		size_t l=strlen(strMsg->data);
		// Package packet to Ring Buffer
		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, (uint8_t*)strMsg->data, strlen(strMsg->data)+1);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
//		printf("%s (%d)\n", strMsg->data, messageBase.length);
		memcpy(t, tx->buffer, 256);
	}
	else if(hpub->type == MT_FLOAT32)
	{
		yRosSerial_float32* msg = (yRosSerial_float32*) message;
		size_t msgSize = sizeof(yRosSerial_float32);
		messageBase.length = msgSize+ 3; //additional: topicId, Message type, checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for(int i = 0; i < msgSize; i++)
		{
			checksum += ((uint8_t*)msg)[i];
		}

		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, (uint8_t*)msg, msgSize);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
//		printf("float ==> data: %.3f\n", msg->data);

//		for(int i = 0; i < sizeof(messageBase); i++)
//		{
//			printf("%d ", ((uint8_t*)&messageBase)[i]);
//		}
//
//		for(int i = 0; i < msgSize; i++)
//		{
//			printf("%d ", ((uint8_t*)msg)[i]);
//		}
//		printf("%d\n", checksum);
	}
	else if(hpub->type == MT_FLOAT64)
	{
		yRosSerial_float64* msg = (yRosSerial_float64*) message;
		size_t msgSize = sizeof(yRosSerial_float64);
		messageBase.length = msgSize+ 3; //additional: topicId, Message type, checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for(int i = 0; i < msgSize; i++)
		{
			checksum += ((uint8_t*)msg)[i];
		}

		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, (uint8_t*)msg, msgSize);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
//		printf("float ==> data: %.3f\n", msg->data);

//		for(int i = 0; i < sizeof(messageBase); i++)
//		{
//			printf("%d ", ((uint8_t*)&messageBase)[i]);
//		}
//
//		for(int i = 0; i < msgSize; i++)
//		{
//			printf("%d ", ((uint8_t*)msg)[i]);
//		}
//		printf("%d\n", checksum);
	}
	else if(hpub->type == MT_ODOMETRY2D)
	{
		yRosSerial_odometry2d* msg = (yRosSerial_odometry2d*) message;
		size_t msgSize = sizeof(yRosSerial_odometry2d);
		messageBase.length = msgSize + 3; //additional: topicId, Message type, checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for(int i = 0; i < msgSize; i++)
		{
			checksum += ((uint8_t*)msg)[i];
		}

		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, (uint8_t*)msg, msgSize);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
//		printf("odometry2d ==> x: %.3f y: %.3f w: %.3f\n", msg->x, msg->y, msg->w);

//		for(int i = 0; i < sizeof(messageBase); i++)
//		{
//			printf("%d ", ((uint8_t*)&messageBase)[i]);
//		}
//
//		for(int i = 0; i < msgSize; i++)
//		{
//			printf("%d ", ((uint8_t*)msg)[i]);
//		}
//		printf("%d\n", checksum);
	}
	else if(hpub->type == MT_TWIST2D)
	{
		yRosSerial_twist2d* msg = (yRosSerial_twist2d*) message;
		size_t msgSize = sizeof(yRosSerial_twist2d);
		messageBase.length = msgSize + 3; //additional: topicId, Message type, checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for(int i = 0; i < msgSize; i++)
		{
			checksum += ((uint8_t*)msg)[i];
		}

		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, (uint8_t*)msg, msgSize);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
//		printf("twist2d ==> x: %.3f y: %.3f w: %.3f\n", msg->x, msg->y, msg->w);

//		for(int i = 0; i < sizeof(messageBase); i++)
//		{
//			printf("%d ", ((uint8_t*)&messageBase)[i]);
//		}
//
//		for(int i = 0; i < msgSize; i++)
//		{
//			printf("%d ", ((uint8_t*)msg)[i]);
//		}
//		printf("%d\n", checksum);
	}
	txFlush();
}

void yRosSerial_getRxBuffer(uint8_t *buffer)
{
	memcpy(buffer, rx->buffer, rx->size);
}

void yRosSerial_getTxBuffer(uint8_t *buffer)
{
	memcpy(buffer, tx->buffer, tx->size);
}

uint16_t yRosSerial_getRxCount()
{
	return rx->count;
}

uint16_t yRosSerial_getTxCount()
{
	return tx->count;
}

void check()
{
//	printf("c: %d, h: %d, t: %d, dma: %ld, tc: %d\n", rx->count, rx->head, rx->tail, __HAL_DMA_GET_COUNTER(setting.huart->hdmarx), __HAL_UART_GET_FLAG(setting.huart, UART_FLAG_TC));
//	printf("c: %d, h: %d, t: %d, dma: %ld, tc: %d\n", tx->count, tx->head, tx->tail, __HAL_DMA_GET_COUNTER(setting.huart->hdmarx), __HAL_UART_GET_FLAG(setting.huart, UART_FLAG_TC));
//	static int c = 0;
//
//	RingBuffer_append(tx, (uint8_t*) &c, sizeof(int));
//	txFlush();
//
//	c++;
}

void transmitTest()
{
	char* stringData = "YOHAN\n";

	RingBuffer_append(tx, (uint8_t*)stringData, strlen(stringData) + 1);
	txFlush();
}
