#ifndef COOLANT_TEMP_H
#define COOLANT_TEMP_H

#include "sensor_types.h"
#include "utils/adc_manager.h"
#include <stdbool.h>

#define CT_PULLUP_RESISTOR 1000
// Value beyond these will output out of range temps
// Determined using the Steinhart equation
#define CT_MIN_MV 534 // -40C
#define CT_MAX_MV 4853 // 120C

typedef struct {

    ADC_Data_t adc;

    ct_type temp;

    bool valid;

    bool adc_err;
    bool out_of_range;

} CoolantTemp_Data_t;

void CoolantTemp_Init(CoolantTemp_Data_t* ct_data);
void CoolantTemp_Update(CoolantTemp_Data_t* ct_data, uint32_t adc_channel);
void CoolantTemp_SendCAN(CoolantTemp_Data_t* ct_data, uint32_t can_id);

#endif // COOLANT_TEMP_H
