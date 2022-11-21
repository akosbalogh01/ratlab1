/*
 * tasks.c
 *
 *  Created on: 2015 m�j. 5
 *      Author: Zoltan Szabo
 *  Application logic
 *
 */

#include <bsp.h>
#include <tasks.h>

#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "temperature.h"
#include "display.h"
#include "limit.h"
#include "event_groups.h"
/* Macros */

/* Global Variables */

void Task_Main(void* param)
{
	(void)param; /* Suppress unused parameter warning */
	/* Feladat 0. */
	uint16_t time=0;

	while(1)
	{
		DisplaySetValue(time);
		time++;
		vTaskDelay(100);
	}
}

void Task_TempGet(void* param)
{
	(void)param; /* Suppress unused parameter warning */
	/* Feladat 3. */
	uint16_t temp=0;
	uint16_t limit=0;

	while(1)
	{
		limit=GetADCValue();
		temp=GetTempSensorValue();
		if(temp>limit){
			xEventGroupSetBits(limitFlag, 1);
		}
		else{
			xEventGroupClearBits(limitFlag, 1);
		}
		LimitSetValue(limit);
		TemperatureSetValue(temp);
		vTaskDelay(100);
	}
}

void Task_TempWrite(void* param)
{
	(void)param; /* Suppress unused parameter warning */
	/* Feladat 3. */
	uint16_t temp=0;
	uint16_t limit=0;
	char data[100]={0};
	while(1)
	{
		if(xEventGroupGetBits(limitFlag)){
			LED_On(LED_RED);
			LED_Off(LED_GREEN);
		}
		else{
			LED_On(LED_GREEN);
			LED_Off(LED_RED);
		}
		temp=TemperatureGetValue();
		limit=LimitGetValue();
		if(GetDisplaySwitchState()){
			DisplaySetValue(temp);
		}
		else{
			DisplaySetValue(limit);
		}
		sprintf(data, "Limit=%d\tTemperature=%d\r\n", limit, temp);
		UARTSendString(data);
		vTaskDelay(100);
	}
}

