/*
 * PHSensor.h
 *
 *  Created on: Nov 25, 2023
 *      Author: zh321
 */

#ifndef INC_USER_L3_PHSENSOR_H_
#define INC_USER_L3_PHSENSOR_H_

#include "FreeRTOS.h"
#include "timers.h"

void RunPHSensor(TimerHandle_t xTimer);

#endif /* INC_USER_L3_PHSENSOR_H_ */
