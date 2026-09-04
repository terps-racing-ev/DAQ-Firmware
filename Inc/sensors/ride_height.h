#pragma once
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal_uart.h"
#include "can_manager.h"
#include "stdio.h"
#include "cmsis_os.h"
#include <string.h>

typedef struct {
    bool new_data;
    
    uint16_t dist;
    uint16_t strength;
    uint16_t temp;
} RideHeight_Data_t;

void RideHeight_Init();
void RideHeight_Task(void *argument);
// void RideHeight_UpdateTask(RideHeight_Data_t* rh_data);
