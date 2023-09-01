#include <string.h>

#include "yrosserial.h"

#define MAX_MESSAGE_SIZE    128 // maximum message size (Should not be more than buffer size)

static RingBuffer_t *rx;
static RingBuffer_t *tx;
static yRosSerial_setting_t setting = { 0 };
static uint8_t initialized;

// temporary buffer
static uint8_t rTemp[3];
static uint8_t tTemp[3];

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

//static void txSend(uint8_t data, size_t len)
//{
//
////	RingBuffer_pop(rx, tTemp, rx->count);
////
////	HAL_UART_Transmit_DMA(setting.huart, tTemp, Size);
//}

static void responseTopic()
{
	printf("Receive request topic. Responding topic...\n");

}

static int processIncomingMessage(RingBuffer_t *rb, size_t len)
{
	if (rb->count < len) return -1;
	enum
	{
		NO_INSTRUCTION, REQUESTING_TOPIC, RESPONSE_TOPIC, REQUEST_SYNC, RESPONSE_SYNC
	};

	// instruction
	if (rb->buffer[rb->tail] < 10)
	{
		uint8_t instruction = rb->buffer[rb->tail];
		printf("tail:%d, instruction: %d, len:%d \n", rb->tail, instruction, len);
		if (validateChecksum(rb, len))
		{
			switch (instruction)
			{
			case REQUESTING_TOPIC:
				responseTopic();
				break;
			}
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
//		for (int i = 0; i < size; i++)
//		{
//			printf("%d ", rx->buffer[rx->head + i]);
//		}
//		printf("\n");

		RingBuffer_append(rx, rTemp, size);
//		HAL_UARTEx_ReceiveToIdle_DMA(setting.huart, rTemp, sizeof(rTemp));
		__HAL_DMA_DISABLE_IT(setting.hdma_rx, DMA_IT_HT);
		return;
	}
}

void yRosSerial_spin()
{
	enum
	{
		GET_HEADER1, GET_HEADER2, GET_LENGTH, GET_MESSAGE
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

void yRosSerial_getRxBuffer(uint8_t *buffer, size_t len)
{
	memcpy(buffer, rx->buffer, len);
}

void check()
{
	printf("c: %d, h: %d, t: %d, dma: %ld\n", rx->count, rx->head, rx->tail, __HAL_DMA_GET_COUNTER(setting.huart->hdmarx));
}
