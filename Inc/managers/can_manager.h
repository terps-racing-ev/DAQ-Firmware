/**
  ******************************************************************************
  * @file           : can_manager.h
  * @brief          : CAN bus manager
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

#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define CAN_MESSAGE_SIZE 			8
#define CAN_TX_QUEUE_SIZE 			32
#define CAN_RX_QUEUE_SIZE 			8
#define CAN_TX_TIMEOUT_MS 			100
#define CAN_HEARTBEAT_INTERVAL_MS 	1000

#define CAN_BOARD_ID_SHIFT     		20
/* Helper macro to create CAN ID with board ID */
#define CAN_ID(base, board)    ((base) | ((board) << CAN_BOARD_ID_SHIFT))

#define CAN_SET_BOARD_ID_CMD 	0x0D000000
#define CAN_RESET_CMD_BASE 		0x0D000001
#define CAN_STATUS_BASE			0x0D000000

/* External Variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern uint32_t RESET_CAN_ID;
extern uint32_t STATUS_CAN_ID;

/* CAN Message Structure */
typedef struct {

    uint32_t id;
    uint8_t data[CAN_MESSAGE_SIZE];

} CAN_Message_t;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Main CAN manager task
  * @param  argument: Not used
  * @retval None
  */
void CAN_ManagerTask(void *argument);

/**
  * @brief  Initialize CAN manager
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef CAN_Manager_Init(void);

/**
  * @brief  Add message to TX queue
  * @param  msg: Pointer to CAN message
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef CAN_SendMessage(CAN_Message_t* msg);

#endif // CAN_MANAGER_H
