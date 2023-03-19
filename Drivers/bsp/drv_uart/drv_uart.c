/*
 * drv_uart.c
 *
 *  Created on: Mar 13, 2023
 *      Author: ad565
 */

#include "drv_uart.h"
#include "stdio.h"


char rxbuf[RX_BUF_SIZE]={0};
uint8_t received = 0U;

inline int _uart1_write(uint8_t *pData, uint16_t Size){
	return HAL_UART_Transmit(&huart1, (uint8_t*)pData, Size, 1000);
}

inline int _uart2_write(uint8_t *pData, uint16_t Size){
	return HAL_UART_Transmit(&huart2, (uint8_t*)pData, Size, 1000);
}

