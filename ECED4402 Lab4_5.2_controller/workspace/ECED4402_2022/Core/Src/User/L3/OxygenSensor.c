/*
 * TempuratureSensor.c
 *
 *  Created on: Nov 25, 2023
 *      Author: zh321
 */

#include <stdlib.h>
#include <User/L3/OxygenSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunOxygenSensor(TimerHandle_t xTimer) //Default 1000 ms
{
	const uint8_t variance = 40;
	const uint8_t mean = 51;

	send_sensorData_message(Oxygen, (rand() % variance) + mean);
}
