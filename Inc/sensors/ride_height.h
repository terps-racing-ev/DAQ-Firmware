#pragma once
#include "moving_average.h"
#include <stdbool.h>

void RideHeight_Init(RideHeight_Data_t* rh_data);
void RideHeight_UpdateTask(RideHeight_Data_t* rh_data);


typedef struct {
    bool new_data;

    uint16_t dist;
    uint16_t strength;
    uint16_t temp;
} RideHeight_Data_t;