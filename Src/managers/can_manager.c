/**
  ******************************************************************************
  * @file           : can_manager.c
  * @brief          : CAN bus manager implementation
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

/* Includes ------------------------------------------------------------------*/
#include "managers/can_manager.h"
#include "cmsis_os.h"
#include <stdbool.h>

/* External Function Prototypes ----------------------------------------------*/
int8_t Config_SetBoardID(uint8_t new_board_id);
uint8_t Config_GetBoardID(void);

/* Private Variables ---------------------------------------------------------*/
osMessageQueueId_t CANTxQueueHandle = NULL;
osMessageQueueId_t CANRxQueueHandle = NULL;

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t CAN_GetNotificationFlags(void);
static void CAN_ProcessRxMessage(CAN_Message_t *msg);
static void CAN_ProcessTxQueue(void);
static HAL_StatusTypeDef CAN_TxMessage(CAN_Message_t* msg);
static HAL_StatusTypeDef CAN_SendHeartbeat(void);
static bool CAN_IsMessageForThisBoard(uint32_t can_id);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Main CAN manager task
  * @param  argument: Not used
  * @retval None
  */
void CAN_ManagerTask(void *argument)
{
    CAN_Message_t rx_msg;
    uint32_t last_heartbeat_tick = 0;
    uint32_t current_tick = 0;

    // Wait 100ms for system to stabilize
    osDelay(100);

    // Initialize timestamp
    last_heartbeat_tick = osKernelGetTickCount();

    /* Infinite loop */
    for(;;)
    {
        current_tick = osKernelGetTickCount();

        // Process any received messages from RX queue
        while (osMessageQueueGet(CANRxQueueHandle, &rx_msg, NULL, 0) == osOK) {
            CAN_ProcessRxMessage(&rx_msg);
        }

        // Process TX queue and send pending messages
        CAN_ProcessTxQueue();

        // Send heartbeat message at regular intervals
        if (current_tick - last_heartbeat_tick >= CAN_HEARTBEAT_INTERVAL_MS) {
            CAN_SendHeartbeat();
            last_heartbeat_tick = current_tick;
        }

        osDelay(10);
    }
}

/**
  * @brief  Initialize CAN manager
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef CAN_Manager_Init(void)
{
    // Create TX message queue
    CANTxQueueHandle = osMessageQueueNew(CAN_TX_QUEUE_SIZE, sizeof(CAN_Message_t), NULL);
    if (CANTxQueueHandle == NULL) {
        return HAL_ERROR;
    }

    // Create RX message queue
    CANRxQueueHandle = osMessageQueueNew(CAN_RX_QUEUE_SIZE, sizeof(CAN_Message_t), NULL);
    if (CANRxQueueHandle == NULL) {
        return HAL_ERROR;
    }

    // NOTE: CAN filter is now configured in MX_CAN1_Init() BEFORE HAL_CAN_Start()
    // This is critical - filters must be configured before starting CAN!

    // Activate CAN notifications (CAN must already be started)
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_GetNotificationFlags()) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;

}

/**
  * @brief  Add message to TX queue
  * @param  msg: Pointer to CAN message
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef CAN_SendMessage(CAN_Message_t* msg)
{
    // Add to TX queue (non-blocking with timeout in ms)
    if (osMessageQueuePut(CANTxQueueHandle, msg, 0, CAN_TX_TIMEOUT_MS) != osOK) {
        return HAL_ERROR;
    }

    return HAL_OK;

}

/**
  * @brief  Get full notification mask used by CAN manager
  * @retval Notification bitmask
  */
static uint32_t CAN_GetNotificationFlags(void)
{
    return (CAN_IT_RX_FIFO0_MSG_PENDING |
            CAN_IT_RX_FIFO1_MSG_PENDING |
            CAN_IT_ERROR |
            CAN_IT_BUSOFF |
            CAN_IT_ERROR_WARNING |
            CAN_IT_ERROR_PASSIVE |
            CAN_IT_LAST_ERROR_CODE);

}

/**
  * @brief  Process message from RX queue
  * @param  msg: Pointer to CAN message
  * @retval None
  */
static void CAN_ProcessRxMessage(CAN_Message_t *msg)
{
	uint32_t base_id = msg->id & 0xFF0FFFFF;

	// Check for configuration command message
    if (base_id == CAN_SET_BOARD_ID_CMD_BASE) {
        Config_SetBoardID(msg->data[0]);
        return;
    }

    // Check for reset command message
    if (base_id == CAN_RESET_CMD_BASE) {
    	// Reset command - trigger NVIC system reset
        NVIC_SystemReset();
        return;  // Never reached, but good practice
    }

}

/**
  * @brief  Transmit messages in TX queue
  * @retval None
  */
static void CAN_ProcessTxQueue(void)
{
    CAN_Message_t msg;

    // Try to send as many messages as possible
    while (osMessageQueueGet(CANTxQueueHandle, &msg, NULL, 0) == osOK) {
        // Transmit the message
        CAN_TxMessage(&msg);
    }

}

/**
  * @brief  Transmit single message to CAN hardware
  * @param  msg: Pointer to CAN message
  * @retval HAL_StatusTypeDef
  */
static HAL_StatusTypeDef CAN_TxMessage(CAN_Message_t* msg) {
    CAN_TxHeaderTypeDef header;
    uint32_t mailbox;
    HAL_StatusTypeDef status;

    header.IDE = CAN_ID_EXT;
    header.ExtId = msg->id;
    header.StdId = 0;
    header.RTR = CAN_RTR_DATA;
    header.DLC = CAN_MESSAGE_SIZE;
    header.TransmitGlobalTime = DISABLE;

    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

    status = HAL_CAN_AddTxMessage(&hcan1, &header, msg->data, &mailbox);

    return status;

}

/**
  * @brief  Send heartbeat message
  * @retval HAL_StatusTypeDef
  */
static HAL_StatusTypeDef CAN_SendHeartbeat(void)
{
    CAN_Message_t msg = {0};
    uint32_t current_tick = osKernelGetTickCount();

    msg.id = STATUS_CAN_ID;

    msg.data[0] = current_tick & 0xFF;
    msg.data[1] = (current_tick >> 8) & 0xFF;
    msg.data[2] = (current_tick >> 16) & 0xFF;
    msg.data[3] = current_tick >> 24;
    msg.data[4] = 0;
    msg.data[5] = 0;
    msg.data[6] = 0;
    msg.data[7] = 0;

    return CAN_SendMessage(&msg);

}

/**
  * @brief  Check if RX message is for this board
  * @param  can_id: ID of RX message
  * @retval True if for this board, false if not
  */
static bool CAN_IsMessageForThisBoard(uint32_t can_id)
{
	uint8_t this_board_id = Config_GetBoardID();
	uint8_t rx_board_id = (can_id >> 20) & 0x0F; // Board ID is encoded in bits 20:23 of RX CAN ID

    return (rx_board_id == this_board_id);

}

/**
  * @brief  CAN RX FIFO 0 message pending callback
  * @param  hcan: Pointer to CAN handle
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
    CAN_Message_t msg;

    // Get message from FIFO
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, msg.data) == HAL_OK) {
    	// Store message details (all messages are extended ID)
        msg.id = RxHeader.ExtId;

		if (CAN_IsMessageForThisBoard(msg.id)) {
			// Add to RX queue (from ISR context)
			osMessageQueuePut(CANRxQueueHandle, &msg, 0, 0);
		}
    }

}

/**
  * @brief  CAN RX FIFO 1 message pending callback
  * @param  hcan: Pointer to CAN handle
  * @retval None
  */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	CAN_Message_t msg;

	// Get message from FIFO
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, msg.data) == HAL_OK) {
		// Store message details (all messages are extended ID)
		msg.id = RxHeader.ExtId;

		if (CAN_IsMessageForThisBoard(msg.id)) {
			// Add to RX queue (from ISR context)
			osMessageQueuePut(CANRxQueueHandle, &msg, 0, 0);
		}
	}

}
