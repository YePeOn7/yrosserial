/**
 * @file yrosserial.h
 * @author Yohan Prakoso (prakoso.yohan@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __YROSSERIAL_H__
#define __YROSSERIAL_H__

#include "ringBuffer.h"
#include "rb2.h"
#include "yRosSerial_mt.h"

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
	ACT_SUBSCRIBE,
	ACT_PUBLISH
} yRosSerial_action_t;

typedef struct
{
	size_t txBufSize;
	size_t rxBufSize;
	uint8_t* rx;
	uint8_t* tx;
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

/**
 * @brief Initializing the yRosSerial lib
 * 
 * @param _setting the setting parameter to be use by the lib
 */
void yRosSerial_init(yRosSerial_setting_t *_setting);

/**
 * @brief Process incomming packets
 * 
 */
void yRosSerial_spin();

/**
 * @brief Assign a topic to be published by the MCU
 * 
 * @param topicName The name of the topic
 * @param mType The Type of Message
 * @return yRosSerial_pubHandle_t* 
 */
yRosSerial_pubHandle_t* yRosSerial_advertise(const char* topicName,  yRosSerial_MessageType_t mType);

/**
 * @brief Assign a topic to be subscribed by the MCU
 * 
 * @param topicName Topic Name to be Subscribed
 * @param mType Message type
 * @param callback callback function that need to be called when new message is received to such a topic
 * @return yRosSerial_subHandle_t* 
 */
yRosSerial_subHandle_t* yRosSerial_subscribe(const char* topicName, yRosSerial_MessageType_t mType, Callback_t callback);

/**
 * @brief Publishing message to a topic 
 * 
 * @param hpub publisher handle
 * @param message message to be sent
 */
void yRosSerial_publish(yRosSerial_pubHandle_t* hpub, void* message);

/**
 * @brief Get the data that store on the Rx Ring buffer
 * 
 * @param buffer 
 */
void yRosSerial_getRxBuffer(uint8_t *buffer);

/**
 * @brief Get the data that store on the Tx Ring buffer
 * 
 * @param buffer 
 */
void yRosSerial_getTxBuffer(uint8_t *buffer);
/**
 * @brief the callback function that need to be put inside HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 * 
 * @param huart pointer uart handle
 * @param size the number of received data
 */
void yRosSerial_handleCompleteReceive(UART_HandleTypeDef *huart, uint16_t size);

/**
 * @brief the callback function that need to be put inside HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 * 
 * @param huart pointer to uart handle
 */
void yRosSerial_handleCompleteTransmit(UART_HandleTypeDef *huart);

/**
 * @brief Get the available data on Rx Ring Buffer
 * 
 * @return uint16_t 
 */
uint16_t yRosSerial_getRxCount();

/**
 * @brief Get the available data on Tx Ring Buffer
 * 
 * @return uint16_t 
 */
uint16_t yRosSerial_getTxCount();

/**
 * @brief Sending response from Client request of topic that is being subscribed and advertised (topic to publish) by MCU
 * 
 */
void responseTopic();
void check();
void transmitTest();

#endif /* __YROSSERIAL_H__ */
