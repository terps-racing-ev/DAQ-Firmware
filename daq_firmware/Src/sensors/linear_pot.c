#include "sensors/linear_pot.h"
#include "utils/can_manager.h"

void LinearPot_Init(LinearPot_Data_t* lp_data)
{
	ADC_Init(&lp_data->adc);

    lp_data->dist_scaled = 0;
    lp_data->valid = false;

}

void LinearPot_Update(LinearPot_Data_t* lp_data, uint32_t adc_channel)
{
    lp_data->valid = false;
    lp_data->adc_err = false;

    if (ADC_Update(&lp_data->adc, adc_channel) != HAL_OK) {
        lp_data->adc_err = true;
        return;
    }

    lp_data->valid = true;

    lp_data->dist_scaled = (lp_type) ((uint32_t)lp_data->adc.filt_mv * LP_MAX_RANGE / SUPPLY_VOLTAGE);

}

static void PackLPData(LinearPot_Data_t* lp_data, CAN_Message_t* msg)
{
    msg->data[0] = lp_data->adc.adc_value & 0xFF;
    msg->data[1] = lp_data->adc.adc_value >> 8;
    msg->data[2] = lp_data->adc.raw_mv & 0xFF;
    msg->data[3] = lp_data->adc.raw_mv >> 8;
    msg->data[4] = lp_data->adc.filt_mv & 0xFF;
    msg->data[5] = lp_data->adc.filt_mv >> 8;
    msg->data[6] = lp_data->dist_scaled & 0xFF;
    msg->data[7] = lp_data->dist_scaled >> 8;
}

void LinearPot_SendCAN(LinearPot_Data_t* lp_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PackLPData(lp_data, &msg);
	CAN_SendMessage(&msg);

}
