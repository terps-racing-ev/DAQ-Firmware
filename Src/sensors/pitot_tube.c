/**
  ******************************************************************************
  * @file           : pitot_tube.c
  * @brief          : Pitot tube reader implementation
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
#include "sensors/pitot_tube.h"
#include <math.h>

/* Private Variables ---------------------------------------------------------*/
static const float AIR_DENSITY = 1.225;
static const float M_PER_S_TO_MPH = 2.23694;

/* Private Function Prototypes -----------------------------------------------*/
static void PitotTube_PackData(PitotTube_Data_t* pitot_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize pitot tube data
  * @param  pitot_data: Pointer to pitot tube data structure
  * @retval None
  */
void PitotTube_Init(PitotTube_Data_t* pitot_data)
{
    ADC_Init(&pitot_data->adc);

    pitot_data->calibration_sum = 0;
    pitot_data->calibration_cnt = 0;
    pitot_data->vel_scaled = 0;
    pitot_data->valid = false;

}

/**
  * @brief  Update pitot tube value with new ADC read
  * @param  pitot_data: Pointer to pitot tube data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void PitotTube_Update(PitotTube_Data_t* pitot_data, uint32_t adc_channel)
{
    uint16_t adc_zero_point;
    float vel;

    pitot_data->valid = false;
    pitot_data->adc_err = false;

    if (pitot_data->calibration_cnt < PITOT_CALIBRATION_READINGS) {
        if (ADC_Read(adc_channel, &pitot_data->adc.adc_value) != HAL_OK) {
            pitot_data->adc_err = true;
            return;
        }
        pitot_data->calibration_sum += pitot_data->adc.adc_value;
        pitot_data->calibration_cnt++;
        pitot_data->vel_scaled = 0;
        return;
    } else if (pitot_data->calibration_cnt == PITOT_CALIBRATION_READINGS) {
        pitot_data->calibration_cnt++;
        adc_zero_point = (uint16_t) (pitot_data->calibration_sum / PITOT_CALIBRATION_READINGS);
        pitot_data->zero_point = ADC_ConvertToUnscaledMV(adc_zero_point);
    }

    if (ADC_Update(&pitot_data->adc, adc_channel) != HAL_OK) {
        pitot_data->adc_err = true;
        return;
    }

    pitot_data->valid = true;

    pitot_data->pressure = pitot_data->adc.filt_mv - pitot_data->zero_point; // 1 to 1 with Pa theoretically
    vel = (sqrt(fabsf( 2*(pitot_data->pressure) / AIR_DENSITY) )) * M_PER_S_TO_MPH; // velocity in mph

    pitot_data->vel_scaled = (pitot_type) (vel * 100);

}

/**
  * @brief  Pack pitot tube temp data into CAN message
  * @param  pitot_data: Pointer to pitot tube data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void PitotTube_PackData(PitotTube_Data_t* pitot_data, CAN_Message_t* msg)
{
    msg->data[0] = pitot_data->adc.adc_value & 0xFF;
    msg->data[1] = pitot_data->adc.adc_value >> 8;
    msg->data[2] = pitot_data->adc.raw_mv & 0xFF;
    msg->data[3] = pitot_data->adc.raw_mv >> 8;
    msg->data[4] = pitot_data->adc.filt_mv & 0xFF;
    msg->data[5] = pitot_data->adc.filt_mv >> 8;
    msg->data[6] = pitot_data->vel_scaled & 0xFF;
    msg->data[7] = pitot_data->vel_scaled >> 8;

}

/**
  * @brief  Send pitot tube data CAN message
  * @param  pitot_data: Pointer to pitot tube data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void PitotTube_SendCAN(PitotTube_Data_t* pitot_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PitotTube_PackData(pitot_data, &msg);
    CAN_SendMessage(&msg);

}
