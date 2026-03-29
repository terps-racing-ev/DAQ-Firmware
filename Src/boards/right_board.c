/**
  ******************************************************************************
  * @file           : right_board.c
  * @brief          : DAQ back right board configuration
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
#include "managers/interrupt_manager.h"
#include "managers/pwm_manager.h"
#include "boards/right_board.h"

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Configure DAQ back right board sensor list and interrupts, activate PWM
  * @retval None
  */
void DBR_Config(void)
{
	Sensor_t wsp_br = {
		.type = WHEEL_SPEED,
		.gpio_pin = WSP_BR_GPIO_PIN,
		.wsp_data = {0},
		.can_id = WSP_BR_CAN_ID
	};

	Sensor_t shock_br = {
		.type = LINEAR_POT,
		.adc_channel = SHOCK_BR_ADC_CHANNEL,
	    .lp_data = {0},
	    .can_id = SHOCK_BR_CAN_ID
	};

	Sensor_t ct_motor = {
		.type = COOLANT_TEMP,
		.adc_channel = CT_MOTOR_ADC_CHANNEL,
		.ct_data = {0},
		.can_id = CT_MOTOR_CAN_ID
	};

	Sensor_t ct_inv = {
		.type = COOLANT_TEMP,
		.adc_channel = CT_INV_ADC_CHANNEL,
		.ct_data = {0},
		.can_id = CT_INV_CAN_ID
	};

	Sensor_t tach_r = {
		.type = TACH,
		.gpio_pin = TACH_R_GPIO_PIN,
		.tach_data = {0},
		.can_id = TACH_R_CAN_ID
	};

	SensorList[0] = wsp_br;
	Interrupt_Config(SensorList[0].gpio_pin, &SensorList[0].wsp_data.interrupt);

	SensorList[1] = shock_br;
	SensorList[2] = ct_motor;
	SensorList[3] = ct_inv;

	SensorList[4] = tach_r;
	Interrupt_Config(SensorList[4].gpio_pin, &SensorList[4].tach_data.interrupt);

	PWM_ACTIVE = true;
	INV_TEMP_PTR = &SensorList[3].ct_data.temp;
	PWM_Init();

}
