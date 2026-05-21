/**
  ******************************************************************************
  * @file           : linear_pot.h
  * @brief          : Linear potentiometer reader
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

#ifndef LINEAR_POT_H
#define LINEAR_POT_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_defs.h"
#include "managers/adc_manager.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define LP_MAX_RANGE 500 // mm*10

/* Linear Pot Data Structure */
typedef struct {

    ADC_Data_t adc;

    lp_type dist_scaled;

    bool valid;

    bool adc_err;

} LinearPot_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize linear pot data
  * @param  lp_data: Pointer to linear pot data structure
  * @retval None
  */
void LinearPot_Init(LinearPot_Data_t* lp_data);

/**
  * @brief  Update linear pot value with new ADC read
  * @param  lp_data: Pointer to linear pot data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void LinearPot_Update(LinearPot_Data_t* lp_data, uint32_t adc_channel);

/**
  * @brief  Send linear pot data CAN message
  * @param  lp_data: Pointer to linear pot data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void LinearPot_SendCAN(LinearPot_Data_t* lp_data, uint32_t can_id);

#endif // LINEAR_POT_H
