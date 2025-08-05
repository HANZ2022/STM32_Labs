/*
 * main_user.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 */

#include <stdio.h>

//STM32 generated header files
#include "main.h"

//User generated header files
#include "User/main_user.h"
#include "User/util.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "task.h"

char main_string[50];
uint32_t main_counter = 0;
uint32_t IdleCycleCount=0;
char IdleString[50];

/*************************************************************
Function that will be called as a task.
Depending on the parameter value, the task will accomplish different
functionality.
**************************************************************/


static void PrintStringTask(void *params){
	int *type = (int *)params;
	switch (*type){
	case 0:
		do{
			print_str("This task is created with ID 0\r\n");
			//for (int i = 0; i<5000000;i++);
			vTaskDelay(1000/portTICK_RATE_MS);
		}while(1);

	case 1:
		do{
			print_str("This task is created with ID 1\r\n");
			//for (int i = 0; i<5000000;i++);
			vTaskDelay(1000/portTICK_RATE_MS);
		}while(1);

	case 2:
		do{
			print_str("This task is created with ID 2\r\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}while(1);

	}
}



/**************************************************
This hook function does nothing, but increment a counter.
***************************************************/
void vApplicationIdleHook(void){
	IdleCycleCount++;
}
/***********************************************
This function contains an infinite loop, such that it can be called as a
task.
It prints the number of tick counts used by the idle tasks since the
start of the program.
*************************************************/
static void TaskPrintIdleCycleCount(void *params){
	do{
		// the format specifier forces 8 charaters for the unsigned integer.
		sprintf(IdleString,"Idle Cycle Count: 0x%08lx\r\n",IdleCycleCount);
		print_str(IdleString);
		IdleCycleCount = 0;
		vTaskDelay(1000/ portTICK_RATE_MS);
	}while(1);
}






void main_user(){
	util_init();

	static int param0 = 0;
	static int param1 = 1;
	static int param2 = 2;
	xTaskCreate(PrintStringTask,"Task1", configMINIMAL_STACK_SIZE + 100, &param0, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(PrintStringTask,"Task2", configMINIMAL_STACK_SIZE + 100, &param1, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(PrintStringTask,"Task3", configMINIMAL_STACK_SIZE + 100, &param2, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(TaskPrintIdleCycleCount,"Task Idle", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while(1);

}
