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

AT_mode_t AT_mode = IDLE;

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
  osTimerStart(ButtonProcessTimerHandle, 10);

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

#define PACK_LENGTH 15

void process_reg(){
	osDelay(1000);
	LOG("Need Pair\n");
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) != 1);
	AT_Request_Set_t pack;
	AT_Receive_Read_t get_pack;
	pack.addr = 0x0001;
	pack.data = EDGE_IR;
	pack.type = REG_DEVICE;
	LOG("Send Request\n");
	AT_request(&pack,&get_pack);
	if(get_pack.type == MAIN_ACK){
		pack.addr = 0x0001;
		pack.data = 0;
		pack.type = EDGE_ACK;
		AT_request_send_pack(&pack);
	}
	LOG("Requested\n");
}

void main_loop(){

}
/* USER CODE END Header_ATProcessTask */
void ATProcessTask(void *argument)
{
  /* USER CODE BEGIN ATProcessTask */

	AT_Init();
	AT_check_addr();
	if(AT_device_mode != EDGE_DEVICE){
		ERROR;
	}
//	char my_addr =
//	wait_ack();
//	edge_register();
	for(;;)
	{
		process_reg();
		LOG("WaitingCmd");
		main_loop();
		while(1);
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

