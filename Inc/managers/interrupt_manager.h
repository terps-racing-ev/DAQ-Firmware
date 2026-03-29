/**
  ******************************************************************************
  * @file           : interrupt_manager.h
  * @brief          : Interrupt manager
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

#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "utils/moving_average.h"
#include <stdint.h>
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define MAX_NUM_INTERRUPTS 6

/* External Variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern osThreadId_t Interrupt_ManagerHandle;

/* Interrupt Data Structure */
typedef struct {

	osMutexId_t mutex;

	uint32_t last_pulse_time;
	uint32_t delta;
	uint32_t ticks;

	MovingAverage_Data_t ma;

    uint32_t avg_delta;

} Interrupt_Data_t;

/* Interrupt Structure */
typedef struct {

	uint16_t gpio_pin;
	Interrupt_Data_t* int_data;

} Interrupt_t;

/* Function Prototypes --------------------------------------------------------*/

/**
  * @brief  Main interrupt manager task
  * @param  argument: Not used
  * @retval None
  */
void Interrupt_ManagerTask(void *argument);

/**
  * @brief  Add interrupt to interrupt list
  * @param  gpio_pin: GPIO pin triggering the interrupt
  * @param  int_data: Pointer to data structure for the interrupt
  * @retval None
  */
void Interrupt_Config(uint16_t gpio_pin, Interrupt_Data_t* int_data);

/**
  * @brief  Initialize interrupt data and moving average
  * @param  int_data: Pointer to interrupt data structure
  * @retval None
  */
void Interrupt_Init(Interrupt_Data_t* int_data);

/**
  * @brief  Reset interrupt data and moving average
  * @param  int_data: Pointer to interrupt data structure
  * @retval None
  */
void Interrupt_Reset(Interrupt_Data_t* int_data);

/**
  * @brief  Get last pulse time
  * @param  int_data: Pointer to interrupt data structure
  * @retval Last pulse time
  */
uint32_t Interrupt_GetLastPulseTime(Interrupt_Data_t* int_data);

/**
  * @brief  Get average delta
  * @param  int_data: Pointer to interrupt data structure
  * @retval Average delta
  */
uint32_t Interrupt_GetAverageDelta(Interrupt_Data_t* int_data);

/**
  * @brief  Get ticks
  * @param  int_data: Pointer to interrupt data structure
  * @retval Ticks
  */
uint32_t Interrupt_GetTicks(Interrupt_Data_t* int_data);

#endif // INTERRUPT_MANAGER_H
