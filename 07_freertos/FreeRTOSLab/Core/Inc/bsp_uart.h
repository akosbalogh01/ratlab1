/*
 * bsp_uart.h
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */

#ifndef INC_BSP_UART_H_
#define INC_BSP_UART_H_

void UART_Init();
void UARTSendString(char* data);
void UART3_RxCpltCallback();

#endif /* INC_BSP_UART_H_ */
