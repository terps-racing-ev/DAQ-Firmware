#ifndef LINEAR_POT_H
#define LINEAR_POT_H

#include "sensor_types.h"
#include "utils/adc_manager.h"
#include <stdbool.h>

#define LP_MAX_RANGE 500 // mm*10

typedef struct {

    ADC_Data_t adc;

    lp_type dist_scaled;

    bool valid;

    bool adc_err;

} LinearPot_Data_t;

void LinearPot_Init(LinearPot_Data_t* lp_data);
void LinearPot_Update(LinearPot_Data_t* lp_data, uint32_t adc_channel);
void LinearPot_SendCAN(LinearPot_Data_t* lp_data, uint32_t can_id);

#endif // LINEAR_POT_H
