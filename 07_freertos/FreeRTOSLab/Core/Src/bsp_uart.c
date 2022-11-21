/*
 * bsp_uart.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */
#include <bsp.h>

extern UART_HandleTypeDef huart3;

uint8_t rxBuff[1];

void UART_Init()
{
	HAL_UART_Receive_IT(&huart3,rxBuff, sizeof(rxBuff));
}

void UARTSendString(char* data)
{
	uint16_t len = 0;
	char* ldata = data;
	while (*ldata)
	{
		len++;
		ldata++;
	}

	HAL_UART_Transmit(&huart3, (uint8_t*)data, len, 1000);
}

void UART3_RxCpltCallback()
{
	HAL_UART_Receive_IT(&huart3,rxBuff, sizeof(rxBuff));
}
