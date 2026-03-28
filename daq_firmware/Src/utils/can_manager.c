#include "utils/can_manager.h"
#include "utils/config_manager.h"
#include "cmsis_os.h"

osMessageQueueId_t CANTxQueueHandle = NULL;
osMessageQueueId_t CANRxQueueHandle = NULL;

static uint32_t CAN_GetNotificationFlags(void);
static void CAN_ProcessRxMessage(CAN_Message_t *msg);
static void CAN_ProcessTxQueue(void);
static HAL_StatusTypeDef CAN_TxMessage(CAN_Message_t* msg);
static HAL_StatusTypeDef CAN_SendHeartbeat(void);

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

HAL_StatusTypeDef CAN_SendMessage(CAN_Message_t* msg)
{
    // Add to TX queue (non-blocking with timeout in ms)
    if (osMessageQueuePut(CANTxQueueHandle, msg, 0, CAN_TX_TIMEOUT_MS) != osOK) {
        return HAL_ERROR;
    }

    return HAL_OK;

}

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

static void CAN_ProcessRxMessage(CAN_Message_t *msg)
{
	// Check for configuration command message
    if (msg->id == CAN_SET_BOARD_ID_CMD) {
        Config_SetBoardID(msg->data[0]);
    }

}

static void CAN_ProcessTxQueue(void)
{
    CAN_Message_t msg;

    // Try to send as many messages as possible
    while (osMessageQueueGet(CANTxQueueHandle, &msg, NULL, 0) == osOK) {
        // Transmit the message
        CAN_TxMessage(&msg);
    }

}

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

static HAL_StatusTypeDef CAN_SendHeartbeat(void)
{
    CAN_Message_t msg = {0};
    msg.id = STATUS_CAN_ID;

    return CAN_SendMessage(&msg);

}

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
