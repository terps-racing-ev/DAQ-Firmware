
/* Includes ------------------------------------------------------------------*/
#include "ride_height.h"

#define TOF_ENABLE_OUTPUT ((uint8_t[]){ 0x5A, 0x05, 0x07, 0x01, 0x67 })
#define TOF_ENABLE_UART   ((uint8_t[]){ 0x5A, 0x05, 0x0A, 0x00, 0x69 }) // SU = 0x69
#define TOF_SAVE_FLASH    ((uint8_t[]){ 0x5A, 0x04, 0x11, 0x6F })

/* Private Types --------------------------------------------------------*/
typedef struct {
    uint8_t data[9];
} TOF_RawFrame_t;

/* Private Variables ------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
static osMessageQueueId_t tofFrameQueueHandle;
static volatile uint8_t   tof_data_buffer[9];
static volatile uint8_t   byte_idx = 0;
static uint8_t            uart_rx_byte;

/* Private Function Prototypes -------------------------------------------*/
static void RideHeight_ProcessFrame(const uint8_t *frame);

/* Function Implementations ----------------------------------------------*/

void RideHeight_Init(void)
{
    // printf("Enabling ride height...\r\n");

    tofFrameQueueHandle = osMessageQueueNew(8, sizeof(TOF_RawFrame_t), NULL);
    if (tofFrameQueueHandle == NULL) {
        printf("Failed to create TOF frame queue\r\n");
    }

    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);

    HAL_UART_Transmit(&huart1, TOF_ENABLE_UART,   5, 100);
    HAL_UART_Transmit(&huart1, TOF_ENABLE_OUTPUT, 5, 100);
    HAL_UART_Transmit(&huart1, TOF_SAVE_FLASH,    4, 100);
}

/* ISR context: only framing + queue push. No parsing, no printf, no CAN. */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1) {
        goto reset_interrupt;
    }

    tof_data_buffer[byte_idx] = uart_rx_byte;

    if (byte_idx == 0 && uart_rx_byte != 0x59) {
        goto reset_interrupt;
    }

    if (byte_idx == 1 && uart_rx_byte != 0x59) {
        byte_idx = 0;
        goto reset_interrupt;
    }

    byte_idx++;

    if (byte_idx == 9) {
        TOF_RawFrame_t frame;
        memcpy(frame.data, (const void *)tof_data_buffer, sizeof(frame.data));

        // Non-blocking: never wait in an ISR. If the queue is full, drop the
        // frame rather than stalling UART reception.
        osMessageQueuePut(tofFrameQueueHandle, &frame, 0, 0);

        byte_idx = 0;
    }

reset_interrupt:
    HAL_UART_Receive_IT(huart, &uart_rx_byte, 1);
}

/* Dedicated task: pulls frames off the queue, parses, prints, sends CAN. */
void RideHeight_Task(void *argument)
{
    TOF_RawFrame_t frame;

    for (;;) {
        if (osMessageQueueGet(tofFrameQueueHandle, &frame, NULL, osWaitForever) == osOK) {
            RideHeight_ProcessFrame(frame.data);
        }
    }
}

static void RideHeight_ProcessFrame(const uint8_t *frame)
{
    // uint16_t dist     = (uint16_t)(frame[3] << 8) | frame[2];
    // uint16_t strength = (uint16_t)(frame[5] << 8) | frame[4];
    // uint16_t temp     = (uint16_t)(frame[7] << 8) | frame[6];
    // (void)temp; // silence unused-var warning until you actually use it

    // printf("Distance: %u cm\r\n", (unsigned int)dist);

    CAN_Message_t msg = {0};
    msg.id = 0x0DAA0000;

    // Distance
    msg.data[0] = frame[2];
    msg.data[1] = frame[3];

    // Strength
    msg.data[2] = frame[4];
    msg.data[3] = frame[5];

    // Temp
    msg.data[4] = frame[6];
    msg.data[5] = frame[7];

    CAN_SendMessage(&msg);
}