/*
* SensorController.c
*
* Created on: Oct 24, 2022
* Author: kadh1
*/

#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "User/L2/Comm_Datalink.h"
#include "User/L3/PHSensor.h"
#include "User/L3/DepthSensor.h"
#include "User/L3/TemperatureSensor.h"
#include "User/L3/OxygenSensor.h"
#include "User/L4/SensorPlatform.h"
#include "User/L4/SensorController.h"
#include "User/util.h"

// Required FreeRTOS header files
#include "FreeRTOS.h"
#include "Timers.h"
#include "semphr.h"

QueueHandle_t Queue_Sensor_Data;
QueueHandle_t Queue_HostPC_Data;
bool IsRemotePlatformEnabled = false;
struct SensorStates sensorState = {0};
static void ResetMessageStruct(struct CommMessage* currentRxMessage) {
    static const struct CommMessage EmptyMessage = {0};
    *currentRxMessage = EmptyMessage;
}

/******************************************************************************
 This task is created from the main.
 ******************************************************************************/
void SensorControllerTask(void *params) {
    enum HostPCCommands HostPCCommand = PC_Command_NONE;
    struct CommMessage currentRxMessage = {0};
    //bool IsRemotePlatformEnabled = false;

    do {
        // Check if there is any received Sensor Data
        if (xQueueReceive(Queue_Sensor_Data, &currentRxMessage, 0) == pdPASS) {
            switch (currentRxMessage.SensorID) {
                case Controller:
                    switch (currentRxMessage.messageId) {
                        case 0: // Do Nothing
                            break;
                        case 1: // Received Acknowledgment
                            sensorState.IsPHAck = false;
                            sensorState.IsDepthAck = false;
                            sensorState.IsTemperatureAck = false;
							sensorState.IsOxygenAck = false;
                            break;
                        case 3: // Do Nothing
                            break;
                    }
                    break;


                case PH:
                    switch (currentRxMessage.messageId) {
                        case 0: // Do Nothing
                            break;
                        case 1: // Received Acknowledgment
                            sensorState.IsPHAck = true;
                            break;
                        case 3: // Received PH Data
                            sensorState.PHData = currentRxMessage.params;
                            break;
                    }
                    break;
                case Depth:
                    switch (currentRxMessage.messageId) {
                        case 0: // Do Nothing
                            break;
                        case 1: // Received Acknowledgment
                            sensorState.IsDepthAck = true;
                            break;
                        case 3: // Received Depth Data
                            sensorState.DepthData = currentRxMessage.params;
                            break;
                    }
                    break;

				case Temperature:
					switch (currentRxMessage.messageId) {
						case 0: // Do Nothing
							break;
						case 1: // Received Acknowledgment
							sensorState.IsTemperatureAck = true;
							break;
						case 3: // Received Temperature Data
							sensorState.TemperatureData = currentRxMessage.params;
							break;
					}
					break;

				case Oxygen:
					switch (currentRxMessage.messageId) {
						case 0: // Do Nothing
							break;
						case 1: // Received Acknowledgment
							sensorState.IsOxygenAck = true;
							break;
						case 3: // Received Oxygen Data
							sensorState.OxygenData = currentRxMessage.params;
							break;
					}
					break;
                default: // Should not get here
                    ResetMessageStruct(&currentRxMessage);
            }
        }


        // Check if there is any received Host PC Command Data
        if (xQueueReceive(Queue_HostPC_Data, &HostPCCommand, 0) == pdPASS) {
            switch (HostPCCommand) {
                case PC_Command_START:
                    //print_str("Sending Start Command\r\n");
                    send_sensorEnable_message(PH, 5000);
                    send_sensorEnable_message(Depth, 5000);
                    send_sensorEnable_message(Temperature, 5000);
					send_sensorEnable_message(Oxygen, 5000);
                    IsRemotePlatformEnabled = true;
                    break;
                case PC_Command_RESET:
                    //print_str("Sending Reset Command\r\n");
                    send_sensorReset_message();
                    IsRemotePlatformEnabled = false;
                    break;
                default:
                    break;
            }
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    } while (1);
}

/*
* This task reads the queue of characters from the Sensor Platform when available
* It then sends the processed data to the Sensor Controller Task
*/
void SensorPlatform_RX_Task() {
    struct CommMessage currentRxMessage = {0};
    Queue_Sensor_Data = xQueueCreate(80, sizeof(struct CommMessage));

    request_sensor_read(); // requests a usart read (through the callback)

    while (1) {
        parse_sensor_message(&currentRxMessage);

        if (currentRxMessage.IsMessageReady == true && currentRxMessage.IsCheckSumValid == true) {
            xQueueSendToBack(Queue_Sensor_Data, &currentRxMessage, 0);
            ResetMessageStruct(&currentRxMessage);
        }
    }
}

/*
* This task reads the queue of characters from the Host PC when available
* It then sends the processed data to the Sensor Controller Task
*/
void HostPC_RX_Task() {
    enum HostPCCommands HostPCCommand = PC_Command_NONE;

    Queue_HostPC_Data = xQueueCreate(80, sizeof(enum HostPCCommands));

    request_hostPC_read();

    while (1) {
        HostPCCommand = parse_hostPC_message();

        if (HostPCCommand != PC_Command_NONE) {
            xQueueSendToBack(Queue_HostPC_Data, &HostPCCommand, 0);
        }
    }
}





// This task is responsible for compressing and sending sensor data.
void Compress_Task()
{
    char Msg[4]; // Buffer to hold the message to be sent.
    while (1) // Infinite loop to continuously check and send data.
    {
        // Check if the remote platform is enabled for data transmission.
        if (IsRemotePlatformEnabled == true) {

            // Check if there is an acknowledgment for depth data.
            if (sensorState.IsDepthAck == true) {
                // Prepare the message with depth sensor ID and data.
                sprintf(Msg, "%c%c",252,sensorState.DepthData);
                print_str(Msg); // Send the message.
            } else {
                // Prepare the message with depth sensor ID and no data indication.
                sprintf(Msg, "%c%c",252,251);
                print_str(Msg); // Send the message.
            }

            // Check if there is an acknowledgment for oxygen data.
            if (sensorState.IsOxygenAck == true)
            {
                // Prepare the message with oxygen sensor ID and data.
                sprintf(Msg, "%c%c",253, sensorState.OxygenData);
                print_str(Msg); // Send the message.

            } else {
                // Prepare the message with oxygen sensor ID and no data indication.
                sprintf(Msg, "%c%c",253, 251);
                print_str(Msg); // Send the message.
            }

            // Check if there is an acknowledgment for temperature data.
            if (sensorState.IsTemperatureAck == true)
            {
                // Prepare the message with temperature sensor ID and data.
                sprintf(Msg, "%c%c",254, sensorState.TemperatureData);
                print_str(Msg); // Send the message.
            } else{
                // Prepare the message with temperature sensor ID and no data indication.
                sprintf(Msg, "%c%c",254, 251);
                print_str(Msg); // Send the message.
            }

            // Check if there is an acknowledgment for pH data.
            if (sensorState.IsPHAck == true) {
                // Prepare the message with pH sensor ID and data.
                sprintf(Msg, "%c%c",255, sensorState.PHData);
                print_str(Msg); // Send the message.
            } else {
                // Prepare the message with pH sensor ID and no data indication.
                sprintf(Msg, "%c%c",255, 251);
                print_str(Msg); // Send the message.
            }

            // Delay the task for a second to reduce CPU usage.
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
}

