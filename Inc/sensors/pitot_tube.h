/**
  ******************************************************************************
  * @file           : pitot_tube.h
  * @brief          : Pitot tube reader
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

#ifndef PITOT_TUBE_H
#define PITOT_TUBE_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_defs.h"
#include "managers/adc_manager.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define PITOT_CALIBRATION_READINGS 100

/* Pitot Tube Data Structure */
typedef struct {

    ADC_Data_t adc;

    uint32_t calibration_sum;
    uint16_t calibration_cnt;
    uint16_t zero_point;

    int16_t pressure;
    pitot_type vel_scaled;

    bool valid;

    bool adc_err;

} PitotTube_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize pitot tube data
  * @param  pitot_data: Pointer to pitot tube data structure
  * @retval None
  */
void PitotTube_Init(PitotTube_Data_t* pitot_data);

/**
  * @brief  Update pitot tube value with new ADC read
  * @param  pitot_data: Pointer to pitot tube data structure
  * @param  adc_channel: ADC channel to read from
  * @retval None
  */
void PitotTube_Update(PitotTube_Data_t* pitot_data, uint32_t adc_channel);

/**
  * @brief  Send pitot tube data CAN message
  * @param  pitot_data: Pointer to pitot tube data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void PitotTube_SendCAN(PitotTube_Data_t* pitot_data, uint32_t can_id);

#endif // PITOT_TUBE_H
