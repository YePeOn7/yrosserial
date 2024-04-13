/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "yrosserial.h"
#include "stringCallback.h"
#include "float32Callback.h"
#include "float64Callback.h"
#include "odometry2dCallback.h"
#include "twist2dCallback.h"
#include "u8mCallback.h"
#include "s8mCallback.h"
#include "u8Callback.h"
#include "s8Callback.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */


uint8_t r[256] = { 0 };
uint8_t t[256] = { 0 };
RingBuffer_t rb;
int a = 0;

int cnt = 0;

// ---- data for subscriber ---- //
float f32;
double f64;
uint8_t subU8M[L_ARRAY];
int8_t subS8M[L_ARRAY];
uint16_t subU16M[L_ARRAY];
int16_t subS16M[L_ARRAY];
uint32_t subU32M[L_ARRAY];
int32_t subS32M[L_ARRAY];
uint64_t subU64M[L_ARRAY];
int64_t subS64M[L_ARRAY];

uint8_t subU8;
int8_t subS8;
uint16_t subU16;
int16_t subS16;
uint32_t subU32;
int32_t subS32;
uint64_t subU64;
int64_t subS64;

yRosSerial_odometry2d_t odometry2d;
yRosSerial_twist2d_t twist2d;

//extern RingBuffer_t *rx;
//extern RingBuffer_t *tx;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int dma = 0;
int dmaE = 0;
int dmaS = 0;
extern Rb2_t rbRx;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_UART_DeInit(&huart2);
	HAL_UART_Init(&huart2);
	yRosSerial_setting_t rosSerialSetting;
	rosSerialSetting.rxBufSize = 256;
	rosSerialSetting.txBufSize = 256;
//	rosSerialSetting.rx = r;
//	rosSerialSetting.tx = t;
	rosSerialSetting.huart = &huart2;
	rosSerialSetting.hdma_rx = &hdma_usart2_rx;
	yRosSerial_init(&rosSerialSetting);

	yRosSerial_pubHandle_t *pubTest1 = yRosSerial_advertise("/pub_string", MT_STRING);
	yRosSerial_pubHandle_t *pubTest4 = yRosSerial_advertise("/pub_odometry", MT_ODOMETRY2D);
	yRosSerial_pubHandle_t *pubTest5 = yRosSerial_advertise("/pub_twist", MT_TWIST2D);
	yRosSerial_pubHandle_t *pubU8M = yRosSerial_advertise("/pub_u8m", MT_UINT8_MULTIARRAY);
	yRosSerial_pubHandle_t *pubS8M = yRosSerial_advertise("/pub_s8m", MT_INT8_MULTIARRAY);
	yRosSerial_pubHandle_t *pubU16M = yRosSerial_advertise("/pub_u16m", MT_UINT16_MULTIARRAY);
	yRosSerial_pubHandle_t *pubS16M = yRosSerial_advertise("/pub_s16m", MT_INT16_MULTIARRAY);
	yRosSerial_pubHandle_t *pubU32M = yRosSerial_advertise("/pub_u32m", MT_UINT32_MULTIARRAY);
	yRosSerial_pubHandle_t *pubS32M = yRosSerial_advertise("/pub_s32m", MT_INT32_MULTIARRAY);
	yRosSerial_pubHandle_t *pubU64M = yRosSerial_advertise("/pub_u64m", MT_UINT64_MULTIARRAY);
	yRosSerial_pubHandle_t *pubS64M = yRosSerial_advertise("/pub_s64m", MT_INT64_MULTIARRAY);
	yRosSerial_pubHandle_t *pubF32M = yRosSerial_advertise("/pub_f32m", MT_FLOAT32_MULTIARRAY);
	yRosSerial_pubHandle_t *pubF64M = yRosSerial_advertise("/pub_f64m", MT_FLOAT64_MULTIARRAY);

	yRosSerial_pubHandle_t *pubU8 = yRosSerial_advertise("/pub_u8", MT_UINT8);
	yRosSerial_pubHandle_t *pubS8 = yRosSerial_advertise("/pub_s8", MT_INT8);
	yRosSerial_pubHandle_t *pubU16 = yRosSerial_advertise("/pub_u16", MT_UINT16);
	yRosSerial_pubHandle_t *pubS16 = yRosSerial_advertise("/pub_s16", MT_INT16);
	yRosSerial_pubHandle_t *pubU32 = yRosSerial_advertise("/pub_u32", MT_UINT32);
	yRosSerial_pubHandle_t *pubS32 = yRosSerial_advertise("/pub_s32", MT_INT32);
	yRosSerial_pubHandle_t *pubU64 = yRosSerial_advertise("/pub_u64", MT_UINT64);
	yRosSerial_pubHandle_t *pubS64 = yRosSerial_advertise("/pub_s64", MT_INT64);
	yRosSerial_pubHandle_t *pubF32 = yRosSerial_advertise("/pub_f32", MT_FLOAT32);
	yRosSerial_pubHandle_t *pubF64 = yRosSerial_advertise("/pub_f64", MT_FLOAT64);

//	yRosSerial_subscribe("/subString", MT_STRING, stringCallback);
//	yRosSerial_subscribe("/subFloat32", MT_FLOAT32, float32Callback);
//	yRosSerial_subscribe("/subFloat64", MT_FLOAT64, float64Callback);
//	yRosSerial_subscribe("/subOdometry", MT_ODOMETRY2D, odometry2dCallback);
//	yRosSerial_subscribe("/subTwist", MT_TWIST2D, twist2dCallback);

	yRosSerial_subscribe("/sub_u8m", MT_UINT8_MULTIARRAY, u8mCallback);
	yRosSerial_subscribe("/sub_s8m", MT_INT8_MULTIARRAY, s8mCallback);
	yRosSerial_subscribe("/sub_u8", MT_UINT8, u8Callback);
	yRosSerial_subscribe("/sub_s8", MT_INT8, s8Callback);

	char bufferMsg[256];

	// message that will be used to publish the data to the topics
	yRosSerial_string_t strMsg = { 0 };
	yRosSerial_odometry2d_t odometryMsg = { 0 };
	yRosSerial_twist2d_t twistMsg = { 0 };

	yRosSerial_uint8_t u8Msg = { 0 };
	yRosSerial_int8_t s8Msg = { 0 };
	yRosSerial_uint16_t u16Msg = { 0 };
	yRosSerial_int16_t s16Msg = { 0 };
	yRosSerial_uint32_t u32Msg = { 0 };
	yRosSerial_int32_t s32Msg = { 0 };
	yRosSerial_uint64_t u64Msg = { 0 };
	yRosSerial_int64_t s64Msg = { 0 };
	yRosSerial_float32_t f32Msg = { 0 };
	yRosSerial_float64_t f64Msg = { 0 };

	strMsg.data = bufferMsg;

	uint8_t u8mData[L_ARRAY];
	yRosSerial_uint8mul_t u8mMsg = { 0 };
	u8mMsg.data = u8mData;
	u8mMsg.length = L_ARRAY;

	int8_t s8mData[L_ARRAY];
	yRosSerial_int8mul_t s8mMsg = { 0 };
	s8mMsg.data = s8mData;
	s8mMsg.length = L_ARRAY;

	uint16_t u16mData[L_ARRAY];
	yRosSerial_uint16mul_t u16mMsg = { 0 };
	u16mMsg.data = u16mData;
	u16mMsg.length = L_ARRAY;

	int16_t s16mData[L_ARRAY];
	yRosSerial_int16mul_t s16mMsg = { 0 };
	s16mMsg.data = s16mData;
	s16mMsg.length = L_ARRAY;

	uint32_t u32mData[L_ARRAY];
	yRosSerial_uint32mul_t u32mMsg = { 0 };
	u32mMsg.data = u32mData;
	u32mMsg.length = L_ARRAY;

	int32_t s32mData[L_ARRAY];
	yRosSerial_int32mul_t s32mMsg = { 0 };
	s32mMsg.data = s32mData;
	s32mMsg.length = L_ARRAY;

	uint64_t u64mData[L_ARRAY];
	yRosSerial_uint64mul_t u64mMsg = { 0 };
	u64mMsg.data = u64mData;
	u64mMsg.length = L_ARRAY;

	int64_t s64mData[L_ARRAY];
	yRosSerial_int64mul_t s64mMsg = { 0 };
	s64mMsg.data = s64mData;
	s64mMsg.length = L_ARRAY;

	float f32mData[L_ARRAY];
	yRosSerial_float32mul_t f32mMsg = { 0 };
	f32mMsg.data = f32mData;
	f32mMsg.length = L_ARRAY;

	double f64mData[L_ARRAY];
	yRosSerial_float64mul_t f64mMsg = { 0 };
	f64mMsg.data = f64mData;
	f64mMsg.length = L_ARRAY;

	for(int i = 0; i < L_ARRAY; i++)
	{
		u8mData[i] = i;
		s8mData[i] = i;
		u16mData[i] = i;
		s16mData[i] = i;
		u32mData[i] = i;
		s32mData[i] = i;
		u64mData[i] = i;
		s64mData[i] = i;
		f32mData[i] = i;
		f64mData[i] = i;
	}

	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		sprintf(bufferMsg, "Hi.... Yoyoyoyo.... From Pub1 (%d)", cnt++);
		yRosSerial_publish(pubTest1, &strMsg);

		yRosSerial_publish(pubF32, &f32Msg);
		yRosSerial_publish(pubF64, &f64Msg);

		yRosSerial_publish(pubTest4, &odometryMsg);
		odometryMsg.x += 0.01;
		odometryMsg.y += 0.02;
		odometryMsg.w += 0.03;

		yRosSerial_publish(pubTest5, &twistMsg);
		twistMsg.x += 1;
		twistMsg.y += 2;
		twistMsg.w += 3;

		yRosSerial_publish(pubU8M, &u8mMsg); HAL_Delay(1);
		yRosSerial_publish(pubS8M, &s8mMsg);  HAL_Delay(1);
		yRosSerial_publish(pubU16M, &u16mMsg); HAL_Delay(1);
		yRosSerial_publish(pubS16M, &s16mMsg); HAL_Delay(1);
		yRosSerial_publish(pubU32M, &u32mMsg); HAL_Delay(1);
		yRosSerial_publish(pubS32M, &s32mMsg); HAL_Delay(1);
		yRosSerial_publish(pubU64M, &u64mMsg); HAL_Delay(1);
		yRosSerial_publish(pubS64M, &s64mMsg); HAL_Delay(1);
		yRosSerial_publish(pubF32M, &f32mMsg); HAL_Delay(1);
		yRosSerial_publish(pubF64M, &f64mMsg); HAL_Delay(1);

		yRosSerial_publish(pubU8, &u8Msg);
		yRosSerial_publish(pubS8, &s8Msg);
		yRosSerial_publish(pubU16, &u16Msg);
		yRosSerial_publish(pubS16, &s16Msg);
		yRosSerial_publish(pubU32, &u32Msg);
		yRosSerial_publish(pubS32, &s32Msg);
		yRosSerial_publish(pubU64, &u64Msg);
		yRosSerial_publish(pubS64, &s64Msg);

		for(int i = 0; i < L_ARRAY; i++)
		{
			u8mData[i]++;
			s8mData[i]--;
			u16mData[i]++;
			s16mData[i]--;
			u32mData[i]++;
			s32mData[i]--;
			u64mData[i]++;
			s64mData[i]--;
			f32mData[i] += 1;
			f64mData[i] -= 1;
		}

		u8Msg.data++;
		s8Msg.data--;
		u16Msg.data++;
		s16Msg.data--;
		u32Msg.data++;
		s32Msg.data--;
		u64Msg.data++;
		s64Msg.data--;
		f32Msg.data += 0.1;
		f64Msg.data -= 0.1;

		yRosSerial_getRxBuffer(r); //only for checking memory from Debug
//		yRosSerial_getTxBuffer(t); //only for checking memory from Debug
		a = rb2_getAvailable(&rbRx);
//		check();

		yRosSerial_spin();
//		dma = HAL_DMA_GetState(&hdma_usart2_rx);
//		while(cnt > 1013);
//		a = yRosSerial_getTxCount();
		dmaE = HAL_DMA_GetError(&hdma_usart2_rx);
		dmaS = HAL_UART_GetState(&huart2);
		HAL_Delay(100);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 1000000;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//	yRosSerial_handleCompleteReceive(huart, Size);
////	HAL_Delay(1);
//}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	yRosSerial_handleCompleteTransmit(huart);
}


int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
