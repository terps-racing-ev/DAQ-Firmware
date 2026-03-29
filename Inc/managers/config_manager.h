/**
  ******************************************************************************
  * @file           : config_manager.h
  * @brief          : DAQ board configuration manager
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

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define CONFIG_BOARD_ID_MIN 			0x0A
#define CONFIG_BOARD_ID_MAX 			0x0C
#define CONFIG_BOARD_ID_DEFAULT 		0x00

#define CONFIG_FLASH_BOARD_ADDR 	0x0803C000
#define CONFIG_FLASH_MAGIC 			0xBEEF0000
#define CONFIG_FLASH_MAGIC_MASK 	0xFFFF0000
#define CONFIG_FLASH_BOARD_MASK 	0x0000000F

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Configure board based on board ID read from flash
  * @retval None
  */
void Config_Init(void);

/**
  * @brief  Set board ID
  * @param  new_board_id: board ID to set
  * @retval 0 if successful, -1 if error
  */
int8_t Config_SetBoardID(uint8_t new_board_id);

/**
  * @brief  Get board ID
  * @retval Current board ID
  */
uint8_t Config_GetBoardID(void);

#endif // CONFIG_MANAGER_H
