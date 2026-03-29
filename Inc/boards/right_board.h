#ifndef RIGHT_BOARD_H
#define RIGHT_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sensor_manager.h"

/* Defines -------------------------------------------------------------------*/
#define DBR_BOARD_ID 			0x0C
#define DBR_NUM_SENSORS 		6

/* Pinout --------------------------------------------------------------------*/
#define WSP_BR_GPIO_PIN 	 	GPIO_PIN_0
#define SHOCK_BR_ADC_CHANNEL	ADC_CHANNEL_8
#define CT_MOTOR_ADC_CHANNEL 	ADC_CHANNEL_9
#define CT_INV_ADC_CHANNEL 		ADC_CHANNEL_10
#define TACH_R_GPIO_PIN 		GPIO_PIN_1
#define RAD_PWM_GPIO_PIN 		GPIO_PIN_6

/* CAN IDs -------------------------------------------------------------------*/
#define DBR_STATUS_CAN_ID 		0x0DC00000
#define WSP_BR_CAN_ID 			0x0DC10000
#define SHOCK_BR_CAN_ID 		0x0DC20000
#define CT_MOTOR_CAN_ID 		0x0DC50000
#define CT_INV_CAN_ID 			0x0DC51000
#define TACH_R_CAN_ID 			0x0DC60000

/* External Variables --------------------------------------------------------*/
extern Sensor_t SensorList[MAX_NUM_SENSORS];
extern bool PWM_ACTIVE;
extern int8_t* INV_TEMP_PTR;

/* Function Prototypes -------------------------------------------------------*/
void DBR_Config(void);

#endif // RIGHT_BOARD_H
