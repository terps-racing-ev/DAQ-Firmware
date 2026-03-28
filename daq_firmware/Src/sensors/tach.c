#include "sensors/tach.h"
#include "utils/can_manager.h"

static const uint32_t US_PER_MIN = 60000000;

void Tach_Init(Tach_Data_t* tach_data)
{
	Interrupt_Init(&tach_data->interrupt);

	tach_data->rpm = 0;
	tach_data->valid = false;
}

void Tach_Update(Tach_Data_t* tach_data)
{
	uint32_t last_pulse_time;
	uint32_t now;
	uint32_t avg_delta;

    tach_data->valid = false;

    last_pulse_time = Interrupt_GetLastPulseTime(&tach_data->interrupt);

    now = __HAL_TIM_GET_COUNTER(&htim2);
    if (now - last_pulse_time > TACH_TIMEOUT) {
        Interrupt_Reset(&tach_data->interrupt);
        tach_data->timeout = true;
        return;
    }

    tach_data->valid = true;

    avg_delta = Interrupt_GetAverageDelta(&tach_data->interrupt);

    if (avg_delta) {
    	tach_data->rpm = (tach_type) (US_PER_MIN / avg_delta / TACH_PULSES_PER_ROTATION);
    }

}

static void PackTachData(Tach_Data_t* tach_data, CAN_Message_t* msg)
{
	uint32_t avg_delta = Interrupt_GetAverageDelta(&tach_data->interrupt);

	msg->data[0] = avg_delta & 0xFF;
	msg->data[1] = (avg_delta >> 8) & 0xFF;
	msg->data[2] = (avg_delta >> 16) & 0xFF;
	msg->data[3] = avg_delta >> 24;
	msg->data[4] = tach_data->rpm & 0xFF;
	msg->data[5] = tach_data->rpm >> 8;
	msg->data[6] = 0;
	msg->data[7] = 0;

}

void Tach_SendCAN(Tach_Data_t* tach_data, uint32_t can_id)
{
	CAN_Message_t msg;

	msg.id = can_id;
	PackTachData(tach_data, &msg);
	CAN_SendMessage(&msg);

}
