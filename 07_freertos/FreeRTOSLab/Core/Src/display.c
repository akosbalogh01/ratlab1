/*
 * display.c
 *
 *  Created on: 2015 nov. 2
 *      Author: zszabo
 */

/* BSP includes */
#include "bsp.h"

/* Public interface */
#include "display.h"

/* Public variables */

/* Private display value variable */
/* The purpose of the functions below is to avoid concurrent access of the global resources */
uint16_t displayValue;

/* Store the display value */
void SetDisplayValue(uint16_t value)
{
	displayValue=value;
}

/* Read the display value */
uint16_t GetDisplayValue()
{
	uint16_t temp=0;
	temp=displayValue;
	return temp;
}

/* Sets the *pValue according to the value to display,
* if the *pValue has been modified the return value should be set to
nonzero, otherwise 0 */
uint8_t GetDisplayValueCallback(uint16_t *pValue)
{
	if(*pValue!=displayValue){
		*pValue=displayValue;
		return 1;
	}
	
	return 0;
}
