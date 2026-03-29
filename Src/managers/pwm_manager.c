/**
  ******************************************************************************
  * @file           : pwm_manager.c
  * @brief          : Rad PWM manager implementation
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
#include "managers/pwm_manager.h"
#include "cmsis_os.h"
#include <stdbool.h>

/* Private Variables ---------------------------------------------------------*/
static const uint8_t TEMP_POINTS[] = {10, 22, 27, 30, 35, 45, 50}; // Temperature points in Celsius
static const uint16_t PWM_VALUES[] = {0, 0, 256, 512, 768, 1023, 1023}; // Corresponding PWM values
static const size_t NUM_POINTS = sizeof(TEMP_POINTS) / sizeof(TEMP_POINTS[0]);

/* Private Function Prototypes -----------------------------------------------*/
static int8_t PWM_GetInvTemp(void);
static uint16_t PWM_CalculateValue(int8_t temp);

/* Public Variables ----------------------------------------------------------*/
bool PWM_ACTIVE = false;
int8_t* INV_TEMP_PTR = NULL;
int8_t inv_temp;
osMutexId_t inv_temp_mutex = NULL;

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Main PWM manager task
  * @param  argument: Not used
  * @retval None
  */
void PWM_ManagerTask(void *argument)
{
	if (PWM_ACTIVE) {
		uint32_t last_update_tick = 0;
		uint32_t current_tick = 0;
		int8_t temp = 0;
		uint16_t pwm_value = 0;

		last_update_tick = osKernelGetTickCount();

		for(;;)
		{
			current_tick = osKernelGetTickCount();

			if (current_tick - last_update_tick >= PWM_UPDATE_INTERVAL_MS) {
				temp = PWM_GetInvTemp();
				pwm_value = PWM_CalculateValue(temp);

				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, pwm_value);

				last_update_tick = current_tick;
			}

			osDelay(100);
		}
	}

}

/**
  * @brief  Configure and start PWM output
  * @retval None
  */
void PWM_Init(void)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_TIM_PWM_Init(&htim16);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;

	HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig);

	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_TIM16;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);

	const osMutexAttr_t mutex_attr = {
		.name = "InvTempMutex"
	};

	inv_temp_mutex = osMutexNew(&mutex_attr);

}

/**
  * @brief  Set inverter temp
  * @param  temp: Inverter temp to set
  * @retval None
  */
void PWM_SetInvTemp(int8_t temp)
{
	osMutexAcquire(inv_temp_mutex, osWaitForever);

	inv_temp = temp;

	osMutexRelease(inv_temp_mutex);

}

/**
  * @brief  Get inverter temp
  * @retval Inverter temp
  */
static int8_t PWM_GetInvTemp(void)
{
	int32_t temp;

	osMutexAcquire(inv_temp_mutex, osWaitForever);

	temp = inv_temp;

	osMutexRelease(inv_temp_mutex);

	return temp;

}

/**
  * @brief  Calculate PWM value based on temp reading
  * @param  temp: Inverter temp reading
  * @retval PWM value (0-1023)
  */
static uint16_t PWM_CalculateValue(int8_t temp)
{
	uint16_t pwm_value;

	if (temp < TEMP_POINTS[0]) {
		pwm_value = PWM_VALUES[0];
	} else if (temp >= TEMP_POINTS[NUM_POINTS - 1]) {
		pwm_value = PWM_VALUES[NUM_POINTS - 1];
	} else {
		for (int i = 0; i < NUM_POINTS - 1; i++) {
			if ((temp >= TEMP_POINTS[i]) && (temp < TEMP_POINTS[i+1])) {
				pwm_value = PWM_VALUES[i] + (uint16_t)((PWM_VALUES[i+1] - PWM_VALUES[i]) * (temp - TEMP_POINTS[i]) / (TEMP_POINTS[i+1] - TEMP_POINTS[i]));
				break;
			}
		}
	}

	return pwm_value;

}
