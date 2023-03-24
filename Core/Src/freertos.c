/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include <at_test/at_tests.h>
#include <sensor_test/sensor_test.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "at_cmd.h"
#include "drv_uart/drv_uart.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ATProcess */
osThreadId_t ATProcessHandle;
const osThreadAttr_t ATProcess_attributes = {
  .name = "ATProcess",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for shell */
osThreadId_t shellHandle;
const osThreadAttr_t shell_attributes = {
  .name = "shell",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for board_io */
osThreadId_t board_ioHandle;
const osThreadAttr_t board_io_attributes = {
  .name = "board_io",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for at_receive */
osSemaphoreId_t at_receiveHandle;
const osSemaphoreAttr_t at_receive_attributes = {
  .name = "at_receive"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void ATProcessTask(void *argument);
void shellTask(void *argument);
void board_io_entry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)&rxbuf,RX_BUF_SIZE);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of at_receive */
  at_receiveHandle = osSemaphoreNew(1, 1, &at_receive_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ATProcess */
  ATProcessHandle = osThreadNew(ATProcessTask, NULL, &ATProcess_attributes);

  /* creation of shell */
  shellHandle = osThreadNew(shellTask, NULL, &shell_attributes);

  /* creation of board_io */
  board_ioHandle = osThreadNew(board_io_entry, NULL, &board_io_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */


  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ATProcessTask */
/**
* @brief Function implementing the ATProcess thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ATProcessTask */
void ATProcessTask(void *argument)
{
  /* USER CODE BEGIN ATProcessTask */

	//test
//	test();

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ATProcessTask */
}

/* USER CODE BEGIN Header_shellTask */
/**
* @brief Function implementing the shell thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_shellTask */
void shellTask(void *argument)
{
  /* USER CODE BEGIN shellTask */

  sensor_test();

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END shellTask */
}

/* USER CODE BEGIN Header_board_io_entry */
/**
* @brief Function implementing the board_io thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_board_io_entry */
void board_io_entry(void *argument)
{
  /* USER CODE BEGIN board_io_entry */
  /* Infinite loop */
//	int result = 0;
//	while(1){
//		result = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
//		if(result == 1){
//			LOG("%d\n",result);
//			osDelay(10);
//		}
//	}
	for(;;){
		osDelay(1);
	}
  /* USER CODE END board_io_entry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

