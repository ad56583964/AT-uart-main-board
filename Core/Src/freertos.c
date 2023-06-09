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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "at_cmd.h"
#include "drv_uart/drv_uart.h"
#include "button/button.h"
#include "main_app.h"
#include "ssd1306/ssd1306.h"
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

AT_mode_t g_AT_mode = IDLE;

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
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for shell */
osThreadId_t shellHandle;
const osThreadAttr_t shell_attributes = {
  .name = "shell",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for display */
osThreadId_t displayHandle;
const osThreadAttr_t display_attributes = {
  .name = "display",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ButtonProcessTimer */
osTimerId_t ButtonProcessTimerHandle;
const osTimerAttr_t ButtonProcessTimer_attributes = {
  .name = "ButtonProcessTimer"
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
void displaytask(void *argument);
void ButtonProcessTimerCallback(void *argument);

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

  /* Create the timer(s) */
  /* creation of ButtonProcessTimer */
  ButtonProcessTimerHandle = osTimerNew(ButtonProcessTimerCallback, osTimerPeriodic, NULL, &ButtonProcessTimer_attributes);

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

  /* creation of display */
  displayHandle = osThreadNew(displaytask, NULL, &display_attributes);

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
  osTimerStart(ButtonProcessTimerHandle, 20);

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

	AT_Init();
	AT_check_addr();

  /* Infinite loop */

	for(;;)
	{
		if(g_AT_mode == REG){
			reg_process();
		}
		else if(g_AT_mode == POLLING){
			polling_process();
		}
		else if(g_AT_mode == IDLE){
			LOG("Idle\n");
			osDelay(1000);
		}
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
//  sensor_test();

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END shellTask */
}

/* USER CODE BEGIN Header_displaytask */
/**
* @brief Function implementing the display thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_displaytask */
void displaytask(void *argument)
{
  /* USER CODE BEGIN displaytask */
	ssd1306_Init();
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
//	device_show_test();
  /* Infinite loop */
  for(;;)
  {
    ssd1306_Fill(White);
    oledshow();
    ssd1306_UpdateScreen();
    osDelay(233);
  }
  /* USER CODE END displaytask */
}

/* ButtonProcessTimerCallback function */
void ButtonProcessTimerCallback(void *argument)
{
  /* USER CODE BEGIN ButtonProcessTimerCallback */
	ButtonCallback();

  /* USER CODE END ButtonProcessTimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

