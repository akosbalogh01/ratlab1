/*
 * limit.c
 *
 *  Created on: 2015 nov. 2
 *      Author: zszabo
 */

/* BSP includes */
#include "bsp.h"

/* Public interface */
#include "limit.h"

/* Public variables */
SemaphoreHandle_t limitSem;
/* Private limit value variable */
uint16_t limitVal;
/* The purpose of the functions below is to avoid concurrent access of the global resources */

/* Store the limit value */
void LimitSetValue(uint16_t value)
{
	int err=xSemaphoreTake(limitSem, portMAX_DELAY);
	if(err==pdTRUE){
		limitVal=value;
		xSemaphoreGive(limitSem);
	}
}

/* Read the limit value */
uint16_t LimitGetValue()
{
	uint16_t value=0;
	int err=xSemaphoreTake(limitSem, portMAX_DELAY);
	if(err==pdTRUE){
		value=limitVal;
		xSemaphoreGive(limitSem);
	}
	return value;
}

