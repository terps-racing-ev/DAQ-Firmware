/**
  ******************************************************************************
  * @file           : wheel_speed.c
  * @brief          : Wheel speed sensor reader implementation
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
#include "managers/can_manager.h"
#include "sensors/wheel_speed.h"
#include <math.h>

/* Private Variables ---------------------------------------------------------*/
static const uint32_t US_PER_MIN = 60000000;
static const uint32_t US_PER_HOUR = 3600000000;
static const uint32_t INCHES_PER_MILE = 63360;
static const float ACCEL_TIMER_DISTANCE_FT = 246.0f;
static AccelTimer_Data_t* accel_timer_instance = NULL;

/* Private Function Prototypes -----------------------------------------------*/
static void WheelSpeed_PackData(WheelSpeed_Data_t* wsp_data, CAN_Message_t* msg);
static void AccelTimer_PackData(AccelTimer_Data_t* accel_timer_data, CAN_Message_t* msg);
static void AccelTimer_ResetRun(AccelTimer_Data_t* accel_timer_data);
static void AccelTimer_CheckCompletion(AccelTimer_Data_t* accel_timer_data, uint32_t timestamp);
static uint32_t AccelTimer_GetTargetTicks(void);
static uint16_t AccelTimer_GetElapsedMs(uint32_t start_time_us, uint32_t finish_time_us);
static void AccelTimer_Lock(AccelTimer_Data_t* accel_timer_data);
static void AccelTimer_Unlock(AccelTimer_Data_t* accel_timer_data);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize wheel speed data
  * @param  wsp_data: Pointer to wheel speed data structure
  * @retval None
  */
void WheelSpeed_Init(WheelSpeed_Data_t* wsp_data)
{
    Interrupt_InitData(&wsp_data->interrupt);

    wsp_data->rpm = 0;
    wsp_data->mph = 0;
    wsp_data->valid = false;

}

/**
  * @brief  Update wheel speed value based on interrupt data
  * @param  wsp_data: Pointer to wheel speed data structure
  * @retval None
  */
void WheelSpeed_Update(WheelSpeed_Data_t* wsp_data)
{
	uint32_t last_pulse_time;
	uint32_t now;
	uint32_t avg_delta;

    wsp_data->valid = false;
    wsp_data->timeout = false;

    last_pulse_time = Interrupt_GetLastPulseTime(&wsp_data->interrupt);

    now = __HAL_TIM_GET_COUNTER(&htim2);
    if (now - last_pulse_time > WSP_TIMEOUT_US) {
        Interrupt_ResetData(&wsp_data->interrupt);
        wsp_data->rpm = 0;
        wsp_data->mph = 0;
        wsp_data->timeout = true;
        return;
    }

    wsp_data->valid = true;

    avg_delta = Interrupt_GetAverageDelta(&wsp_data->interrupt);

    if (avg_delta) {
    	wsp_data->rpm = (wsp_type) (US_PER_MIN / avg_delta / WSP_SPOKES);
    	wsp_data->mph = (wsp_type) (100 * 2 * M_PI * WSP_ROLLING_RADIUS * US_PER_HOUR / avg_delta / WSP_SPOKES / INCHES_PER_MILE); // mph*100
    }

}

/**
  * @brief  Pack wheel speed data into CAN message
  * @param  wsp_data: Pointer to wheel speed data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void WheelSpeed_PackData(WheelSpeed_Data_t* wsp_data, CAN_Message_t* msg)
{
    uint32_t avg_delta = Interrupt_GetAverageDelta(&wsp_data->interrupt);

    msg->data[0] = (wsp_data->valid) | (wsp_data->timeout << 1);
	  msg->data[1] = avg_delta & 0xFF;
    msg->data[2] = (avg_delta >> 8) & 0xFF;
    msg->data[3] = (avg_delta >> 16) & 0xFF;
    msg->data[4] = avg_delta >> 24;
    msg->data[5] = wsp_data->rpm & 0xFF;
    msg->data[6] = wsp_data->rpm >> 8;
    msg->data[7] = 0;

}

/**
  * @brief  Send wheel speed data CAN message
  * @param  wsp_data: Pointer to wheel speed data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void WheelSpeed_SendCAN(WheelSpeed_Data_t* wsp_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	WheelSpeed_PackData(wsp_data, &msg);
	CAN_SendMessage(&msg);

}

/**
  * @brief  Pack odometer data into CAN message
  * @param  odo_data: Pointer to odometer data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void Odo_PackData(Odo_Data_t* odo_data, CAN_Message_t* msg)
{
    uint32_t l_ticks = Interrupt_GetTicks(&odo_data->l_data->interrupt);
    uint32_t r_ticks = Interrupt_GetTicks(&odo_data->r_data->interrupt);

    msg->data[0] = l_ticks & 0xFF;
    msg->data[1] = (l_ticks >> 8) & 0xFF;
    msg->data[2] = (l_ticks >> 16) & 0xFF;
    msg->data[3] = l_ticks >> 24;
    msg->data[4] = r_ticks & 0xFF;
    msg->data[5] = (r_ticks >> 8) & 0xFF;
    msg->data[6] = (r_ticks >> 16) & 0xFF;
    msg->data[7] = r_ticks >> 24;

}

/**
  * @brief  Send odometer data CAN message
  * @param  odo_data: Pointer to odometer data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void Odo_SendCAN(Odo_Data_t* odo_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	Odo_PackData(odo_data, &msg);
	CAN_SendMessage(&msg);

}

/**
  * @brief  Initialize acceleration timer data
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
void AccelTimer_Init(AccelTimer_Data_t* accel_timer_data)
{
  accel_timer_data->mutex = osMutexNew(NULL);
  AccelTimer_ResetRun(accel_timer_data);
  accel_timer_data->enabled = false;
  accel_timer_instance = accel_timer_data;

}

/**
  * @brief  Update acceleration timer state
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
void AccelTimer_Update(AccelTimer_Data_t* accel_timer_data)
{
  (void)accel_timer_data;

}

/**
  * @brief  Process acceleration timer CAN command
  * @param  command: Command byte
  * @retval None
  */
void AccelTimer_ProcessCommand(uint8_t command)
{
  AccelTimer_Data_t* accel_timer_data = accel_timer_instance;
  bool was_enabled;

  if (accel_timer_data == NULL) {
    return;
  }

  AccelTimer_Lock(accel_timer_data);

  switch (command) {
    case ACCEL_TIMER_CMD_DISABLE:
      accel_timer_data->enabled = false;
      accel_timer_data->running = false;
      break;
    case ACCEL_TIMER_CMD_ENABLE:
      AccelTimer_ResetRun(accel_timer_data);
      accel_timer_data->enabled = true;
      break;
    case ACCEL_TIMER_CMD_RESET:
      was_enabled = accel_timer_data->enabled;
      AccelTimer_ResetRun(accel_timer_data);
      accel_timer_data->enabled = was_enabled;
      break;
    default:
      break;
  }

  AccelTimer_Unlock(accel_timer_data);

}

/**
  * @brief  Process a wheel speed interrupt for the acceleration timer
  * @param  int_data: Interrupt data that received a pulse
  * @param  timestamp: Pulse timestamp in microseconds
  * @retval None
  */
void AccelTimer_ProcessInterrupt(Interrupt_Data_t* int_data, uint32_t timestamp)
{
  AccelTimer_Data_t* accel_timer_data = accel_timer_instance;
  bool is_left;
  bool is_right;

  if (accel_timer_data == NULL || int_data == NULL) {
    return;
  }

  is_left = (accel_timer_data->l_data != NULL) && (int_data == &accel_timer_data->l_data->interrupt);
  is_right = (accel_timer_data->r_data != NULL) && (int_data == &accel_timer_data->r_data->interrupt);

  if (!is_left && !is_right) {
    return;
  }

  AccelTimer_Lock(accel_timer_data);

  if (!accel_timer_data->enabled || accel_timer_data->complete) {
    AccelTimer_Unlock(accel_timer_data);
    return;
  }

  if (!accel_timer_data->running) {
    accel_timer_data->running = true;
    accel_timer_data->start_time_us = timestamp;
  }

  if (is_left) {
    accel_timer_data->l_ticks++;
  }

  if (is_right) {
    accel_timer_data->r_ticks++;
  }

  AccelTimer_CheckCompletion(accel_timer_data, timestamp);
  AccelTimer_Unlock(accel_timer_data);

}

/**
  * @brief  Pack acceleration timer data into CAN message
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void AccelTimer_PackData(AccelTimer_Data_t* accel_timer_data, CAN_Message_t* msg)
{
  uint8_t flags;

  AccelTimer_Lock(accel_timer_data);

  flags = (accel_timer_data->enabled) |
      (accel_timer_data->running << 1) |
      (accel_timer_data->complete << 2) |
      (accel_timer_data->l_complete << 3) |
      (accel_timer_data->r_complete << 4) |
      (accel_timer_data->avg_complete << 5);

  msg->data[0] = flags;
  msg->data[1] = 0;
  msg->data[2] = accel_timer_data->l_elapsed_ms & 0xFF;
  msg->data[3] = accel_timer_data->l_elapsed_ms >> 8;
  msg->data[4] = accel_timer_data->r_elapsed_ms & 0xFF;
  msg->data[5] = accel_timer_data->r_elapsed_ms >> 8;
  msg->data[6] = accel_timer_data->avg_elapsed_ms & 0xFF;
  msg->data[7] = accel_timer_data->avg_elapsed_ms >> 8;

  AccelTimer_Unlock(accel_timer_data);

}

/**
  * @brief  Send acceleration timer CAN message
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void AccelTimer_SendCAN(AccelTimer_Data_t* accel_timer_data, uint32_t can_id)
{
  CAN_Message_t msg;

  msg.id = can_id;
  AccelTimer_PackData(accel_timer_data, &msg);
  CAN_SendMessage(&msg);

}

/**
  * @brief  Reset acceleration timer run data
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
static void AccelTimer_ResetRun(AccelTimer_Data_t* accel_timer_data)
{
  accel_timer_data->l_ticks = 0;
  accel_timer_data->r_ticks = 0;
  accel_timer_data->start_time_us = 0;
  accel_timer_data->l_finish_time_us = 0;
  accel_timer_data->r_finish_time_us = 0;
  accel_timer_data->avg_finish_time_us = 0;
  accel_timer_data->l_elapsed_ms = 0;
  accel_timer_data->r_elapsed_ms = 0;
  accel_timer_data->avg_elapsed_ms = 0;
  accel_timer_data->running = false;
  accel_timer_data->complete = false;
  accel_timer_data->l_complete = false;
  accel_timer_data->r_complete = false;
  accel_timer_data->avg_complete = false;

}

/**
  * @brief  Check acceleration timer completion state
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @param  timestamp: Pulse timestamp in microseconds
  * @retval None
  */
static void AccelTimer_CheckCompletion(AccelTimer_Data_t* accel_timer_data, uint32_t timestamp)
{
  uint32_t target_ticks = AccelTimer_GetTargetTicks();
  uint32_t avg_target_ticks = target_ticks * 2;

  if (!accel_timer_data->l_complete && accel_timer_data->l_ticks >= target_ticks) {
    accel_timer_data->l_finish_time_us = timestamp;
    accel_timer_data->l_elapsed_ms = AccelTimer_GetElapsedMs(accel_timer_data->start_time_us, timestamp);
    accel_timer_data->l_complete = true;
  }

  if (!accel_timer_data->r_complete && accel_timer_data->r_ticks >= target_ticks) {
    accel_timer_data->r_finish_time_us = timestamp;
    accel_timer_data->r_elapsed_ms = AccelTimer_GetElapsedMs(accel_timer_data->start_time_us, timestamp);
    accel_timer_data->r_complete = true;
  }

  if (!accel_timer_data->avg_complete && (accel_timer_data->l_ticks + accel_timer_data->r_ticks) >= avg_target_ticks) {
    accel_timer_data->avg_finish_time_us = timestamp;
    accel_timer_data->avg_elapsed_ms = AccelTimer_GetElapsedMs(accel_timer_data->start_time_us, timestamp);
    accel_timer_data->avg_complete = true;
  }

  accel_timer_data->complete = accel_timer_data->l_complete && accel_timer_data->r_complete && accel_timer_data->avg_complete;
  if (accel_timer_data->complete) {
    accel_timer_data->running = false;
  }

}

/**
  * @brief  Get target wheel-speed ticks for 246 ft
  * @retval Target tick count
  */
static uint32_t AccelTimer_GetTargetTicks(void)
{
  float circumference_in = (float)M_PI * WSP_FRONT_WHEEL_DIAMETER_IN;
  float target_ticks = ACCEL_TIMER_DISTANCE_FT * 12.0f * WSP_SPOKES / circumference_in;

  return (uint32_t)ceilf(target_ticks);

}

/**
  * @brief  Get elapsed milliseconds between two microsecond timestamps
  * @param  start_time_us: Start timestamp
  * @param  finish_time_us: Finish timestamp
  * @retval Saturated elapsed milliseconds
  */
static uint16_t AccelTimer_GetElapsedMs(uint32_t start_time_us, uint32_t finish_time_us)
{
  uint32_t elapsed_ms = (finish_time_us - start_time_us) / 1000;

  if (elapsed_ms > UINT16_MAX) {
    return UINT16_MAX;
  }

  return (uint16_t)elapsed_ms;

}

/**
  * @brief  Lock acceleration timer mutex
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
static void AccelTimer_Lock(AccelTimer_Data_t* accel_timer_data)
{
  if (accel_timer_data->mutex != NULL) {
    osMutexAcquire(accel_timer_data->mutex, osWaitForever);
  }

}

/**
  * @brief  Unlock acceleration timer mutex
  * @param  accel_timer_data: Pointer to acceleration timer data structure
  * @retval None
  */
static void AccelTimer_Unlock(AccelTimer_Data_t* accel_timer_data)
{
  if (accel_timer_data->mutex != NULL) {
    osMutexRelease(accel_timer_data->mutex);
  }

}
