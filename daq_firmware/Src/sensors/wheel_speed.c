#include "sensors/wheel_speed.h"
#include "utils/can_manager.h"
#include <math.h>

static const uint32_t US_PER_MIN = 60000000;
static const uint32_t US_PER_HOUR = 3600000000;
static const uint32_t INCHES_PER_MILE = 63360;

void WheelSpeed_Init(WheelSpeed_Data_t* wsp_data)
{
    Interrupt_Init(&wsp_data->interrupt);

    wsp_data->rpm = 0;
    wsp_data->mph = 0;
    wsp_data->valid = false;

}

void WheelSpeed_Update(WheelSpeed_Data_t* wsp_data)
{
	uint32_t last_pulse_time;
	uint32_t now;
	uint32_t avg_delta;

    wsp_data->valid = false;

    last_pulse_time = Interrupt_GetLastPulseTime(&wsp_data->interrupt);

    now = __HAL_TIM_GET_COUNTER(&htim2);
    if (now - last_pulse_time > WSP_TIMEOUT) {
        Interrupt_Reset(&wsp_data->interrupt);
        wsp_data->timeout = true;
        return;
    }

    wsp_data->valid = true;

    avg_delta = Interrupt_GetAverageDelta(&wsp_data->interrupt);

    if (avg_delta) {
    	wsp_data->rpm = (wsp_type) (US_PER_MIN / avg_delta / SPOKES);
    	wsp_data->mph = (wsp_type) (100 * 2 * M_PI * ROLLING_RADIUS * US_PER_HOUR / avg_delta / SPOKES / INCHES_PER_MILE); // mph*100
    }

}

static void PackWSPData(WheelSpeed_Data_t* wsp_data, CAN_Message_t* msg)
{
    uint32_t avg_delta = Interrupt_GetAverageDelta(&wsp_data->interrupt);

	msg->data[0] = avg_delta & 0xFF;
    msg->data[1] = (avg_delta >> 8) & 0xFF;
    msg->data[2] = (avg_delta >> 16) & 0xFF;
    msg->data[3] = avg_delta >> 24;
    msg->data[4] = wsp_data->rpm & 0xFF;
    msg->data[5] = wsp_data->rpm >> 8;
    msg->data[6] = wsp_data->mph & 0xFF;
    msg->data[7] = wsp_data->mph >> 8;

}

void WheelSpeed_SendCAN(WheelSpeed_Data_t* wsp_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PackWSPData(wsp_data, &msg);
	CAN_SendMessage(&msg);

}
