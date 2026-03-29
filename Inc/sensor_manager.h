#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "sensors/coolant_temp.h"
#include "sensors/linear_pot.h"
#include "sensors/pitot_tube.h"
#include "sensors/steering_angle.h"
#include "sensors/tach.h"
#include "sensors/wheel_speed.h"

#define MAX_NUM_SENSORS 6 // per board
#define SENSOR_UPDATE_INTERVAL_MS 50
#define SENSOR_CAN_SEND_INTERVAL_MS 100

extern uint8_t NUM_SENSORS;
extern int8_t* INV_TEMP_PTR;

typedef enum {
	COOLANT_TEMP,
	LINEAR_POT,
	PITOT_TUBE,
	STEERING_ANGLE,
	TACH,
	WHEEL_SPEED
} SensorType;

typedef struct Sensor_t {

    SensorType type;

	union {
        uint32_t adc_channel;
        uint16_t gpio_pin;
    };

    union {
        CoolantTemp_Data_t ct_data;
    	LinearPot_Data_t lp_data;
        PitotTube_Data_t pitot_data;
        SteeringAngle_Data_t sa_data;
        Tach_Data_t tach_data;
        WheelSpeed_Data_t wsp_data;
    };

    uint32_t can_id;

} Sensor_t;

void Sensor_ManagerTask(void *argument);
void Sensor_Init(Sensor_t* sensor);
void Sensor_Update(Sensor_t* sensor);
void Sensor_SendCAN(Sensor_t* sensor);

#endif // SENSOR_MANAGER_H
