/**
  ******************************************************************************
  * @file           : tach.c
  * @brief          : Rad tachometer reader implementation
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
#include "sensors/tach.h"

/* Private Variables ---------------------------------------------------------*/
static const uint32_t US_PER_MIN = 60000000;

/* Private Function Prototypes -----------------------------------------------*/
static void Tach_PackData(Tach_Data_t* tach_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize tach data
  * @param  tach_data: Pointer to tach data structure
  * @retval None
  */
void Tach_Init(Tach_Data_t* tach_data)
{
	Interrupt_Init(&tach_data->interrupt);

	tach_data->rpm = 0;
	tach_data->valid = false;
}

/**
  * @brief  Update tach value based on interrupt data
  * @param  tach_data: Pointer to tach data structure
  * @retval None
  */
void Tach_Update(Tach_Data_t* tach_data)
{
	uint32_t last_pulse_time;
	uint32_t now;
	uint32_t avg_delta;

    tach_data->valid = false;

    last_pulse_time = Interrupt_GetLastPulseTime(&tach_data->interrupt);

    now = __HAL_TIM_GET_COUNTER(&htim2);
    if (now - last_pulse_time > TACH_TIMEOUT_US) {
        Interrupt_Reset(&tach_data->interrupt);
        tach_data->timeout = true;
        return;
    }

    tach_data->valid = true;

    avg_delta = Interrupt_GetAverageDelta(&tach_data->interrupt);

    if (avg_delta) {
    	tach_data->rpm = (tach_type) (US_PER_MIN / avg_delta / TACH_PULSES_PER_ROTATION);
    }

}

/**
  * @brief  Pack tach data into CAN message
  * @param  tach_data: Pointer to tach data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void Tach_PackData(Tach_Data_t* tach_data, CAN_Message_t* msg)
{
	uint32_t avg_delta = Interrupt_GetAverageDelta(&tach_data->interrupt);

	msg->data[0] = avg_delta & 0xFF;
	msg->data[1] = (avg_delta >> 8) & 0xFF;
	msg->data[2] = (avg_delta >> 16) & 0xFF;
	msg->data[3] = avg_delta >> 24;
	msg->data[4] = tach_data->rpm & 0xFF;
	msg->data[5] = tach_data->rpm >> 8;
	msg->data[6] = 0;
	msg->data[7] = 0;

}

/**
  * @brief  Send tach data CAN message
  * @param  tach_data: Pointer to tach data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void Tach_SendCAN(Tach_Data_t* tach_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	Tach_PackData(tach_data, &msg);
	CAN_SendMessage(&msg);

}
