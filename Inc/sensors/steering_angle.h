/**
  ******************************************************************************
  * @file           : steering_angle.h
  * @brief          : Steering angle sensor reader
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

#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_defs.h"
#include "managers/adc_manager.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define SA_ZERO_MV 2500 // Zero point voltage based on calibration
#define SA_SWING_MV 2500 // Voltage range in either direction
#define SA_MAX_MV 5000
#define SA_SWING_ANGLE 1800 // 180 degrees * 10

/* Steering Angle Data Structure */
typedef struct {

    ADC_Data_t adc;

    int16_t delta;
    sa_type angle;

    bool valid;

    bool adc_err;

} SteeringAngle_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize steering angle data
  * @param  sa_data: Pointer to steering angle data structure
  * @retval None
  */
void SteeringAngle_Init(SteeringAngle_Data_t* sa_data);

/**
  * @brief  Update steering angle value with new ADC read
  * @param  sa_data: Pointer to steering angle data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void SteeringAngle_Update(SteeringAngle_Data_t* sa_data, uint32_t adc_channel);

/**
  * @brief  Send steering angle data CAN message
  * @param  sa_data: Pointer to steering angle data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void SteeringAngle_SendCAN(SteeringAngle_Data_t* sa_data, uint32_t can_id);

#endif // STEERING_ANGLE_H
