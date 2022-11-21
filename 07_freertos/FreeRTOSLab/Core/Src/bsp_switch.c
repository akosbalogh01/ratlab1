/*
 * bsp_switch.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */
#include "bsp.h"
/* Read the display switch state */
uint8_t GetDisplaySwitchState()
{
	return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2);
}

/* Read the mode selector switch state */
uint8_t GetModeSwitchState()
{
	return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4);
}
