/**
  ******************************************************************************
  * @file           : config_manager.c
  * @brief          : DAQ board configuration manager implementation
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
#include "managers/config_manager.h"
#include "main.h"
#include "cmsis_os.h"
#include "boards/front_board.h"
#include "boards/left_board.h"
#include "boards/right_board.h"

/* Private Variables ---------------------------------------------------------*/
static osMutexId_t config_mutex = NULL;
static uint8_t BOARD_ID = 0;

/* Private Function Prototypes -----------------------------------------------*/
static uint8_t Config_ReadBoardIDFromFlash(void);
static int8_t Config_WriteBoardIDToFlash(uint8_t board_id);

/* Public Variables ----------------------------------------------------------*/
uint8_t NUM_SENSORS = 0;
uint32_t STATUS_CAN_ID = 0;

/* Function Implementations --------------------------------------------------*/

/**
  * @brief  Configure board based on board ID read from flash
  * @retval None
  */
void Config_Init(void)
{
    // Create mutex for thread-safe access
    const osMutexAttr_t mutex_attr = {
        .name = "ConfigMutex",
        .attr_bits = osMutexRecursive | osMutexPrioInherit,
        .cb_mem = NULL,
        .cb_size = 0U
    };

    config_mutex = osMutexNew(&mutex_attr);

    // Read board ID from flash storage
    BOARD_ID = Config_ReadBoardIDFromFlash();

    switch (BOARD_ID) {
		case DBF_BOARD_ID:
			DBF_Config();
			NUM_SENSORS = DBF_NUM_SENSORS;
			STATUS_CAN_ID = DBF_STATUS_CAN_ID;
			break;
		case DBL_BOARD_ID:
			DBL_Config();
			NUM_SENSORS = DBL_NUM_SENSORS;
			STATUS_CAN_ID = DBL_STATUS_CAN_ID;
			break;
		case DBR_BOARD_ID:
			DBR_Config();
			NUM_SENSORS = DBR_NUM_SENSORS;
			STATUS_CAN_ID = DBR_STATUS_CAN_ID;
			break;
    }
}

/**
  * @brief  Set board ID
  * @param  new_board_id: board ID to set
  * @retval 0 if successful, -1 if error
  */
int8_t Config_SetBoardID(uint8_t new_board_id)
{
	// Validate board ID range
	if ((new_board_id < CONFIG_BOARD_ID_MIN) || (new_board_id > CONFIG_BOARD_ID_MAX)) {
		return -1;
	}

	// Write to flash
	int8_t result = Config_WriteBoardIDToFlash(new_board_id);

	// Reset the microcontroller
	if (result == 0) {
		NVIC_SystemReset();
	}

	return 0;

}

/**
  * @brief  Get board ID
  * @retval Current board ID
  */
uint8_t Config_GetBoardID(void)
{
    uint8_t id;

    // Thread-safe read
    if (config_mutex != NULL) {
        osMutexAcquire(config_mutex, osWaitForever);
    }

    id = BOARD_ID;

    if (config_mutex != NULL) {
        osMutexRelease(config_mutex);
    }

    return id;

}

/**
  * @brief  Read board ID from flash
  * @retval Current board ID (or default if not set)
  */
static uint8_t Config_ReadBoardIDFromFlash(void)
{
    // Read 32-bit word from flash
    uint32_t flash_value = *(__IO uint32_t*)CONFIG_FLASH_BOARD_ADDR;

    // Check if magic value is present (upper 16 bits)
    if ((flash_value & CONFIG_FLASH_MAGIC_MASK) == CONFIG_FLASH_MAGIC) {
        // Extract board ID from lower 4 bits
        uint8_t stored_board_id = (uint8_t)(flash_value & CONFIG_FLASH_BOARD_MASK);

        // Validate range
        if ((stored_board_id >= CONFIG_BOARD_ID_MIN) && (stored_board_id <= CONFIG_BOARD_ID_MAX)) {
            return stored_board_id;
        }
    }

    // Flash uninitialized or invalid, return default
    return CONFIG_BOARD_ID_DEFAULT;
}

/**
  * @brief  Write board ID to flash
  * @param  board_id: Board ID to write
  * @retval 0 if successful, -1 if error
  */
static int8_t Config_WriteBoardIDToFlash(uint8_t board_id)
{
    HAL_StatusTypeDef status;

    // Validate board ID range
    if ((board_id < CONFIG_BOARD_ID_MIN) || (board_id > CONFIG_BOARD_ID_MAX)) {
        return -1;
    }

    // Prepare flash value: magic + board ID
    uint32_t flash_value = CONFIG_FLASH_MAGIC | (uint32_t)board_id;

    // Unlock flash
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return -1;
    }

    // Erase the page containing our address
    // STM32L432 has 2KB pages, need to erase page before writing
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error = 0;

    // Calculate page number (page size = 2048 bytes = 0x800)
    uint32_t page_address = CONFIG_FLASH_BOARD_ADDR;
    uint32_t page_number = (page_address - FLASH_BASE) / FLASH_PAGE_SIZE;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Page = page_number;
    erase_init.NbPages = 1;

    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    // Program the double-word (64-bit) - STM32L4 requires double-word programming
    // We'll write our 32-bit value twice to make 64 bits
    uint64_t data = ((uint64_t)flash_value << 32) | (uint64_t)flash_value;

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, CONFIG_FLASH_BOARD_ADDR, data);

    // Lock flash
    HAL_FLASH_Lock();

    return (status == HAL_OK) ? 0 : -1;

}
