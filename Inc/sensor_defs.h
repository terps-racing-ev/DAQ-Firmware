/**
  ******************************************************************************
  * @file           : sensor_defs.h
  * @brief          : Definitions for sensor data types and error flags
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Sensor Data Types */
typedef int8_t ct_type; // -40 to 120 C

typedef uint16_t lp_type; // 0 to 500 mm*10

typedef uint16_t pitot_type; // mph*100

typedef int16_t sa_type; // -1800 to 1800 degrees*10

typedef uint16_t tach_type; // rpm

typedef uint16_t wsp_type; // mph*100

/* Sensor Error Flags */
#define ERROR_ADC_ERR 		(1 << 0)
#define ERROR_TIMEOUT 		(1 << 1)
#define ERROR_OUT_OF_RANGE 	(1 << 2)

#endif // SENSOR_TYPES_H
