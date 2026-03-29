#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H

#include "sensor_types.h"
#include "utils/adc_manager.h"
#include <stdbool.h>

#define SA_ZERO_MV 2500 // ADC zero point based on calibration
#define SA_SWING_MV 2500 // Voltage range in either direction
#define SA_MAX_MV 5000
#define SA_SWING_ANGLE 1800 // 180 degrees * 10

typedef struct {

    ADC_Data_t adc;

    int16_t delta;
    sa_type angle;

    bool valid;

    bool adc_err;

} SteeringAngle_Data_t;

void SteeringAngle_Init(SteeringAngle_Data_t* sa_data);
void SteeringAngle_Update(SteeringAngle_Data_t* sa_data, uint32_t adc_channel);
void SteeringAngle_SendCAN(SteeringAngle_Data_t* sa_data, uint32_t can_id);

#endif // STEERING_ANGLE_H
