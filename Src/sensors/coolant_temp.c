/**
  ******************************************************************************
  * @file           : coolant_temp.c
  * @brief          : Coolant temp sensor reader implementation
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
#include "sensors/coolant_temp.h"
#include <math.h>

/* Private Variables ---------------------------------------------------------*/
/* Steinhart Equation Constants (see MATLAB file to adjust) */
static const float A = -0.0009123445192;
static const float B = 0.0005229826803;
static const float C = -0.0000007045640079;

/* Private Function Prototypes -----------------------------------------------*/
static void CoolantTemp_PackData(CoolantTemp_Data_t* ct_data, CAN_Message_t* msg);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize coolant temp data
  * @param  ct_data: Pointer to coolant temp data structure
  * @retval None
  */
void CoolantTemp_Init(CoolantTemp_Data_t* ct_data)
{
	ADC_Init(&ct_data->adc);

    ct_data->temp = 0;
    ct_data->valid = false;

}

/**
  * @brief  Update coolant temp value with new ADC read
  * @param  ct_data: Pointer to coolant temp data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void CoolantTemp_Update(CoolantTemp_Data_t* ct_data, uint32_t adc_channel)
{
    float resistance, lnr, tempC;

    ct_data->valid = false;
    ct_data->out_of_range = false;
    ct_data->adc_err = false;

    if (ADC_Update(&ct_data->adc, adc_channel) != HAL_OK) {
        ct_data->adc_err = true;
        return;
    }

    if (ct_data->adc.filt_mv < CT_MIN_MV || ct_data->adc.filt_mv > CT_MAX_MV) {
        ct_data->out_of_range = true;
        return;
    }

    ct_data->valid = true;

    resistance = (float)ct_data->adc.filt_mv * CT_PULLUP_RESISTOR / (SUPPLY_VOLTAGE - (float)ct_data->adc.filt_mv);
    lnr = log(resistance);
    tempC = ( 1.0 / (A + B*lnr + C*pow(lnr, 3)) ) - 273.15;

    ct_data->temp = (ct_type) tempC;

}

/**
  * @brief  Pack coolant temp data into CAN message
  * @param  ct_data: Pointer to coolant temp data structure
  * @param  msg: Pointer to CAN message structure
  * @retval None
  */
static void CoolantTemp_PackData(CoolantTemp_Data_t* ct_data, CAN_Message_t* msg)
{
    msg->data[0] = ct_data->adc.adc_value & 0xFF;
    msg->data[1] = ct_data->adc.adc_value >> 8;
    msg->data[2] = ct_data->adc.raw_mv & 0xFF;
    msg->data[3] = ct_data->adc.raw_mv >> 8;
    msg->data[4] = ct_data->adc.filt_mv & 0xFF;
    msg->data[5] = ct_data->adc.filt_mv >> 8;
    msg->data[6] = ct_data->temp & 0xFF;
    msg->data[7] = ct_data->temp >> 8;

}

/**
  * @brief  Send coolant temp data CAN message
  * @param  ct_data: Pointer to coolant temp data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void CoolantTemp_SendCAN(CoolantTemp_Data_t* ct_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	CoolantTemp_PackData(ct_data, &msg);
	CAN_SendMessage(&msg);

}
