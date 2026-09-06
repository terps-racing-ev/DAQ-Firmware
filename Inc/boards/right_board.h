/**
  ******************************************************************************
  * @file           : right_board.h
  * @brief          : DAQ back right board configuration
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

#ifndef RIGHT_BOARD_H
#define RIGHT_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "managers/sensor_manager.h"

/* Defines -------------------------------------------------------------------*/
#define DBR_BOARD_ID 			0x0C
#define DBR_NUM_SENSORS 		6

/* Pinout --------------------------------------------------------------------*/
#define WSP_BR_GPIO_PIN 	 	  GPIO_PIN_0
#define CT_RAD1_ADC_CHANNEL   ADC_CHANNEL_6
#define SHOCK_BR_ADC_CHANNEL	ADC_CHANNEL_8
#define CT_MOTOR_ADC_CHANNEL 	ADC_CHANNEL_9
#define CT_INV_ADC_CHANNEL 		ADC_CHANNEL_10
#define CT_RAD2_ADC_CHANNEL	  ADC_CHANNEL_11

/* CAN IDs -------------------------------------------------------------------*/
#define WSP_BR_CAN_ID 			0x0DC10000
#define CT_RAD1_CAN_ID      0x0DC50000
#define SHOCK_BR_CAN_ID 		0x0DC20000
#define CT_MOTOR_CAN_ID 		0x0DC51000
#define CT_INV_CAN_ID 			0x0DC52000
#define CT_RAD2_CAN_ID 			0x0DC53000

/* External Variables --------------------------------------------------------*/
extern Sensor_t SensorList[MAX_NUM_SENSORS];

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Configure DAQ back right board sensor list and interrupts
  * @retval None
  */
void DBR_Config(void);

#endif // RIGHT_BOARD_H
