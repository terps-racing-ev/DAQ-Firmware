/**
  ******************************************************************************
  * @file           : left_board.h
  * @brief          : DAQ back left board configuration
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

#ifndef LEFT_BOARD_H
#define LEFT_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "managers/sensor_manager.h"

/* Defines -------------------------------------------------------------------*/
#define DBL_BOARD_ID 			0x0B
#define DBL_NUM_SENSORS 		6

/* Pinout --------------------------------------------------------------------*/
#define WSP_BL_GPIO_PIN 		      GPIO_PIN_0
#define PITOT_SIDEPOD_ADC_CHANNEL ADC_CHANNEL_6
#define SHOCK_BL_ADC_CHANNEL 	    ADC_CHANNEL_8
#define PITOT_FW_ADC_CHANNEL 	    ADC_CHANNEL_9
#define PITOT_RW_ADC_CHANNEL 	    ADC_CHANNEL_10
#define PITOT_NOSE_ADC_CHANNEL 	  ADC_CHANNEL_11

/* CAN IDs -------------------------------------------------------------------*/
#define WSP_BL_CAN_ID 			  0x0DB10000
#define PITOT_SIDEPOD_CAN_ID  0x0DB40000
#define SHOCK_BL_CAN_ID 		  0x0DB20000
#define PITOT_FW_CAN_ID 		  0x0DB41000
#define PITOT_RW_CAN_ID 			0x0DB42000
#define PITOT_NOSE_CAN_ID 		0x0DB43000

/* External Variables --------------------------------------------------------*/
extern Sensor_t SensorList[MAX_NUM_SENSORS];

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Configure DAQ back left board sensor list and interrupts
  * @retval None
  */
void DBL_Config(void);

#endif // LEFT_BOARD_H
