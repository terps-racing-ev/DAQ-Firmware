/**
  ******************************************************************************
  * @file           : adc_manager.c
  * @brief          : ADC reading manager implementation
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
#include "managers/adc_manager.h"
#include "sensor_defs.h"

/* Private Function Prototypes -----------------------------------------------*/
static HAL_StatusTypeDef ADC_ConfigureChannel(uint32_t adc_channel);
static uint32_t ADC_CalculateVREF(void);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize ADC data and moving average
  * @param  adc_data: Pointer to ADC data structure
  * @retval None
  */
void ADC_Init(ADC_Data_t* adc_data)
{
	adc_data->adc_value = 0;
	adc_data->raw_mv = 0;
	adc_data->filt_mv = 0;

	MovingAverage_Init(&adc_data->ma, WINDOW_SIZE);

}

/**
  * @brief  Update ADC data with new ADC value converted to raw and filtered mV
  * @param  adc_data: Pointer to ADC data structure
  * @param  adc_channel: ADC channel to read from
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef ADC_Update(ADC_Data_t* adc_data, uint32_t adc_channel)
{
	if (ADC_Read(adc_channel, &adc_data->adc_value) != HAL_OK) {
		return HAL_ERROR;
	}

	adc_data->raw_mv = ADC_ConvertToUnscaledMV(adc_data->adc_value);
	adc_data->filt_mv = (uint16_t) MovingAverage_Update(&adc_data->ma, adc_data->raw_mv);

	return HAL_OK;

}

/**
  * @brief  Configure ADC channel
  * @param  adc_channel: ADC channel to read from
  * @retval HAL_StatusTypeDef
  */
static HAL_StatusTypeDef ADC_ConfigureChannel(uint32_t adc_channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = adc_channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    return HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

/**
  * @brief  Read ADC channel
  * @param  adc_channel: ADC channel to read from
  * @param  adc_value: Output pointer for ADC value
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef ADC_Read(uint32_t adc_channel, uint16_t* adc_value)
{
    HAL_StatusTypeDef err;

    // Configure ADC for the specified channel
    err = ADC_ConfigureChannel(adc_channel);
    if (err != HAL_OK) {
        return err;
    }

    // Start ADC conversion
    err = HAL_ADC_Start(&hadc1);
    if (err != HAL_OK) {
        return err;
    }

    // Wait for conversion to complete
    err = HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT_MS);
    if (err != HAL_OK) {
        return err;
    }

    // Get ADC value
    *adc_value = HAL_ADC_GetValue(&hadc1);

    // Stop ADC
    return HAL_ADC_Stop(&hadc1);

}

/**
  * @brief  Calculate ADC reference voltage
  * @retval Reference voltage in mV
  */
static uint32_t ADC_CalculateVREF(void)
{
    uint16_t vref_adc_value;
	uint32_t vref_voltage = 0;

    if (ADC_Read(ADC_CHANNEL_VREFINT, &vref_adc_value) == HAL_OK) {
		vref_voltage = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(vref_adc_value, ADC_RESOLUTION_12B);
	}

    return vref_voltage;

}

/**
  * @brief  Convert ADC reading to sensor output voltage (pre-voltage divider)
  * @param  adc_value: Raw ADC reading
  * @retval Sensor output voltage in mV
  */
uint16_t ADC_ConvertToUnscaledMV(uint16_t adc_value)
{
    uint32_t vref;
    uint16_t adc_mv;

    vref = ADC_CalculateVREF();
    adc_mv = (uint16_t)(vref * adc_value >> 12);

    return (uint16_t) (adc_mv * VOLTAGE_DIVIDER_SCALE_FACTOR);

}
