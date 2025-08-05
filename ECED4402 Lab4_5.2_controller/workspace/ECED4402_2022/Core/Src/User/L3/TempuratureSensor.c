/*
 * TempuratureSensor.c
 *
 *  Created on: Nov 25, 2023
 *      Author: zh321
 */


#include <stdlib.h>
#include <User/L3/TemperatureSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"


void RunTemperatureSensor(TimerHandle_t xTimer) //Default 1000 ms
{
	const uint8_t variance = 36;
	const uint8_t mean = 0;

	send_sensorData_message(Temperature, (rand() % variance) + mean);
}
