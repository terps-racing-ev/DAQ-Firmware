/* Includes ------------------------------------------------------------------*/
#include "boards/left_board.h"
#include "utils/interrupt_manager.h"

/* Function Implementations --------------------------------------------------*/
void DBL_Config(void)
{
	Sensor_t wsp_bl = {
	    .type = WHEEL_SPEED,
		.gpio_pin = WSP_BL_GPIO_PIN,
	    .wsp_data = {0},
	    .can_id = WSP_BL_CAN_ID
	};

	Sensor_t shock_bl = {
	    .type = LINEAR_POT,
		.adc_channel = SHOCK_BL_ADC_CHANNEL,
	    .lp_data = {0},
	    .can_id = SHOCK_BL_CAN_ID
	};

	Sensor_t ct_swirl = {
		.type = COOLANT_TEMP,
		.adc_channel = CT_SWIRL_ADC_CHANNEL,
		.ct_data = {0},
		.can_id = CT_SWIRL_CAN_ID
	};

	Sensor_t ct_rad1 = {
		.type = COOLANT_TEMP,
		.adc_channel = CT_RAD1_ADC_CHANNEL,
	    .ct_data = {0},
	    .can_id = CT_RAD1_CAN_ID
	};

	Sensor_t ct_rad2 = {
		.type = COOLANT_TEMP,
		.adc_channel = CT_RAD2_ADC_CHANNEL,
		.ct_data = {0},
		.can_id = CT_RAD2_CAN_ID
	};

	Sensor_t tach_l = {
		.type = TACH,
		.gpio_pin = TACH_L_GPIO_PIN,
		.tach_data = {0},
		.can_id = TACH_L_CAN_ID
	};

	SensorList[0] = wsp_bl;
	Interrupt_Config(SensorList[0].gpio_pin, &SensorList[0].wsp_data.interrupt);

	SensorList[1] = shock_bl;
	SensorList[2] = ct_swirl;
	SensorList[3] = ct_rad1;
	SensorList[4] = ct_rad2;

	SensorList[5] = tach_l;
	Interrupt_Config(SensorList[5].gpio_pin, &SensorList[5].tach_data.interrupt);

}
