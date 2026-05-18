/**
  ******************************************************************************
  * @file           : front_board.c
  * @brief          : DAQ front board configuration
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
#include "boards/front_board.h"

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Configure DAQ front board sensor list and interrupts
  * @retval None
  */
void DBF_Config(void)
{
	Sensor_t wsp_fl = {
	    .type = WHEEL_SPEED,
		.gpio_pin = WSP_FL_GPIO_PIN,
	    .wsp_data = {0},
	    .can_id = WSP_FL_CAN_ID
	};

	Sensor_t wsp_fr = {
		.type = WHEEL_SPEED,
		.gpio_pin = WSP_FR_GPIO_PIN,
	    .wsp_data = {0},
	    .can_id = WSP_FR_CAN_ID
	};

	Sensor_t shock_fl = {
		.type = LINEAR_POT,
		.adc_channel = SHOCK_FL_ADC_CHANNEL,
	    .lp_data = {0},
	    .can_id = SHOCK_FL_CAN_ID
	};

	Sensor_t shock_fr = {
		.type = LINEAR_POT,
		.adc_channel = SHOCK_FR_ADC_CHANNEL,
	    .lp_data = {0},
	    .can_id = SHOCK_FR_CAN_ID
	};

	Sensor_t steering_angle = {
		.type = STEERING_ANGLE,
		.adc_channel = STEERING_ANGLE_ADC_CHANNEL,
	    .sa_data = {0},
	    .can_id = STEERING_ANGLE_CAN_ID
	};

	Sensor_t pitot = {
		.type = PITOT_TUBE,
		.adc_channel = PITOT_ADC_CHANNEL,
	    .pitot_data = {0},
	    .can_id = PITOT_CAN_ID
	};

	Sensor_t odo = {
		.type = ODO,
		.odo_data = {0},
		.can_id = F_ODO_LR_CAN_ID
	};

	SensorList[0] = wsp_fl;
	Interrupt_Config(SensorList[0].gpio_pin, &SensorList[0].wsp_data.interrupt);

	SensorList[1] = wsp_fr;
	Interrupt_Config(SensorList[1].gpio_pin, &SensorList[1].wsp_data.interrupt);

	SensorList[2] = shock_fl;
	SensorList[3] = shock_fr;
	SensorList[4] = steering_angle;
	SensorList[5] = pitot;
	
	SensorList[6] = odo;
	SensorList[6].odo_data.l_data = &SensorList[0].wsp_data;
	SensorList[6].odo_data.r_data = &SensorList[1].wsp_data;
}
