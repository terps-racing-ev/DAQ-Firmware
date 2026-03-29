#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include "main.h"
#include "cmsis_os.h"
#include "utils/moving_average.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_NUM_INTERRUPTS 6

extern TIM_HandleTypeDef htim2;
extern osThreadId_t Interrupt_ManagerHandle;

typedef struct {

	osMutexId_t mutex;

	uint32_t last_pulse_time;
	uint32_t delta;
	uint32_t ticks;

	MovingAverage_Data_t ma;

    uint32_t avg_delta;

} Interrupt_Data_t;

typedef struct {

	uint16_t gpio_pin;
	Interrupt_Data_t* int_data;

} Interrupt_t;

void Interrupt_ManagerTask(void *argument);
void Interrupt_Config(uint16_t gpio_pin, Interrupt_Data_t* int_data);
void Interrupt_Init(Interrupt_Data_t* int_data);
void Interrupt_Reset(Interrupt_Data_t* int_data);
uint32_t Interrupt_GetLastPulseTime(Interrupt_Data_t* int_data);
uint32_t Interrupt_GetAverageDelta(Interrupt_Data_t* int_data);
uint32_t Interrupt_GetTicks(Interrupt_Data_t* int_data);

#endif // INTERRUPT_MANAGER_H
