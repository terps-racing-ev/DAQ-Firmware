#include "sensors/coolant_temp.h"
#include "utils/can_manager.h"
#include <math.h>

// Steinhart equation constants, see MATLAB file to adjust
static const float A = -0.0009123445192;
static const float B = 0.0005229826803;
static const float C = -0.0000007045640079;

void CoolantTemp_Init(CoolantTemp_Data_t* ct_data)
{
	ADC_Init(&ct_data->adc);

    ct_data->temp = 0;
    ct_data->valid = false;

}

void CoolantTemp_Update(CoolantTemp_Data_t* ct_data, uint32_t adc_channel)
{
    float resistance, lnr, tempC;

    ct_data->valid = false;
    ct_data->out_of_range = false;
    ct_data->adc_err = false;

    if (ADC_Update(&ct_data->adc, adc_channel) != HAL_OK) {
        ct_data->adc_err = true;
        return;
    }

    if (ct_data->adc.filt_mv < CT_MIN_MV || ct_data->adc.filt_mv > CT_MAX_MV) {
        ct_data->out_of_range = true;
        return;
    }

    ct_data->valid = true;

    resistance = (float)ct_data->adc.filt_mv * CT_PULLUP_RESISTOR / (SUPPLY_VOLTAGE - (float)ct_data->adc.filt_mv);
    lnr = log(resistance);
    tempC = ( 1.0 / (A + B*lnr + C*pow(lnr, 3)) ) - 273.15;

    ct_data->temp = (ct_type) tempC;

}

static void PackCTData(CoolantTemp_Data_t* ct_data, CAN_Message_t* msg)
{
    msg->data[0] = ct_data->adc.adc_value & 0xFF;
    msg->data[1] = ct_data->adc.adc_value >> 8;
    msg->data[2] = ct_data->adc.raw_mv & 0xFF;
    msg->data[3] = ct_data->adc.raw_mv >> 8;
    msg->data[4] = ct_data->adc.filt_mv & 0xFF;
    msg->data[5] = ct_data->adc.filt_mv >> 8;
    msg->data[6] = ct_data->temp & 0xFF;
    msg->data[7] = ct_data->temp >> 8;

}

void CoolantTemp_SendCAN(CoolantTemp_Data_t* ct_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PackCTData(ct_data, &msg);
	CAN_SendMessage(&msg);

}
