#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>

#define CONFIG_BOARD_ID_MIN 			0x0A
#define CONFIG_BOARD_ID_MAX 			0x0C
#define CONFIG_BOARD_ID_DEFAULT 		0x00

#define CONFIG_FLASH_BOARD_ADDR 	0x0803C000
#define CONFIG_FLASH_MAGIC 			0xBEEF0000
#define CONFIG_FLASH_MAGIC_MASK 	0xFFFF0000
#define CONFIG_FLASH_BOARD_MASK 	0x0000000F

void Config_Init(void);
int8_t Config_SetBoardID(uint8_t new_board_id);
uint8_t Config_GetBoardID(void);

#endif // CONFIG_MANAGER_H
