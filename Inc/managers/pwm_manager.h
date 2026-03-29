/**
  ******************************************************************************
  * @file           : pwm_manager.h
  * @brief          : Rad PWM manager
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

#ifndef PWM_MANAGER_H
#define PWM_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define PWM_UPDATE_INTERVAL_MS 1000

/* External Variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim16;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Main PWM manager task
  * @param  argument: Not used
  * @retval None
  */
void PWM_ManagerTask(void *argument);

/**
  * @brief  Configure and start PWM output
  * @retval None
  */
void PWM_Init(void);

/**
  * @brief  Set inverter temp
  * @param  temp: Inverter temp to set
  * @retval None
  */
void PWM_SetInvTemp(int8_t temp);

#endif // PWM_MANAGER_H
