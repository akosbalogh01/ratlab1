/*
 * bsp_tempsensor.c
 *
 *  Created on: Sep 16, 2020
 *      Author: zszabo
 */

#include <bsp.h>
extern I2C_HandleTypeDef hi2c1;
/* Private Definitions */
#define I2C_TIMEOUT ((uint16_t) 0xFFFF)

uint8_t TempSensorAddr = 0x30;
uint8_t TempSensorConfRegAddr = 0x01;
uint8_t TempSensorDataRegAddr = 0x05;
uint8_t TempSensorResRegAddr = 0x08;

I2C_HandleTypeDef I2CHandle;

void TempSensorInit()
{
	uint8_t data = 0;
	HAL_I2C_Mem_Write(&hi2c1,TempSensorAddr, TempSensorResRegAddr, 1, &data, 1, I2C_TIMEOUT);
}

/* Read the temperature */
uint8_t GetTempSensorValue()
{
	uint8_t dataBuff[2];
	uint8_t temperature = 0x00;

	HAL_I2C_Mem_Read(&hi2c1, TempSensorAddr, TempSensorDataRegAddr,1,dataBuff,2, I2C_TIMEOUT);

	dataBuff[0] = dataBuff[0] & 0x1F;
	if ((dataBuff[0] & 0x10) == 0x10){	//TA<0°C
		dataBuff[0] = dataBuff[0] & 0x0F;
		temperature = 256 - ((dataBuff[0]<<4) | (dataBuff[1]>>4));
	}else{	//TA>0°C
		temperature = (dataBuff[0]<<4) | (dataBuff[1]>>4);
	}
	return temperature;
}
