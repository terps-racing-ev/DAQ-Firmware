/**
  ******************************************************************************
  * @file           : moving_average.c
  * @brief          : Moving average calculator implementation
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
#include "utils/moving_average.h"

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Initialize moving average data
  * @param  ma: Pointer to moving average data structure
  * @param  window_size: Moving average window size
  * @retval None
  */
void MovingAverage_Init(MovingAverage_Data_t* ma, uint8_t window_size)
{
    if (ma == NULL || window_size == 0) {
        return;
    }

    ma->weight = 2.0 / (window_size + 1.0);
    ma->filtered_val = 0;

}

/**
  * @brief  Update moving average with new value
  * @param  ma: Pointer to moving average data structure
  * @param  new_val: New value to add
  * @retval New average value
  */
uint32_t MovingAverage_Update(MovingAverage_Data_t* ma, uint32_t new_val)
{
    if (ma == NULL) {
        return new_val;
    }

    ma->filtered_val = (1.0 - ma->weight) * ma->filtered_val + (ma->weight) * new_val;

    return (uint32_t) ma->filtered_val;

}

/**
  * @brief  Reset moving average data
  * @param  ma: Pointer to moving average structure
  * @retval None
  */
void MovingAverage_Reset(MovingAverage_Data_t* ma)
{
    if (ma == NULL) {
        return;
    }

    ma->filtered_val = 0;

}
