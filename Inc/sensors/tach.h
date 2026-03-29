/**
  ******************************************************************************
  * @file           : tach.h
  * @brief          : Rad tachometer reader
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

#ifndef TACH_H
#define TACH_H

/* Includes ------------------------------------------------------------------*/
#include "sensor_types.h"
#include "managers/interrupt_manager.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define TACH_PULSES_PER_ROTATION 2
#define TACH_TIMEOUT_US 1000000

/* Tach Data Structure */
typedef struct {

    Interrupt_Data_t interrupt;

    tach_type rpm;

    bool valid;

    bool timeout;

} Tach_Data_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Initialize tach data
  * @param  tach_data: Pointer to tach data structure
  * @retval None
  */
void Tach_Init(Tach_Data_t* tach_data);

/**
  * @brief  Update tach value based on interrupt data
  * @param  tach_data: Pointer to tach data structure
  * @retval None
  */
void Tach_Update(Tach_Data_t* tach_data);

/**
  * @brief  Send tach data CAN message
  * @param  tach_data: Pointer to tach data structure
  * @param  can_id: CAN message ID
  * @retval None
  */
void Tach_SendCAN(Tach_Data_t* tach_data, uint32_t can_id);

#endif // TACH_H
