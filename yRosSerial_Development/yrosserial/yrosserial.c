
#include <string.h>

#include "yrosserial.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_MESSAGE_SIZE    128 // maximum message size (Should not be more than buffer size)
#define MAX_PUBLISHER_SIZE	20
#define MAX_SUBSRIBER_SIZE	20
#define HEADER				{0x05, 0x09}
#define MAX_INSTRUCTION_VAL	10

static yRosSerial_pubHandle_t publisherList[MAX_PUBLISHER_SIZE] = { 0 };
yRosSerial_subHandle_t subscriberList[MAX_SUBSRIBER_SIZE] = { 0 };
RingBuffer_t *rx;
RingBuffer_t *tx;
Rb2_t rbRx = {0};
static yRosSerial_setting_t setting = { 0 };
static uint8_t initialized;
static uint8_t topicId = 10; // topic Id start from 10

// temporary buffer
uint8_t rTemp[128];
uint8_t tTemp[128];


/**
 * @brief To validate the checksum from the received packet
 * 
 * @param rb ring buffer parameter that hold the packet
 * @param len the length of the packet
 * @return int the status of checksum verification
 */
static int validateChecksum(Rb2_t *rb, size_t len)
{
	uint8_t sum = len;
	for (int i = 0; i < len - 1; i++)
		sum += rb->buffer[(rb->tail + i) % rb->size];

	uint8_t obtainedSum = rb->buffer[(rb->tail + len - 1) % rb->size];
//	if (sum == obtainedSum) printf("Checksum is correct\n");
//	else printf("Checksum get: %d, expected: %d\n", obtainedSum, sum);

	return (sum == obtainedSum);
}
int abc = 0;

/**
 * @brief Sending the data that has been appended to tx ring buffer
 * 
 */
static void txFlush()
{
	abc++;
	if (__HAL_UART_GET_FLAG(setting.huart, UART_FLAG_TC))
	{
		size_t bytesToFlush = RingBuffer_popCopy(tx, tTemp, sizeof(tTemp));
		HAL_UART_Transmit_DMA(setting.huart, tTemp, bytesToFlush);
//		return;
	}
}

/**
 * @brief read data from ring buffer without modify ring buffer component. The data will be read from tail + offset as much len parameter
 * @param offset The start index to read from the tail
 * @param len the number of data to be obtained
 * @return None
 */
void readRingBuffer(RingBuffer_t *rb, uint16_t offset, uint16_t len, void* output)
{
	uint8_t *b = rb->buffer;
	uint16_t t = rb->tail;
	uint16_t bz = rb->size;
	if(t + offset + len <= bz || t + offset > bz) memcpy(output, b+((offset+t)%bz), len);
	else
	{
		size_t len1 = bz - (t + offset);
		size_t len2 = len - len1;
		memcpy(output, b + t + offset, len1);
		memcpy((uint8_t*)output + len1, b, len2);
	}
}

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
		txFlush();
		HAL_Delay(5);
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
		txFlush();
		HAL_Delay(5);
	}
}

/**
 * @brief Parsing the incomming message to decide which response to be sent to the client
 * 
 * @param rb should be a ring buffer with tail that point to parameter instruction or topicId (packet after length, should be 4th byte)
 * @param len the packets length excluding header and length info
 * @return int 
 */
static int processIncomingMessage(Rb2_t *rb, size_t len)
{
	uint8_t *b = rb->buffer;
	uint16_t t = rb->tail;
	size_t a = rb2_getAvailable(rb);

	if (a < len) return -1;
	if (len == 0) return -2;

	// validate checksum
	if (!validateChecksum(rb, len))
	{
		rb->tail = (rb->tail + len) % rb->size;
		rb->count = rb2_getAvailable(rb);
		return -2;
	}

	if (b[t] < MAX_INSTRUCTION_VAL) // process instruction
	{
		uint8_t instruction;
		rb2_pop(rb, &instruction, 1);
//		printf("tail:%d, instruction: %d, len:%d \n", rb->tail, instruction, len);
		if (instruction == INS_REQ_TOPIC) responseTopic();

	}
	else if(b[t] >= MAX_INSTRUCTION_VAL) // process message from subscibed topic
	{
		int id = b[t]; //topic id
		uint8_t x[512];

		memcpy(x, rb->buffer, 256);

		uint8_t data[len-2]; // -2 for excluding id and mt
		rb2_pop(rb, data, 2); // remove 2 data
		rb2_pop(rb, data, len-2);

		for(int i = 0; i < MAX_SUBSRIBER_SIZE; i++)
		{
			if(subscriberList[i].id == id){
				subscriberList[i].callback((void*)data);
			}
		}
	}

	return 0;
}

//------------------ Interface Implementation --------------- //
void yRosSerial_init(yRosSerial_setting_t *_setting)
{
	tx = RingBuffer_create(_setting->txBufSize);
	memcpy(&setting, _setting, sizeof(yRosSerial_setting_t));

	rbRx.buffer = calloc(_setting->rxBufSize, sizeof(uint8_t));
	rbRx.hdma = _setting->hdma_rx;
	rbRx.huart = _setting->huart;
	rbRx.size = _setting->rxBufSize;

	initialized = 1;

	HAL_UART_Receive_DMA(rbRx.huart, rbRx.buffer, rbRx.size);
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
	static size_t len = 0;
	static int skipCount = 0;
	uint8_t breakFor = 0;
	size_t available = 0;

	while ((available = rb2_getAvailable(&rbRx)) > 0)
	{
		uint8_t data = 0;
		// printf("check a: %d\n", a);
		if (state != GET_MESSAGE)
		{
			rb2_pop(&rbRx, &data, 1);
//			printf("tail: %d, c: %d, getData: %d\n", rbRx.tail, available, data);
		}
		switch (state)
		{
		case GET_HEADER1:
//			printf("Checking Header 1\n");
			if (data == 5) state = GET_HEADER2;
			break;
		case GET_HEADER2:
//			printf("Checking Header 2\n");
			if (data == 9) state = GET_LENGTH;
			else if (data == 5) ;         // keep the current state
			else state = GET_HEADER1;   // reset the state
			break;
		case GET_LENGTH:
//			printf("Getting length\n");
			len = data;
			state = GET_MESSAGE;
			break;
		case GET_MESSAGE:
//			printf("Getting Message\n");
			if (available >= len)
			{
				processIncomingMessage(&rbRx, len);
				state = GET_HEADER1;
				len = 0;
			}
			else
			{
//				printf("Break! rb count: %d\n", available);
				breakFor = 1;
				skipCount++;
				if(skipCount > 15){
					state = GET_HEADER1;
					len = 0;
				}
			}
			break;
		}

		if (breakFor) break;

//		printf("\n");
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

yRosSerial_subHandle_t* yRosSerial_subscribe(const char* topicName, yRosSerial_MessageType_t mType, Callback_t callback)
{
	for (int i = 0; i < MAX_SUBSRIBER_SIZE; i++)
	{
		if (subscriberList[i].id == 0)
		{
			subscriberList[i].id = topicId++;
			subscriberList[i].name = topicName;
			subscriberList[i].type = mType;
			subscriberList[i].callback = callback;
			return &subscriberList[i];
		}
	}

	return NULL;
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
		yRosSerial_string_t* strMsg = (yRosSerial_string_t*) message;
		messageBase.length = strlen(strMsg->data) + 4; // add null terminator, topicId, Message type, and checksum

		// generate checksum
		uint8_t checksum = messageBase.length;
		checksum += hpub->id;
		checksum += hpub->type;
		for (int i = 0; i < strlen(strMsg->data); i++)
		{
			checksum += strMsg->data[i];
		}

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
		yRosSerial_float32_t* msg = (yRosSerial_float32_t*) message;
		size_t msgSize = sizeof(yRosSerial_float32_t);
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
		yRosSerial_float64_t* msg = (yRosSerial_float64_t*) message;
		size_t msgSize = sizeof(yRosSerial_float64_t);
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
		yRosSerial_odometry2d_t* msg = (yRosSerial_odometry2d_t*) message;
		size_t msgSize = sizeof(yRosSerial_odometry2d_t);
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
		yRosSerial_twist2d_t* msg = (yRosSerial_twist2d_t*) message;
		size_t msgSize = sizeof(yRosSerial_twist2d_t);
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
	else if(hpub->type == MT_UINT8_MULTIARRAY ||
			hpub->type == MT_INT8_MULTIARRAY ||
			hpub->type == MT_UINT16_MULTIARRAY ||
			hpub->type == MT_INT16_MULTIARRAY ||
			hpub->type == MT_UINT32_MULTIARRAY ||
			hpub->type == MT_INT32_MULTIARRAY ||
			hpub->type == MT_UINT64_MULTIARRAY ||
			hpub->type == MT_INT64_MULTIARRAY ||
			hpub->type == MT_FLOAT32_MULTIARRAY ||
			hpub->type == MT_FLOAT64_MULTIARRAY)
	{
		yRosSerial_uint8mul_t* msg = (yRosSerial_uint8mul_t*) message;
		size_t s = 0; //to handle size for single array
		switch(hpub->type)
		{
		case MT_UINT8_MULTIARRAY:
		case MT_INT8_MULTIARRAY: s = 1; break;
		case MT_UINT16_MULTIARRAY:
		case MT_INT16_MULTIARRAY: s = 2; break;
		case MT_UINT32_MULTIARRAY:
		case MT_INT32_MULTIARRAY:
		case MT_FLOAT32_MULTIARRAY: s = 4; break;
		case MT_UINT64_MULTIARRAY:
		case MT_INT64_MULTIARRAY:
		case MT_FLOAT64_MULTIARRAY: s = 8; break;
		default: break;
		}
		size_t ls = sizeof(msg->length);

		size_t msgSize = s * msg->length + ls;
		messageBase.length = msgSize + 3; //additional: topicId, Message type, checksum

		uint8_t m[msgSize];
		memcpy(m, &msg->length, ls);
		memcpy(m+ls, msg->data, msg->length * s);

		// generate checksum
		uint8_t checksum = 0;
		checksum += messageBase.length;
		checksum += messageBase.topicId;
		checksum += messageBase.type;
		for(int i = 0; i < msgSize; i++)
		{
			checksum += m[i];
		}

		RingBuffer_append(tx, header, sizeof(header));
		RingBuffer_append(tx, (uint8_t*)&messageBase, sizeof(messageBase));
		RingBuffer_append(tx, m, msgSize);
		RingBuffer_append(tx, &checksum, sizeof(uint8_t));
	}
	txFlush();
}

void yRosSerial_getRxBuffer(uint8_t *buffer)
{
	memcpy(buffer, rbRx.buffer, rbRx.size);
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
