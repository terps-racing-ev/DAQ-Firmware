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
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define WSP_FRONT_WHEEL_DIAMETER_IN 17.66f
#define WSP_ROLLING_RADIUS 9 // Inches
#define WSP_SPOKES 16
#define WSP_TIMEOUT_US 1000000

#define ACCEL_TIMER_CMD_DISABLE 0
#define ACCEL_TIMER_CMD_ENABLE 1
#define ACCEL_TIMER_CMD_RESET 2

/* Wheel Speed Data Structure */
typedef struct {

    Interrupt_Data_t interrupt;

    wsp_type rpm;
    wsp_type mph;

    bool valid;

    bool timeout;

} WheelSpeed_Data_t;

/* Odo Data Structure */
typedef struct {

	WheelSpeed_Data_t* l_data;
	WheelSpeed_Data_t* r_data;

} Odo_Data_t;

/* Acceleration Timer Data Structure */
typedef struct {

  osMutexId_t mutex;

  WheelSpeed_Data_t* l_data;
  WheelSpeed_Data_t* r_data;

  uint32_t l_ticks;
  uint32_t r_ticks;

  uint32_t start_time_us;
  uint32_t l_finish_time_us;
  uint32_t r_finish_time_us;
  uint32_t avg_finish_time_us;

  uint16_t l_elapsed_ms;
  uint16_t r_elapsed_ms;
  uint16_t avg_elapsed_ms;

  bool enabled;
  bool running;
  bool complete;
  bool l_complete;
  bool r_complete;
  bool avg_complete;

} AccelTimer_Data_t;

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
  * @brief  Send odometer data CAN message
  * @param  odo_data: Pointer to odometer data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void Odo_SendCAN(Odo_Data_t* odo_data, uint32_t can_id);

/**
  * @brief  Initialize acceleration timer data
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
void AccelTimer_Init(AccelTimer_Data_t* accel_timer_data);

/**
  * @brief  Update acceleration timer state
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
void AccelTimer_Update(AccelTimer_Data_t* accel_timer_data);

/**
  * @brief  Send acceleration timer CAN message
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void AccelTimer_SendCAN(AccelTimer_Data_t* accel_timer_data, uint32_t can_id);

/**
  * @brief  Process acceleration timer CAN command
  * @param  command: Command byte
  * @retval None
  */
void AccelTimer_ProcessCommand(uint8_t command);

/**
  * @brief  Process a wheel speed interrupt for the acceleration timer
  * @param  int_data: Interrupt data that received a pulse
  * @param  timestamp: Pulse timestamp in microseconds
  * @retval None
  */
void AccelTimer_ProcessInterrupt(Interrupt_Data_t* int_data, uint32_t timestamp);

#endif // WHEEL_SPEED_H
