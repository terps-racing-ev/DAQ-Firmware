/**
  ******************************************************************************
  * @file           : linear_pot.c
  * @brief          : Linear potentiometer reader implementation
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
#include "sensors/linear_pot.h"

/* Private Function Prototypes -----------------------------------------------*/
static void LinearPot_PackData(LinearPot_Data_t* lp_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize linear pot data
  * @param  lp_data: Pointer to linear pot data structure
  * @retval None
  */
void LinearPot_Init(LinearPot_Data_t* lp_data)
{
	ADC_Init(&lp_data->adc);

    lp_data->dist_scaled = 0;
    lp_data->valid = false;

}

/**
  * @brief  Update linear pot value with new ADC read
  * @param  lp_data: Pointer to linear pot data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void LinearPot_Update(LinearPot_Data_t* lp_data, uint32_t adc_channel)
{
    lp_data->valid =false;
    lp_data->adc_err = false;

    if (ADC_Update(&lp_data->adc, adc_channel) != HAL_OK) {
        lp_data->adc_err = true;
        return;
    }

    lp_data->valid = true;

    lp_data->dist_scaled = (lp_type) ((uint32_t)lp_data->adc.filt_mv * LP_MAX_RANGE / SUPPLY_VOLTAGE);

}

/**
  * @brief  Pack linear pot data into CAN message
  * @param  lp_data: Pointer to linear pot data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void LinearPot_PackData(LinearPot_Data_t* lp_data, CAN_Message_t* msg)
{
    msg->data[0] = (lp_data->valid) | (lp_data->adc_err << 1);
    msg->data[1] = lp_data->adc.raw_mv & 0xFF;
    msg->data[2] = lp_data->adc.raw_mv >> 8;
    msg->data[3] = lp_data->adc.filt_mv & 0xFF;
    msg->data[4] = lp_data->adc.filt_mv >> 8;
    msg->data[5] = lp_data->dist_scaled & 0xFF;
    msg->data[6] = lp_data->dist_scaled >> 8;
    msg->data[7] = 0;

}

/**
  * @brief  Send linear pot data CAN message
  * @param  lp_data: Pointer to linear pot data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void LinearPot_SendCAN(LinearPot_Data_t* lp_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	LinearPot_PackData(lp_data, &msg);
	CAN_SendMessage(&msg);

}
