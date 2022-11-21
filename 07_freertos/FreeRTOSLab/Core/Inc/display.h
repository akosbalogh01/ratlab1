/*
 * display.h
 *
 *  Created on: 2015 nov. 2
 *      Author: zszabo
 */

#ifndef INCLUDES_DISPLAY_H_
#define INCLUDES_DISPLAY_H_

/* OS includes */
#include "FreeRTOS.h"
#include "semphr.h"

/* Public variables */

/* Public functions */
void DisplaySetValue(uint16_t value);
uint16_t DisplayGetValue();

#endif /* INCLUDES_DISPLAY_H_ */
