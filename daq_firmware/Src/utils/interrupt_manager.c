#include "utils/interrupt_manager.h"

static Interrupt_t InterruptList[MAX_NUM_INTERRUPTS] = {0};
static uint8_t NUM_INTERRUPTS = 0;

static void Interrupt_Handler(Interrupt_Data_t* int_data);

void Interrupt_ManagerTask(void *argument)
{
	uint32_t awaited_flags;
	uint32_t set_flags;

	awaited_flags = (uint32_t)((1U << NUM_INTERRUPTS) - 1U);

	for(;;)
	{
		set_flags = osThreadFlagsWait(awaited_flags, osFlagsWaitAny, osWaitForever);

		for (int i = 0; i < NUM_INTERRUPTS; i++) {
			if ((set_flags >> i) & 1U) {
				Interrupt_Handler(InterruptList[i].int_data);
			}
		}

		osDelay(2);
	}
}

void Interrupt_Config(uint16_t gpio_pin, Interrupt_Data_t* int_data)
{
	InterruptList[NUM_INTERRUPTS].gpio_pin = gpio_pin;
	InterruptList[NUM_INTERRUPTS].int_data = int_data;
	InterruptList[NUM_INTERRUPTS].int_data->mutex = osMutexNew(NULL);
	NUM_INTERRUPTS++;

}

void Interrupt_Init(Interrupt_Data_t* int_data)
{
	osMutexAcquire(int_data->mutex, osWaitForever);

	int_data->delta = 0;
    int_data->last_pulse_time = 0;
    int_data->ticks = 0;

    MovingAverage_Init(&int_data->ma, WINDOW_SIZE);

    osMutexRelease(int_data->mutex);

}

static void Interrupt_Handler(Interrupt_Data_t* int_data)
{
	osMutexAcquire(int_data->mutex, osWaitForever);

	uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);
    int_data->delta = now - int_data->last_pulse_time;
    int_data->last_pulse_time = now;
    int_data->ticks++;

    int_data->avg_delta = MovingAverage_Update(&int_data->ma, int_data->delta);

    osMutexRelease(int_data->mutex);

}

void Interrupt_Reset(Interrupt_Data_t* int_data)
{
	osMutexAcquire(int_data->mutex, osWaitForever);

	int_data->delta = 0;
	int_data->last_pulse_time = __HAL_TIM_GET_COUNTER(&htim2);
	int_data->ticks = 0;

	MovingAverage_Reset(&int_data->ma);

	osMutexRelease(int_data->mutex);

}

uint32_t Interrupt_GetLastPulseTime(Interrupt_Data_t* int_data)
{
	uint32_t last_pulse_time;

	osMutexAcquire(int_data->mutex, osWaitForever);

	last_pulse_time = int_data->last_pulse_time;

	osMutexRelease(int_data->mutex);

	return last_pulse_time;

}

uint32_t Interrupt_GetAverageDelta(Interrupt_Data_t* int_data)
{
	uint32_t avg_delta;

	osMutexAcquire(int_data->mutex, osWaitForever);

	avg_delta = int_data->avg_delta;

	osMutexRelease(int_data->mutex);

	return avg_delta;

}

uint32_t Interrupt_GetTicks(Interrupt_Data_t* int_data)
{
	uint32_t ticks;

	osMutexAcquire(int_data->mutex, osWaitForever);

	ticks = int_data->ticks;

	osMutexRelease(int_data->mutex);

	return ticks;

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t flags;

	for (int i = 0; i < NUM_INTERRUPTS; i++) {
		if (GPIO_Pin == InterruptList[i].gpio_pin) {
			flags = (uint32_t)(1U << i);
			osThreadFlagsSet(Interrupt_ManagerHandle, flags);
		}
	}

}
