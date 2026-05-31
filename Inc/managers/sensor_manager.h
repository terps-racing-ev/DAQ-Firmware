/**
  ******************************************************************************
  * @file           : sensor_manager.h
  * @brief          : Sensor manager
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

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "sensors/coolant_temp.h"
#include "sensors/linear_pot.h"
#include "sensors/pitot_tube.h"
#include "sensors/steering_angle.h"
#include "sensors/tach.h"
#include "sensors/wheel_speed.h"

/* Defines -------------------------------------------------------------------*/
#define MAX_NUM_SENSORS 8 // 6 actual sensors per board, plus pseudo-sensors
#define SENSOR_UPDATE_INTERVAL_MS 50
#define SENSOR_CAN_SEND_INTERVAL_MS 100

/* External Variables --------------------------------------------------------*/
extern uint8_t NUM_SENSORS;
extern int8_t* INV_TEMP_PTR;

/* Sensor Type Enumeration */
typedef enum {
	COOLANT_TEMP,
	LINEAR_POT,
	PITOT_TUBE,
	STEERING_ANGLE,
	TACH,
	WHEEL_SPEED,
  ODO,
  ACCEL_TIMER
} SensorType;

/* Sensor Structure */
typedef struct Sensor_t {

    SensorType type;

	union {
        uint32_t adc_channel;
        uint16_t gpio_pin;
    };

    union {
        CoolantTemp_Data_t ct_data;
    	LinearPot_Data_t lp_data;
        PitotTube_Data_t pitot_data;
        SteeringAngle_Data_t sa_data;
        Tach_Data_t tach_data;
        WheelSpeed_Data_t wsp_data;
        Odo_Data_t odo_data;
        AccelTimer_Data_t accel_timer_data;
    };

    uint32_t can_id;

} Sensor_t;

/* Function Prototypes --------------------------------------------------------*/

/**
  * @brief  Main sensor manager task
  * @param  argument: Not used
  * @retval None
  */
void Sensor_ManagerTask(void *argument);

/**
  * @brief  Initialize sensor data based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_Init(Sensor_t* sensor);

/**
  * @brief  Update sensor data based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_Update(Sensor_t* sensor);

/**
  * @brief  Send sensor CAN message based on sensor type
  * @param  sensor: Pointer to sensor structure
  * @retval None
  */
void Sensor_SendCAN(Sensor_t* sensor);

#endif // SENSOR_MANAGER_H
