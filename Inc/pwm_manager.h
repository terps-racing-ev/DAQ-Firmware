#ifndef PWM_MANAGER_H
#define PWM_MANAGER_H

#include "main.h"

#define PWM_UPDATE_INTERVAL_MS 1000

extern TIM_HandleTypeDef htim16;

void PWM_ManagerTask(void *argument);
void PWM_Init(void);
void PWM_SetInvTemp(int8_t temp);

#endif // PWM_MANAGER_H
