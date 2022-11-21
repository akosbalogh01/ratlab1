/*
 * bsp_adc.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */
#include "bsp.h"
extern ADC_HandleTypeDef hadc2;
void ADC_Init()
{
	HAL_ADC_Start(&hadc2);
}
uint8_t GetADCValue()
{
	return HAL_ADC_GetValue(&hadc2)>>6;
}
