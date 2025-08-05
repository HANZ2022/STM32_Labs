/*
 * SensorController.h
 *
 *  Created on: Oct 24, 2022
 *      Author: kadh1
 */

#ifndef INC_USER_L4_SENSORCONTROLLER_H_
#define INC_USER_L4_SENSORCONTROLLER_H_


struct SensorStates{
	bool IsPHAck;
	bool IsDepthAck;
	bool IsTemperatureAck;
	bool IsOxygenAck;
	uint8_t DepthData;
	uint8_t PHData;
	uint8_t TemperatureData;
	uint8_t OxygenData;
};
void HostPC_RX_Task();
void SensorPlatform_RX_Task();
void SensorControllerTask(void *params);
void Compress_Task();
#endif /* INC_USER_L4_SENSORCONTROLLER_H_ */
