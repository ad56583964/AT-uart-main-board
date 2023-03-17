/*
 * drv_uart.h
 *
 *  Created on: Mar 13, 2023
 *      Author: ad565
 */

#ifndef BSP_DRV_UART_H_
#define BSP_DRV_UART_H_

#include "usart.h"

#define RX_BUF_SIZE 20

extern char rxbuf[RX_BUF_SIZE];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

#define uart1_write(str) _uart1_write((uint8_t*)&str,(uint16_t)sizeof(str))
#define uart2_write(str) _uart2_write((uint8_t*)&str,(uint16_t)sizeof(str))

int _uart1_write(uint8_t *pData, uint16_t Size);

int _uart2_write(uint8_t *pData, uint16_t Size);

int EVENT_Test();



#endif /* BSP_DRV_UART_H_ */
