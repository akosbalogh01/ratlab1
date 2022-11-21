/*
 * bsp_led.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */


#include "bsp.h"

/*
 * Turn on the leds specified by the ledPos parameter.
 */
void LED_On(uint16_t ledPos)
{
	if (!( ledPos & LED_ALL ))
		assert_param(ledPos);

	HAL_GPIO_WritePin(GPIOD, ledPos, GPIO_PIN_SET);
}
/*
 * Turn off the leds specified by the ledPos parameter.
 */
void LED_Off(uint16_t ledPos)
{
	if (!( ledPos & LED_ALL ))
		assert_param(ledPos);

	HAL_GPIO_WritePin(GPIOD, ledPos, GPIO_PIN_RESET);
}

/*
 * Set the leds specified by the ledPos parameter to values specified by the ledVal parameter.
 */
void LED_SetValue(uint16_t ledPos, uint16_t ledVal)
{
	uint16_t odr;
	if (!( ledPos & LED_ALL ))
		assert_param(ledPos);

	ledVal &= ledPos;
	odr = GPIOD->ODR;
	odr &= ~ledPos;
	odr |= ledVal;
	GPIOD->ODR = odr;
}
