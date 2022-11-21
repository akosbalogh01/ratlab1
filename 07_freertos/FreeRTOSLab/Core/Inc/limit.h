/*
 * limit.h
 *
 *  Created on: 2015 nov. 2
 *      Author: zszabo
 */

#ifndef INCLUDES_LIMIT_H_
#define INCLUDES_LIMIT_H_

/* OS includes */
#include "FreeRTOS.h"
#include "semphr.h"

/* Public variables */

/* Public functions */
void LimitSetValue(uint16_t value);
uint16_t LimitGetValue();



#endif /* INCLUDES_LIMIT_H_ */
