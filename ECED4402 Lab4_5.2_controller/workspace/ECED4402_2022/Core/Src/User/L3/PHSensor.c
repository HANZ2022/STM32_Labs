/*
 * TempuratureSensor.c
 *
 *  Created on: Nov 25, 2023
 *      Author: zh321
 */

#include <stdlib.h>
#include <User/L3/PHSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"



/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunPHSensor(TimerHandle_t xTimer) //Default 1000 ms
{
	const uint8_t variance = 10;
	const uint8_t mean = 76;

	send_sensorData_message(PH, (rand() % variance) + mean);
}
