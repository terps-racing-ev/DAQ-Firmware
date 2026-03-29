#include "sensors/steering_angle.h"
#include "utils/can_manager.h"

void SteeringAngle_Init(SteeringAngle_Data_t* sa_data)
{
	ADC_Init(&sa_data->adc);

    sa_data->angle = 0;
    sa_data->valid = false;

}

void SteeringAngle_Update(SteeringAngle_Data_t* sa_data, uint32_t adc_channel)
{
    sa_data->valid = false;
    sa_data->adc_err = false;

    if (ADC_Update(&sa_data->adc, adc_channel) != HAL_OK) {
        sa_data->adc_err = true;
        return;
    }

    sa_data->valid = true;

    sa_data->delta = sa_data->adc.filt_mv - SA_ZERO_MV;

    // Wrap around
    if (sa_data->delta > SA_SWING_MV) {
        sa_data->delta -= SA_MAX_MV;
    } else if (sa_data->delta < -1 * SA_SWING_MV) {
        sa_data->delta += SA_MAX_MV;
    }

    sa_data->angle = (sa_type) ((int32_t)sa_data->delta * SA_SWING_ANGLE / SA_SWING_MV);

}

static void PackSAData(SteeringAngle_Data_t* sa_data, CAN_Message_t* msg)
{
    msg->data[0] = sa_data->adc.adc_value & 0xFF;
    msg->data[1] = sa_data->adc.adc_value >> 8;
    msg->data[2] = sa_data->adc.raw_mv & 0xFF;
    msg->data[3] = sa_data->adc.raw_mv >> 8;
    msg->data[4] = sa_data->adc.filt_mv & 0xFF;
    msg->data[5] = sa_data->adc.filt_mv >> 8;
    msg->data[6] = (uint8_t)(sa_data->angle & 0xFF);
    msg->data[7] = (uint8_t)(sa_data->angle >> 8);

}

void SteeringAngle_SendCAN(SteeringAngle_Data_t* sa_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PackSAData(sa_data, &msg);
	CAN_SendMessage(&msg);

}
