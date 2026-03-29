/* Includes ------------------------------------------------------------------*/
#include "boards/front_board.h"
#include "utils/interrupt_manager.h"

/* Function Implementations --------------------------------------------------*/
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

	SensorList[0] = wsp_fl;
	Interrupt_Config(SensorList[0].gpio_pin, &SensorList[0].wsp_data.interrupt);

	SensorList[1] = wsp_fr;
	Interrupt_Config(SensorList[1].gpio_pin, &SensorList[1].wsp_data.interrupt);

	SensorList[2] = shock_fl;
	SensorList[3] = shock_fr;
	SensorList[4] = steering_angle;
	SensorList[5] = pitot;

}
