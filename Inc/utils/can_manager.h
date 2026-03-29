#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "main.h"
#include <stdint.h>

#define CAN_MESSAGE_SIZE 8
#define CAN_TX_QUEUE_SIZE 32
#define CAN_RX_QUEUE_SIZE 8
#define CAN_TX_TIMEOUT_MS 100
#define CAN_HEARTBEAT_INTERVAL_MS 1000

#define CAN_SET_BOARD_ID_CMD 0x00000BDC

extern CAN_HandleTypeDef hcan1;
extern uint32_t STATUS_CAN_ID;

typedef struct {

    uint32_t id;
    uint8_t data[CAN_MESSAGE_SIZE];

} CAN_Message_t;

void CAN_ManagerTask(void *argument);
HAL_StatusTypeDef CAN_Manager_Init(void);
HAL_StatusTypeDef CAN_SendMessage(CAN_Message_t* msg);

#endif // CAN_MANAGER_H
