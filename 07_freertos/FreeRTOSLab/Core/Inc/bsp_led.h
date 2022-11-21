/*
 * bsp_led.h
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */

#ifndef INC_BSP_LED_H_
#define INC_BSP_LED_H_

#include "bsp.h"

#define 	LED_GREEN	(uint16_t)1<<12
#define 	LED_RED		(uint16_t)1<<14
#define 	LED_BLUE		(uint16_t)1<<13
#define 	LED_YELLOW		(uint16_t)1<<15

#define		LED_ALL		(uint16_t)(LED_GREEN|LED_RED|LED_BLUE|LED_YELLOW)

void LED_On(uint16_t ledPos);
void LED_Off(uint16_t ledPos);
void LED_SetValue(uint16_t ledPos, uint16_t ledVal);

#endif /* INC_BSP_LED_H_ */
