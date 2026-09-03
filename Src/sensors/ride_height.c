
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "ride_height.h"
#include "stm32l4xx_hal_uart.h"
#include "can_manager.h"

#include "cmsis_os.h"

/* Private Variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
uint8_t tof_data_buffer[8];
uint8_t uart_rx_byte;
uint8_t byte_idx = 0;

#define TOF_ENABLE_OUTPUT ((const uint8_t[]){ 0x5A, 0x05, 0x07, 0x01, 0x67  })
#define TOF_ENABLE_UART   ((const uint8_t[]){ 0x5A, 0x05, 0x0A, 0x00, 0x00  }) // SU=checksum but disabled by default
#define TOF_SAVE_FLASH    ((const uint8_t[]){ 0x5A, 0x04, 0x11, 0x6F, 0x00, })

/* Private Function Prototypes -----------------------------------------------*/

/* Function Implementations --------------------------------------------------*/
typedef struct {

}
/**
  * @brief  Initialize coolant temp data
  * @param  ct_data: Pointer to coolant temp data structure
  * @retval None
  */
void RideHeight_Init(RideHeight_Data_t* rh_data)
{
    // Ensure the ride height is in proper operation
	HAL_UART_Transmit(&huart2, TOF_ENABLE_OUTPUT, 5);
    HAL_UART_Transmit(&huart2, TOF_ENABLE_UART, 5);
    HAL_UART_Transmit(&huart2, TOF_SAVE_FLASH, 5);

    // Receive 1 byte at a time
    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance != USART2) {
        goto reset_interrupt;
    }

    tof_data_buffer[byte_idx] = uart_rx_byte;

    if (byte_idx == 0 && uart_rx_byte != 0x59) {
        goto reset_interrupt;
    }

    if (byte_idx == 1 && uart_rx_byte != 0x59) {
        byte_idx == 0;
        goto reset_interrupt;
    }

    byte_idx++;

    if (byte_idx == 9) {
        uint16_t dist = tof_data_buffer[3] << 8 & tof_data_buffer[2];
        uint16_t strength = tof_data_buffer[5] << 8 & tof_data_buffer[4];
        uint16_t temp = tof_data_buffer[7] << 8 & tof_data_buffer[6];

        printf("Distance: %u cm\n", (unsigned int) dist);

        CAN_Message_t msg;
        msg.id = 0x0DBFF000;

        msg.data[0] = tof_data_buffer[2];
        msg.data[1] = tof_data_buffer[3];

        // Strength
        msg.data[2] = tof_data_buffer[4];
        msg.data[3] = tof_data_buffer[5];

        // Temp??
        msg.data[4] = tof_data_buffer[6];
        msg.data[5] = tof_data_buffer[7];

        byte_idx = 0;

        CAN_SendMessage(&msg);
    }


reset_interrupt:
    HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);


}
