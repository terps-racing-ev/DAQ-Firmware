/**
  ******************************************************************************
  * @file           : steering_angle.c
  * @brief          : Steering angle sensor reader implementation
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
#include "sensors/steering_angle.h"

/* Private Function Prototypes -----------------------------------------------*/
static void SteeringAngle_PackData(SteeringAngle_Data_t* sa_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize steering angle data
  * @param  sa_data: Pointer to steering angle data structure
  * @retval None
  */
void SteeringAngle_Init(SteeringAngle_Data_t* sa_data)
{
	ADC_Init(&sa_data->adc);

    sa_data->angle = 0;
    sa_data->valid = false;

}

/**
  * @brief  Update steering angle value with new ADC read
  * @param  sa_data: Pointer to steering angle data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void SteeringAngle_Update(SteeringAngle_Data_t* sa_data, uint32_t adc_channel)
{
    sa_data->valid = false;
    sa_data->adc_err = false;

    if (ADC_Update(&sa_data->adc, adc_channel) != HAL_OK) {
        sa_data->adc_err = true;
        return;
    }

    sa_data->valid = true;

    sa_data->delta = sa_data->adc.filt_mv - SA_ZERO_MV;

    // Wrap around
    if (sa_data->delta > SA_SWING_MV) {
        sa_data->delta -= SA_MAX_MV;
    } else if (sa_data->delta < -1 * SA_SWING_MV) {
        sa_data->delta += SA_MAX_MV;
    }

    sa_data->angle = (sa_type) ((int32_t)sa_data->delta * SA_SWING_ANGLE / SA_SWING_MV);

}

/**
  * @brief  Pack steering angle data into CAN message
  * @param  sa_data: Pointer to steering angle data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void SteeringAngle_PackData(SteeringAngle_Data_t* sa_data, CAN_Message_t* msg)
{
    msg->data[0] = (sa_data->valid) | (sa_data->adc_err << 1);
    msg->data[1] = sa_data->adc.raw_mv & 0xFF;
    msg->data[2] = sa_data->adc.raw_mv >> 8;
    msg->data[3] = sa_data->adc.filt_mv & 0xFF;
    msg->data[4] = sa_data->adc.filt_mv >> 8;
    msg->data[5] = (uint8_t)(sa_data->angle & 0xFF);
    msg->data[6] = (uint8_t)(sa_data->angle >> 8);
    msg->data[7] = 0;

}

/**
  * @brief  Send steering angle data CAN message
  * @param  sa_data: Pointer to steering angle data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void SteeringAngle_SendCAN(SteeringAngle_Data_t* sa_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	SteeringAngle_PackData(sa_data, &msg);
	CAN_SendMessage(&msg);

}
