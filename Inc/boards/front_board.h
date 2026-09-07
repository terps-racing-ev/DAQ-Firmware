/**
  ******************************************************************************
  * @file           : front_board.h
  * @brief          : DAQ front board configuration
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

#ifndef FRONT_BOARD_H
#define FRONT_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "managers/sensor_manager.h"

/* Defines -------------------------------------------------------------------*/
#define DBF_BOARD_ID 				0x0A
#define DBF_NUM_SENSORS 			7 // dummy sensors for odometer and acceleration timer

/* Pinout --------------------------------------------------------------------*/
#define WSP_FL_GPIO_PIN 			GPIO_PIN_0
#define WSP_FR_GPIO_PIN 			GPIO_PIN_1
#define SHOCK_FL_ADC_CHANNEL 		ADC_CHANNEL_11
#define SHOCK_FR_ADC_CHANNEL 		ADC_CHANNEL_9
#define STEERING_ANGLE_ADC_CHANNEL 	ADC_CHANNEL_10

/* CAN IDs -------------------------------------------------------------------*/
#define WSP_FL_CAN_ID 				0x0DA10000
#define WSP_FR_CAN_ID 				0x0DA11000
#define WSPD_DEBUG_CAN_ID 			0x0DA100DB
#define SHOCK_FL_CAN_ID 			0x0DA20000
#define SHOCK_FR_CAN_ID 			0x0DA21000
#define STEERING_ANGLE_CAN_ID 		0x0DA30000
#define PITOT_CAN_ID 				0x0DA40000
#define ACCEL_TIMER_CAN_ID 		0x0DA80000
#define ACCEL_TIMER_DISTANCE_CAN_ID 	0x0DA81000
#define ODO_LR_CAN_ID       		0x0DA90000

/* External Variables --------------------------------------------------------*/
extern Sensor_t SensorList[MAX_NUM_SENSORS];

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Configure DAQ front board sensor list and interrupts
  * @retval None
  */
void DBF_Config(void);

#endif // FRONT_BOARD_H
