/**
  ******************************************************************************
  * @file           : sensor_manager.c
  * @brief          : Sensor manager implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "managers/sensor_manager.h"
#include "cmsis_os.h"
#include "managers/pwm_manager.h"

/* Public Variables ----------------------------------------------------------*/
Sensor_t SensorList[MAX_NUM_SENSORS] = {0};

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Main sensor manager task
  * @param  argument: Not used
  * @retval None
  */
void Sensor_ManagerTask(void *argument)
{
	uint32_t last_update_tick = 0;
	uint32_t last_can_send_tick = 0;
	uint32_t current_tick = 0;

	for (int i = 0; i < NUM_SENSORS; i++) {
		Sensor_Init(&SensorList[i]);
	}

	// Initialize timestamps
	last_update_tick = osKernelGetTickCount();
	last_can_send_tick = osKernelGetTickCount();

	/* Infinite loop */
	for(;;)
	{
		current_tick = osKernelGetTickCount();

		if (current_tick - last_update_tick >= SENSOR_UPDATE_INTERVAL_MS) {
			for (int i = 0; i < NUM_SENSORS; i++) {
				Sensor_Update(&SensorList[i]);
			}
			last_update_tick = current_tick;
		}

		if (current_tick - last_can_send_tick >= SENSOR_CAN_SEND_INTERVAL_MS) {
			for (int i = 0; i < NUM_SENSORS; i++) {
				Sensor_SendCAN(&SensorList[i]);
			}
			last_can_send_tick = current_tick;
		}

		osDelay(10);
	}
}

/**
  * @brief  Initialize sensor data based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_Init(Sensor_t* sensor)
{
	switch (sensor->type) {
		case COOLANT_TEMP:
			CoolantTemp_Init(&sensor->ct_data);
			break;
		case LINEAR_POT:
			LinearPot_Init(&sensor->lp_data);
			break;
		case PITOT_TUBE:
			PitotTube_Init(&sensor->pitot_data);
			break;
		case STEERING_ANGLE:
			SteeringAngle_Init(&sensor->sa_data);
			break;
		case TACH:
			Tach_Init(&sensor->tach_data);
			break;
		case WHEEL_SPEED:
			WheelSpeed_Init(&sensor->wsp_data);
			break;
		case ODO:
			ODO_Init(&sensor->odo_data);
			break;
	}

}

/**
  * @brief  Update sensor data based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_Update(Sensor_t* sensor)
{
	switch (sensor->type) {
		case COOLANT_TEMP:
			CoolantTemp_Update(&sensor->ct_data, sensor->adc_channel);
			if (&sensor->ct_data.temp == INV_TEMP_PTR) {
				PWM_SetInvTemp(sensor->ct_data.temp);
			}
			break;
		case LINEAR_POT:
			LinearPot_Update(&sensor->lp_data, sensor->adc_channel);
			break;
		case PITOT_TUBE:
			PitotTube_Update(&sensor->pitot_data, sensor->adc_channel);
			break;
		case STEERING_ANGLE:
			SteeringAngle_Update(&sensor->sa_data, sensor->adc_channel);
			break;
		case TACH:
			Tach_Update(&sensor->tach_data);
			break;
		case WHEEL_SPEED:
			WheelSpeed_Update(&sensor->wsp_data);
			break;
	}

}

/**
  * @brief  Send sensor CAN message based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_SendCAN(Sensor_t* sensor)
{
	switch (sensor->type) {
		case COOLANT_TEMP:
			CoolantTemp_SendCAN(&sensor->ct_data, sensor->can_id);
			break;
		case LINEAR_POT:
			LinearPot_SendCAN(&sensor->lp_data, sensor->can_id);
			break;
		case PITOT_TUBE:
			PitotTube_SendCAN(&sensor->pitot_data, sensor->can_id);
			break;
		case STEERING_ANGLE:
			SteeringAngle_SendCAN(&sensor->sa_data, sensor->can_id);
			break;
		case TACH:
			Tach_SendCAN(&sensor->tach_data, sensor->can_id);
			break;
		case WHEEL_SPEED:
			WheelSpeed_SendCAN(&sensor->wsp_data, sensor->can_id);
			break;
		case ODO:
			ODO_SendCAN(&sensor->odo_data, sensor->can_id);
	}


}
