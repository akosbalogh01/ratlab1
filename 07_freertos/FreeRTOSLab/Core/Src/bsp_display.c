/*
 * bsp_display.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */

#include <bsp.h>

extern TIM_HandleTypeDef htim4;
uint8_t currentDigit = 0;
uint16_t displayValue = 0;

void DisplayDigit( uint8_t digit, uint8_t value, uint8_t dp);

void Display_Init()
{
	HAL_TIM_Base_Start_IT(&htim4);
}

/* Callback for the display timer IRQ handler*/
void TIM4_PeriodElapsedCallback()
{
	uint16_t value;
	uint8_t digitValue = 0;
	switch (currentDigit)
	{
	case 0:
		if (GetDisplayValueCallback(&value))
		{
			displayValue = value;
		}
		digitValue = displayValue / 1000;
		break;
	case 1:
		digitValue = (displayValue % 1000) / 100;
		break;
	case 2:
		digitValue = (displayValue % 100) / 10;
		break;
	case 3:
		digitValue = displayValue % 10;
		break;
	}
	DisplayDigit( currentDigit, digitValue, 0);
	currentDigit++;
	currentDigit &= 3;
}

const uint8_t SegmentTable[16] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };

/* Display the value on the selected 7 segment digit */
void DisplayDigit( uint8_t digit, uint8_t value, uint8_t dp  )
{
	uint16_t odr;
	if ( value > 0x0f)
		assert_param(value);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (digit & 1) != 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, ((digit>>1) & 1) != 0);

	odr = GPIOE->ODR & 0x00ff;
	odr |= (((uint16_t)( SegmentTable[value] + (dp ? 0x80 : 0x00) ))<<8);
	GPIOE->ODR = odr;

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
}

/* Get the value to display */
/* Returns nonzero if a valid value have been written into pValue parameter. */
__weak uint8_t GetDisplayValueCallback(uint16_t *pValue)
{
	return 0;
}
