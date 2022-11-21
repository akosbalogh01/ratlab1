/*
 * temperature.c
 *
 *  Created on: 2015 nov. 2
 *      Author: zszabo
 */

/* BSP includes */
#include "bsp.h"

/* Public interface */
#include "temperature.h"

/* Public variables */
SemaphoreHandle_t tempSem;

/* Private temperature value variable */
uint16_t tempVal;

/* The purpose of the functions below is to avoid concurrent access of the global resources */

/* Store the temperature value */
void TemperatureSetValue(uint16_t value)
{
	/* Take the semaphore */
	int err=xSemaphoreTake(tempSem, portMAX_DELAY);
	
	/* Set the value of the private variable */
	if(err==pdTRUE){
		tempVal=value;
		
		/* Give the semaphore */
		xSemaphoreGive(tempSem);
	}
}

/* Read the temperature value */
uint16_t TemperatureGetValue()
{
	uint16_t retVal=0;
	
	/* Take the semaphore */
	int err=xSemaphoreTake(tempSem, portMAX_DELAY);
	if(err==pdTRUE){
		/* Save the value of private variable */
		retVal=tempVal;
		
		/* Give the semaphore */
		xSemaphoreGive(tempSem);
	}
	
	/* Return the safely copied value */
	return retVal;
}



