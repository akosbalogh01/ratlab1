/*
 * temperature.h
 *
 *  Created on: 2015 nov. 2
 *      Author: Szabó Zoltán
 */

#ifndef INCLUDES_TEMPERATURE_H_
#define INCLUDES_TEMPERATURE_H_

/* OS includes */
#include "FreeRTOS.h"
#include "semphr.h"

/* Public variables */

/* Public functions */
void TemperatureSetValue(uint16_t value);
uint16_t TemperatureGetValue();




#endif /* INCLUDES_TEMPERATURE_H_ */
