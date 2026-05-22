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

/* Private Function Prototypes -----------------------------------------------*/
static void WheelSpeed_PackData(WheelSpeed_Data_t* wsp_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize wheel speed data
  * @param  wsp_data: Pointer to wheel speed data structure
  * @retval None
  */
void WheelSpeed_Init(WheelSpeed_Data_t* wsp_data)
{
    Interrupt_Init(&wsp_data->interrupt);

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
        // Interrupt_Reset(&wsp_data->interrupt);
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
