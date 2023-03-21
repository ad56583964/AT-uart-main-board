/*
 * drv_uart.c
 *
 *  Created on: Mar 13, 2023
 *      Author: ad565
 */

#include "drv_uart.h"
#include "stdio.h"
#include <stdarg.h>


char rxbuf[RX_BUF_SIZE]={0};
uint8_t received = 0U;

inline int _uart1_write(uint8_t *pData, uint16_t Size){
	return HAL_UART_Transmit(&huart1, (uint8_t*)pData, Size, 1000);
}

inline int _uart2_write(uint8_t *pData, uint16_t Size){
	return HAL_UART_Transmit(&huart2, (uint8_t*)pData, Size, 1000);
}

int8_t UART1_printf(char *fmt, ...)
{
//引用https://blog.csdn.net/windeal3203/article/details/39209003

    va_list ap;
    int len = 0;
    static char buf[64];
    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    _uart1_write((uint8_t*)buf, len);
    return 0;
}
