/**
  ******************************************************************************
  * @file           : moving_average.h
  * @brief          : Moving average calculator
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

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>

/* Defines -------------------------------------------------------------------*/
#define WINDOW_SIZE 8

/* Moving Average Data Structure */
typedef struct {

    float weight;
    float filtered_val;

} MovingAverage_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize moving average data
  * @param  ma: Pointer to moving average data structure
  * @param  window_size: Moving average window size
  * @retval None
  */
void MovingAverage_Init(MovingAverage_Data_t* ma, uint8_t window_size);

/**
  * @brief  Update moving average with new value
  * @param  ma: Pointer to moving average data structure
  * @param  new_val: New value to add
  * @retval New average value
  */
uint32_t MovingAverage_Update(MovingAverage_Data_t* ma, uint32_t new_val);

/**
  * @brief  Reset moving average data
  * @param  ma: Pointer to moving average structure
  * @retval None
  */
void MovingAverage_Reset(MovingAverage_Data_t* ma);

#endif // MOVING_AVERAGE_H
