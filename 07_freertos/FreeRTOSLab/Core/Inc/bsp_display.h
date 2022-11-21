/*
 * bsp_display.h
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */

#ifndef INC_BSP_DISPLAY_H_
#define INC_BSP_DISPLAY_H_

void Display_Init();
uint8_t GetDisplayValueCallback(uint16_t *pValue);
void TIM4_PeriodElapsedCallback();
#endif /* INC_BSP_DISPLAY_H_ */
