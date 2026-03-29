#ifndef PITOT_TUBE_H
#define PITOT_TUBE_H

#include "sensor_types.h"
#include "utils/adc_manager.h"
#include <stdbool.h>

#define PITOT_CALIBRATION_READINGS 100

typedef struct {

    ADC_Data_t adc;

    uint32_t calibration_sum;
    uint16_t calibration_cnt;
    uint16_t zero_point;

    int16_t pressure;
    pitot_type vel_scaled;

    bool valid;

    bool adc_err;

} PitotTube_Data_t;

void PitotTube_Init(PitotTube_Data_t* pitot_data);
void PitotTube_Update(PitotTube_Data_t* pitot_data, uint32_t adc_channel);
void PitotTube_SendCAN(PitotTube_Data_t* pitot_data, uint32_t can_id);

#endif // PITOT_TUBE_H
