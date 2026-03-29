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
#include "managers/config_manager.h"

/* Private Variables ---------------------------------------------------------*/
osMessageQueueId_t CANTxQueueHandle = NULL;
osMessageQueueId_t CANRxQueueHandle = NULL;

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t CAN_GetNotificationFlags(void);
static void CAN_ProcessRxMessage(CAN_Message_t *msg);
static void CAN_ProcessTxQueue(void);
static HAL_StatusTypeDef CAN_TxMessage(CAN_Message_t* msg);
static HAL_StatusTypeDef CAN_SendHeartbeat(void);

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
	// Check for configuration command message
    if (msg->id == CAN_SET_BOARD_ID_CMD) {
        Config_SetBoardID(msg->data[0]);
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
    msg.id = STATUS_CAN_ID;

    return CAN_SendMessage(&msg);

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

        // Add to RX queue (from ISR context)
        osMessageQueuePut(CANRxQueueHandle, &msg, 0, 0);
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

		// Add to RX queue (from ISR context)
		osMessageQueuePut(CANRxQueueHandle, &msg, 0, 0);
	}

}
