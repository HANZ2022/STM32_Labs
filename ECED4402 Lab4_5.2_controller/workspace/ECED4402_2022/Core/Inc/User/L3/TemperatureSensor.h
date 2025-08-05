/*
 * Tempurature.h
 *
 *  Created on: Nov 25, 2023
 *      Author: zh321
 */

#ifndef INC_USER_L3_TemperatureSensor_H_
#define INC_USER_L3_TemperatureSensor_H_

#include "FreeRTOS.h"
#include "timers.h"

void RunTemperatureSensor(TimerHandle_t xTimer);

#endif /* INC_USER_L3_DEPTHSENSOR_H_ */
