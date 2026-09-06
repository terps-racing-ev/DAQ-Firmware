/**
  ******************************************************************************
  * @file           : left_board.c
  * @brief          : DAQ back left board configuration
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
#include "boards/left_board.h"

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Configure DAQ back left board sensor list and interrupts
  * @retval None
  */
void DBL_Config(void)
{
	Sensor_t wsp_bl = {
	    .type = WHEEL_SPEED,
		.gpio_pin = WSP_BL_GPIO_PIN,
	    .wsp_data = {0},
	    .can_id = WSP_BL_CAN_ID
	};

	Sensor_t pitot_sidepod = {
	    .type = PITOT_TUBE,
		.adc_channel = PITOT_SIDEPOD_ADC_CHANNEL,
	    .pitot_data = {0},
	    .can_id = PITOT_SIDEPOD_CAN_ID
	};

	Sensor_t shock_bl = {
	    .type = LINEAR_POT,
		.adc_channel = SHOCK_BL_ADC_CHANNEL,
	    .lp_data = {0},
	    .can_id = SHOCK_BL_CAN_ID
	};

	Sensor_t pitot_fw = {
		.type = PITOT_TUBE,
		.adc_channel = PITOT_FW_ADC_CHANNEL,
		.pitot_data = {0},
		.can_id = PITOT_FW_CAN_ID
	};

	Sensor_t pitot_rw = {
		.type = PITOT_TUBE,
		.adc_channel = PITOT_RW_ADC_CHANNEL,
	    .pitot_data = {0},
	    .can_id = PITOT_RW_CAN_ID
	};

	Sensor_t pitot_nose = {
		.type = PITOT_TUBE,
		.adc_channel = PITOT_NOSE_ADC_CHANNEL,
		.pitot_data = {0},
		.can_id = PITOT_NOSE_CAN_ID
	};

	SensorList[0] = wsp_bl;
	Interrupt_ConfigPin(SensorList[0].gpio_pin, &SensorList[0].wsp_data.interrupt);

	SensorList[1] = pitot_sidepod;
	SensorList[2] = shock_bl;
	SensorList[3] = pitot_fw;
	SensorList[4] = pitot_rw;
	SensorList[5] = pitot_nose;

}
