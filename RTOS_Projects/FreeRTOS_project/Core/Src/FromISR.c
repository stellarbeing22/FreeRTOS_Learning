/*
 * FromISR.c
 *
 *  Created on: Mar 10, 2026
 *      Author: stellarbeing22
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : FromISR.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include<stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define RED_LED GPIO_PIN_0
#define GREEN_LED GPIO_PIN_1
#define YELLOW_LED GPIO_PIN_4
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void red_led_Handler(void *parameters);
static void green_led_Handler(void *parameters);
static void yellow_led_Handler(void *parameters);


void btn_ISR_Hander(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	TaskHandle_t red_led;
	TaskHandle_t green_led;
	TaskHandle_t yellow_led;
	TaskHandle_t volatile next_task = NULL;
	BaseType_t status;
	TickType_t xLastWakeTimeRed;
	TickType_t xLastWakeTimeGreen;
	TickType_t xLastWakeTimeYellow;
	uint8_t Press_ctr = 0;


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
  /* USER CODE BEGIN 2 */
  HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );


  status = xTaskCreate(red_led_Handler, "red_led", 128, NULL, 2, &red_led);
  configASSERT(status == pdPASS);

  status = xTaskCreate(green_led_Handler, "green_led", 128, NULL, 2, &green_led);
  configASSERT(status == pdPASS);

  status = xTaskCreate(yellow_led_Handler, "yellow_handle", 128, NULL, 2, &yellow_led);
  configASSERT(status == pdPASS);

  (void)status;

  next_task = red_led;
  //Start the scheduler
  vTaskStartScheduler();

  //if control reaches this point, that means that launch of the scheduler has failed due to insufficient memory in the heap
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RED_LED|GREEN_LED|YELLOW_LED|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 LD2_Pin */
  GPIO_InitStruct.Pin = RED_LED|GREEN_LED|YELLOW_LED|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//
static void red_led_Handler(void *parameters)
{
	HAL_GPIO_WritePin(GPIOA, RED_LED, GPIO_PIN_RESET);
	TickType_t delay = pdMS_TO_TICKS(1000);
	uint32_t deleteTrue = 0;
	while(1)
	{
		xTaskNotifyWait(0, 0xFFFFFFFF, &deleteTrue, delay);
		if(deleteTrue)
		{
			taskENTER_CRITICAL();
			next_task = green_led;
			taskEXIT_CRITICAL();
			deleteTrue = 0;
			vTaskSuspend(NULL);
		}else
		{
		HAL_GPIO_TogglePin(GPIOA, RED_LED);
		}
	}
}

static void green_led_Handler(void *parameters)
{
	HAL_GPIO_WritePin(GPIOA, GREEN_LED, GPIO_PIN_RESET);
	TickType_t delay = pdMS_TO_TICKS(1000);
	uint32_t deleteTrue = 0;
	while(1)
	{
		xTaskNotifyWait(0, 0xFFFFFFFF, &deleteTrue, delay);
		if(deleteTrue)
		{
			taskENTER_CRITICAL();
			next_task = yellow_led;
			taskEXIT_CRITICAL();
			deleteTrue = 0;
			vTaskSuspend(NULL);
		}else
		{
		HAL_GPIO_TogglePin(GPIOA, GREEN_LED);
		}
	}
}

static void yellow_led_Handler(void *parameters)
{
	HAL_GPIO_WritePin(GPIOA, YELLOW_LED, GPIO_PIN_RESET);
	TickType_t delay = pdMS_TO_TICKS(1000);
	uint32_t deleteTrue = 0;
	while(1)
	{
		xTaskNotifyWait(0, 0xFFFFFFFF, &deleteTrue, delay);
		if(deleteTrue)
		{
			taskENTER_CRITICAL();
			next_task = red_led;
			taskEXIT_CRITICAL();
			deleteTrue = 0;
			vTaskSuspend(NULL);
		}else
		{
		HAL_GPIO_TogglePin(GPIOA, YELLOW_LED);
		}
	}
}

void btn_ISR_Hander(void)
{
	BaseType_t xHighestPriorityTaskWoken = pdFALSE;
	Press_ctr++;

	if(Press_ctr > 3)
	{
		Press_ctr = 0;
		xTaskResumeFromISR(red_led);
		xTaskResumeFromISR(green_led);
		xTaskResumeFromISR(yellow_led);
	}else
	{
		xTaskNotifyFromISR(next_task,0x1,eSetBits,&xHighestPriorityTaskWoken);
	}
	portYIELD_FROM_ISR(xHighestPriorityTaskWoken);
}

void EXTI15_10_IRQHandler()
{
	btn_ISR_Hander();
	HAL_GPIO_EXTI_IRQHandler(B1_Pin); //clear pin 13  (EXTI15_10) pending bit
}



/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
/*
 * LED.c
 *
 *  Created on: Mar 10, 2026
 *      Author: stellarbeing22
 */


