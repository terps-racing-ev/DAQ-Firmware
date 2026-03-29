#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <stdint.h>

typedef int8_t ct_type; // -40 to 120 C

typedef uint16_t lp_type; // 0 to 500 mm*10

typedef uint16_t pitot_type; // mph*100

typedef int16_t sa_type; // -1800 to 1800 degrees*10

typedef uint16_t tach_type; // rpm

typedef uint16_t wsp_type; // mph*100

#endif // SENSOR_TYPES_H
