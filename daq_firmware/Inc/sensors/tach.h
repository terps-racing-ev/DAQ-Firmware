#ifndef TACH_H
#define TACH_H

#include "sensor_types.h"
#include "utils/interrupt_manager.h"
#include <stdbool.h>

#define TACH_PULSES_PER_ROTATION 2
#define TACH_TIMEOUT 1000000

typedef struct {

    Interrupt_Data_t interrupt;

    tach_type rpm;

    bool valid;

    bool timeout;

} Tach_Data_t;

void Tach_Init(Tach_Data_t* tach_data);
void Tach_Update(Tach_Data_t* tach_data);
void Tach_SendCAN(Tach_Data_t* tach_data, uint32_t can_id);

#endif // TACH_H
