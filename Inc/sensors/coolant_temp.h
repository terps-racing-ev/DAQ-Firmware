/**
  ******************************************************************************
  * @file           : coolant_temp.h
  * @brief          : Coolant temp sensor reader
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

#ifndef COOLANT_TEMP_H
#define COOLANT_TEMP_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_types.h"
#include "managers/adc_manager.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define CT_PULLUP_RESISTOR 1000
#define CT_MIN_MV 534 // -40C
#define CT_MAX_MV 4853 // 120C

/* Coolant Temp Data Structure */
typedef struct {

    ADC_Data_t adc;

    ct_type temp;

    bool valid;

    bool adc_err;
    bool out_of_range;

} CoolantTemp_Data_t;

/* Function Prototypes --------------------------------------------------------*/

/**
  * @brief  Initialize coolant temp data
  * @param  ct_data: Pointer to coolant temp data structure
  * @retval None
  */
void CoolantTemp_Init(CoolantTemp_Data_t* ct_data);

/**
  * @brief  Update coolant temp value with new ADC read
  * @param  ct_data: Pointer to coolant temp data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void CoolantTemp_Update(CoolantTemp_Data_t* ct_data, uint32_t adc_channel);

/**
  * @brief  Send coolant temp data CAN message
  * @param  ct_data: Pointer to coolant temp data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void CoolantTemp_SendCAN(CoolantTemp_Data_t* ct_data, uint32_t can_id);

#endif // COOLANT_TEMP_H
