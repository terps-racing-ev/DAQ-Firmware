/**
  ******************************************************************************
  * @file           : wheel_speed.h
  * @brief          : Wheel speed sensor reader
  ******************************************************************************
  * @attention
  *
  * Some realistic times between pulses:
  * 1mph ~200,000us
  * 40mph ~5,000us
  * 100mph ~2,000us
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

#ifndef WHEEL_SPEED_H
#define WHEEL_SPEED_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_defs.h"
#include "managers/interrupt_manager.h"

/* Defines -------------------------------------------------------------------*/
#define WSP_ROLLING_RADIUS 9 // Inches
#define WSP_SPOKES 16
#define WSP_TIMEOUT_US 1000000

/* Wheel Speed Data Structure */
typedef struct {


    Interrupt_Data_t interrupt;

    wsp_type rpm;
    wsp_type mph;

    uint8_t error_flags;

    uint8_t use_odo;

} WheelSpeed_Data_t;

typedef struct {
  WheelSpeed_Data_t* l_data;

  WheelSpeed_Data_t* r_data;
} ODO_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize wheel speed data
  * @param  wsp_data: Pointer to wheel speed data structure
  * @retval None
  */
void WheelSpeed_Init(WheelSpeed_Data_t* wsp_data);

/**
  * @brief  Update wheel speed value based on interrupt data
  * @param  wsp_data: Pointer to wheel speed data structure
  * @retval None
  */
void WheelSpeed_Update(WheelSpeed_Data_t* wsp_data);


/**
 * @brief  Send wheel speed data CAN message
 * @param  wsp_data: Pointer to wheel speed data structure
 * @param  can_id: CAN message ID
 * @retval None
 */
void WheelSpeed_SendCAN(WheelSpeed_Data_t* wsp_data, uint32_t can_id);

/**
  * @brief  Send odometry data over CAN
  * @param  wsp_data: Pointer to odometry data structure
  * @retval None
  */
void ODO_SendCAN(ODO_Data_t* odo_data);

#endif // WHEEL_SPEED_H
