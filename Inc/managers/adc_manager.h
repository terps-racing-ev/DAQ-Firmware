/**
  ******************************************************************************
  * @file           : adc_manager.h
  * @brief          : ADC reading manager
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

#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "utils/moving_average.h"
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define ADC_TIMEOUT_MS 1

/* External Variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

/* ADC Data Structure */
typedef struct {

	uint16_t adc_value;
	uint16_t raw_mv;
	uint16_t filt_mv;

	MovingAverage_Data_t ma;

} ADC_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize ADC data and moving average
  * @param  adc_data: Pointer to ADC data structure
  * @retval None
  */
void ADC_Init(ADC_Data_t* adc_data);

/**
  * @brief  Update ADC data with new ADC value converted to raw and filtered mV
  * @param  adc_data: Pointer to ADC data structure
  * @param  adc_channel: ADC channel to read from
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef ADC_Update(ADC_Data_t* adc_data, uint32_t adc_channel);

/**
  * @brief  Read ADC channel
  * @param  adc_channel: ADC channel to read from
  * @param  adc_value: Output pointer for ADC value
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef ADC_Read(uint32_t adc_channel, uint16_t* adc_value);

/**
  * @brief  Convert ADC reading to sensor output voltage (pre-voltage divider)
  * @param  adc_value: Raw ADC reading
  * @retval Sensor output voltage in mV
  */
uint16_t ADC_ConvertToUnscaledMV(uint16_t adc_value);

#endif // ADC_MANAGER_H
