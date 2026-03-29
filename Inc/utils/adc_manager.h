#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include "main.h"
#include "utils/moving_average.h"
#include <stdint.h>

#define ADC_TIMEOUT 1 // ms
#define SUPPLY_VOLTAGE 5000 // mV
#define VOLTAGE_DIVIDER_SCALE_FACTOR 2

extern ADC_HandleTypeDef hadc1;

typedef struct {

	uint16_t adc_value;
	uint16_t raw_mv;
	uint16_t filt_mv;

	MovingAverage_Data_t ma;

} ADC_Data_t;

void ADC_Init(ADC_Data_t* adc_data);
HAL_StatusTypeDef ADC_Update(ADC_Data_t* adc_data, uint32_t adc_channel);
HAL_StatusTypeDef ADC_Read(uint32_t adc_channel, uint16_t* adc_value);
uint16_t ADC_ConvertToUnscaledMV(uint16_t adc_value);

#endif // ADC_MANAGER_H
