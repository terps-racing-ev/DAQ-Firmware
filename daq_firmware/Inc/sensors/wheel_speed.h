#ifndef WHEEL_SPEED_H
#define WHEEL_SPEED_H

#include "sensor_types.h"
#include "utils/interrupt_manager.h"
#include <stdbool.h>

/*
Some realistic times between pulses:
1mph ~200,000us
40mph ~5,000us
100mph ~2,000us
*/

#define ROLLING_RADIUS 9 // Inches
#define SPOKES 16
#define WSP_TIMEOUT 1000000

typedef struct {

    Interrupt_Data_t interrupt;

    wsp_type rpm;
    wsp_type mph;

    bool valid;

    bool timeout;

} WheelSpeed_Data_t;

void WheelSpeed_Init(WheelSpeed_Data_t* wsp_data);
void WheelSpeed_Update(WheelSpeed_Data_t* wsp_data);
void WheelSpeed_SendCAN(WheelSpeed_Data_t* wsp_data, uint32_t can_id);

#endif // WHEEL_SPEED_H
