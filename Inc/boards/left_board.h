#ifndef LEFT_BOARD_H
#define LEFT_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sensor_manager.h"

/* Defines -------------------------------------------------------------------*/
#define DBL_BOARD_ID 			0x0B
#define DBL_NUM_SENSORS 		6

/* Pinout --------------------------------------------------------------------*/
#define WSP_BL_GPIO_PIN 		GPIO_PIN_0
#define SHOCK_BL_ADC_CHANNEL 	ADC_CHANNEL_8
#define CT_SWIRL_ADC_CHANNEL 	ADC_CHANNEL_9
#define CT_RAD1_ADC_CHANNEL 	ADC_CHANNEL_10
#define CT_RAD2_ADC_CHANNEL 	ADC_CHANNEL_11
#define TACH_L_GPIO_PIN 		GPIO_PIN_1

/* CAN IDs -------------------------------------------------------------------*/
#define DBL_STATUS_CAN_ID 		0x0DB00000
#define WSP_BL_CAN_ID 			0x0DB10000
#define SHOCK_BL_CAN_ID 		0x0DB20000
#define CT_SWIRL_CAN_ID 		0x0DB50000
#define CT_RAD1_CAN_ID 			0x0DB51000
#define CT_RAD2_CAN_ID 			0x0DB52000
#define TACH_L_CAN_ID 			0x0DB60000

/* External Variables --------------------------------------------------------*/
extern Sensor_t SensorList[MAX_NUM_SENSORS];

/* Function Prototypes -------------------------------------------------------*/
void DBL_Config(void);

#endif // LEFT_BOARD_H
